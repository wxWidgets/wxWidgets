# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import __core

def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

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
        if hasattr(self,name) or (name in ("this", "thisown")):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


#// Give a reference to the dictionary of this module to the C++ extension
#// code.
_core_._wxPySetDictionary(vars())

#// A little trick to make 'wx' be a reference to this module so wx.Names can
#// be used here.
import sys as _sys
wx = _sys.modules[__name__]


#----------------------------------------------------------------------------

def _deprecated(callable, msg=None):
    """
    Create a wrapper function that will raise a DeprecationWarning
    before calling the callable.
    """
    if msg is None:
        msg = "%s is deprecated" % callable
    def deprecatedWrapper(*args, **kwargs):
        import warnings
        warnings.warn(msg, DeprecationWarning, stacklevel=2)
        return callable(*args, **kwargs)
    deprecatedWrapper.__doc__ = msg
    return deprecatedWrapper
    
                   
#----------------------------------------------------------------------------

wxNOT_FOUND = __core.wxNOT_FOUND
wxVSCROLL = __core.wxVSCROLL
wxHSCROLL = __core.wxHSCROLL
wxCAPTION = __core.wxCAPTION
wxDOUBLE_BORDER = __core.wxDOUBLE_BORDER
wxSUNKEN_BORDER = __core.wxSUNKEN_BORDER
wxRAISED_BORDER = __core.wxRAISED_BORDER
wxBORDER = __core.wxBORDER
wxSIMPLE_BORDER = __core.wxSIMPLE_BORDER
wxSTATIC_BORDER = __core.wxSTATIC_BORDER
wxTRANSPARENT_WINDOW = __core.wxTRANSPARENT_WINDOW
wxNO_BORDER = __core.wxNO_BORDER
wxTAB_TRAVERSAL = __core.wxTAB_TRAVERSAL
wxWANTS_CHARS = __core.wxWANTS_CHARS
wxPOPUP_WINDOW = __core.wxPOPUP_WINDOW
wxCENTER_FRAME = __core.wxCENTER_FRAME
wxCENTRE_ON_SCREEN = __core.wxCENTRE_ON_SCREEN
wxCENTER_ON_SCREEN = __core.wxCENTER_ON_SCREEN
wxCLIP_CHILDREN = __core.wxCLIP_CHILDREN
wxCLIP_SIBLINGS = __core.wxCLIP_SIBLINGS
wxALWAYS_SHOW_SB = __core.wxALWAYS_SHOW_SB
wxRETAINED = __core.wxRETAINED
wxBACKINGSTORE = __core.wxBACKINGSTORE
wxCOLOURED = __core.wxCOLOURED
wxFIXED_LENGTH = __core.wxFIXED_LENGTH
wxLB_NEEDED_SB = __core.wxLB_NEEDED_SB
wxLB_ALWAYS_SB = __core.wxLB_ALWAYS_SB
wxLB_SORT = __core.wxLB_SORT
wxLB_SINGLE = __core.wxLB_SINGLE
wxLB_MULTIPLE = __core.wxLB_MULTIPLE
wxLB_EXTENDED = __core.wxLB_EXTENDED
wxLB_OWNERDRAW = __core.wxLB_OWNERDRAW
wxLB_HSCROLL = __core.wxLB_HSCROLL
wxPROCESS_ENTER = __core.wxPROCESS_ENTER
wxPASSWORD = __core.wxPASSWORD
wxCB_SIMPLE = __core.wxCB_SIMPLE
wxCB_DROPDOWN = __core.wxCB_DROPDOWN
wxCB_SORT = __core.wxCB_SORT
wxCB_READONLY = __core.wxCB_READONLY
wxRA_HORIZONTAL = __core.wxRA_HORIZONTAL
wxRA_VERTICAL = __core.wxRA_VERTICAL
wxRA_SPECIFY_ROWS = __core.wxRA_SPECIFY_ROWS
wxRA_SPECIFY_COLS = __core.wxRA_SPECIFY_COLS
wxRA_USE_CHECKBOX = __core.wxRA_USE_CHECKBOX
wxRB_GROUP = __core.wxRB_GROUP
wxRB_SINGLE = __core.wxRB_SINGLE
wxSB_HORIZONTAL = __core.wxSB_HORIZONTAL
wxSB_VERTICAL = __core.wxSB_VERTICAL
wxRB_USE_CHECKBOX = __core.wxRB_USE_CHECKBOX
wxST_SIZEGRIP = __core.wxST_SIZEGRIP
wxST_NO_AUTORESIZE = __core.wxST_NO_AUTORESIZE
wxFLOOD_SURFACE = __core.wxFLOOD_SURFACE
wxFLOOD_BORDER = __core.wxFLOOD_BORDER
wxODDEVEN_RULE = __core.wxODDEVEN_RULE
wxWINDING_RULE = __core.wxWINDING_RULE
wxTOOL_TOP = __core.wxTOOL_TOP
wxTOOL_BOTTOM = __core.wxTOOL_BOTTOM
wxTOOL_LEFT = __core.wxTOOL_LEFT
wxTOOL_RIGHT = __core.wxTOOL_RIGHT
wxOK = __core.wxOK
wxYES_NO = __core.wxYES_NO
wxCANCEL = __core.wxCANCEL
wxYES = __core.wxYES
wxNO = __core.wxNO
wxNO_DEFAULT = __core.wxNO_DEFAULT
wxYES_DEFAULT = __core.wxYES_DEFAULT
wxICON_EXCLAMATION = __core.wxICON_EXCLAMATION
wxICON_HAND = __core.wxICON_HAND
wxICON_QUESTION = __core.wxICON_QUESTION
wxICON_INFORMATION = __core.wxICON_INFORMATION
wxICON_STOP = __core.wxICON_STOP
wxICON_ASTERISK = __core.wxICON_ASTERISK
wxICON_MASK = __core.wxICON_MASK
wxICON_WARNING = __core.wxICON_WARNING
wxICON_ERROR = __core.wxICON_ERROR
wxFORWARD = __core.wxFORWARD
wxBACKWARD = __core.wxBACKWARD
wxRESET = __core.wxRESET
wxHELP = __core.wxHELP
wxMORE = __core.wxMORE
wxSETUP = __core.wxSETUP
wxSIZE_AUTO_WIDTH = __core.wxSIZE_AUTO_WIDTH
wxSIZE_AUTO_HEIGHT = __core.wxSIZE_AUTO_HEIGHT
wxSIZE_AUTO = __core.wxSIZE_AUTO
wxSIZE_USE_EXISTING = __core.wxSIZE_USE_EXISTING
wxSIZE_ALLOW_MINUS_ONE = __core.wxSIZE_ALLOW_MINUS_ONE
wxPORTRAIT = __core.wxPORTRAIT
wxLANDSCAPE = __core.wxLANDSCAPE
wxPRINT_QUALITY_HIGH = __core.wxPRINT_QUALITY_HIGH
wxPRINT_QUALITY_MEDIUM = __core.wxPRINT_QUALITY_MEDIUM
wxPRINT_QUALITY_LOW = __core.wxPRINT_QUALITY_LOW
wxPRINT_QUALITY_DRAFT = __core.wxPRINT_QUALITY_DRAFT
wxID_ANY = __core.wxID_ANY
wxID_SEPARATOR = __core.wxID_SEPARATOR
wxID_LOWEST = __core.wxID_LOWEST
wxID_OPEN = __core.wxID_OPEN
wxID_CLOSE = __core.wxID_CLOSE
wxID_NEW = __core.wxID_NEW
wxID_SAVE = __core.wxID_SAVE
wxID_SAVEAS = __core.wxID_SAVEAS
wxID_REVERT = __core.wxID_REVERT
wxID_EXIT = __core.wxID_EXIT
wxID_UNDO = __core.wxID_UNDO
wxID_REDO = __core.wxID_REDO
wxID_HELP = __core.wxID_HELP
wxID_PRINT = __core.wxID_PRINT
wxID_PRINT_SETUP = __core.wxID_PRINT_SETUP
wxID_PREVIEW = __core.wxID_PREVIEW
wxID_ABOUT = __core.wxID_ABOUT
wxID_HELP_CONTENTS = __core.wxID_HELP_CONTENTS
wxID_HELP_COMMANDS = __core.wxID_HELP_COMMANDS
wxID_HELP_PROCEDURES = __core.wxID_HELP_PROCEDURES
wxID_HELP_CONTEXT = __core.wxID_HELP_CONTEXT
wxID_CLOSE_ALL = __core.wxID_CLOSE_ALL
wxID_PREFERENCES = __core.wxID_PREFERENCES
wxID_CUT = __core.wxID_CUT
wxID_COPY = __core.wxID_COPY
wxID_PASTE = __core.wxID_PASTE
wxID_CLEAR = __core.wxID_CLEAR
wxID_FIND = __core.wxID_FIND
wxID_DUPLICATE = __core.wxID_DUPLICATE
wxID_SELECTALL = __core.wxID_SELECTALL
wxID_DELETE = __core.wxID_DELETE
wxID_REPLACE = __core.wxID_REPLACE
wxID_REPLACE_ALL = __core.wxID_REPLACE_ALL
wxID_PROPERTIES = __core.wxID_PROPERTIES
wxID_VIEW_DETAILS = __core.wxID_VIEW_DETAILS
wxID_VIEW_LARGEICONS = __core.wxID_VIEW_LARGEICONS
wxID_VIEW_SMALLICONS = __core.wxID_VIEW_SMALLICONS
wxID_VIEW_LIST = __core.wxID_VIEW_LIST
wxID_VIEW_SORTDATE = __core.wxID_VIEW_SORTDATE
wxID_VIEW_SORTNAME = __core.wxID_VIEW_SORTNAME
wxID_VIEW_SORTSIZE = __core.wxID_VIEW_SORTSIZE
wxID_VIEW_SORTTYPE = __core.wxID_VIEW_SORTTYPE
wxID_FILE1 = __core.wxID_FILE1
wxID_FILE2 = __core.wxID_FILE2
wxID_FILE3 = __core.wxID_FILE3
wxID_FILE4 = __core.wxID_FILE4
wxID_FILE5 = __core.wxID_FILE5
wxID_FILE6 = __core.wxID_FILE6
wxID_FILE7 = __core.wxID_FILE7
wxID_FILE8 = __core.wxID_FILE8
wxID_FILE9 = __core.wxID_FILE9
wxID_OK = __core.wxID_OK
wxID_CANCEL = __core.wxID_CANCEL
wxID_APPLY = __core.wxID_APPLY
wxID_YES = __core.wxID_YES
wxID_NO = __core.wxID_NO
wxID_STATIC = __core.wxID_STATIC
wxID_FORWARD = __core.wxID_FORWARD
wxID_BACKWARD = __core.wxID_BACKWARD
wxID_DEFAULT = __core.wxID_DEFAULT
wxID_MORE = __core.wxID_MORE
wxID_SETUP = __core.wxID_SETUP
wxID_RESET = __core.wxID_RESET
wxID_CONTEXT_HELP = __core.wxID_CONTEXT_HELP
wxID_YESTOALL = __core.wxID_YESTOALL
wxID_NOTOALL = __core.wxID_NOTOALL
wxID_ABORT = __core.wxID_ABORT
wxID_RETRY = __core.wxID_RETRY
wxID_IGNORE = __core.wxID_IGNORE
wxID_ADD = __core.wxID_ADD
wxID_REMOVE = __core.wxID_REMOVE
wxID_UP = __core.wxID_UP
wxID_DOWN = __core.wxID_DOWN
wxID_HOME = __core.wxID_HOME
wxID_REFRESH = __core.wxID_REFRESH
wxID_STOP = __core.wxID_STOP
wxID_INDEX = __core.wxID_INDEX
wxID_BOLD = __core.wxID_BOLD
wxID_ITALIC = __core.wxID_ITALIC
wxID_JUSTIFY_CENTER = __core.wxID_JUSTIFY_CENTER
wxID_JUSTIFY_FILL = __core.wxID_JUSTIFY_FILL
wxID_JUSTIFY_RIGHT = __core.wxID_JUSTIFY_RIGHT
wxID_JUSTIFY_LEFT = __core.wxID_JUSTIFY_LEFT
wxID_UNDERLINE = __core.wxID_UNDERLINE
wxID_INDENT = __core.wxID_INDENT
wxID_UNINDENT = __core.wxID_UNINDENT
wxID_ZOOM_100 = __core.wxID_ZOOM_100
wxID_ZOOM_FIT = __core.wxID_ZOOM_FIT
wxID_ZOOM_IN = __core.wxID_ZOOM_IN
wxID_ZOOM_OUT = __core.wxID_ZOOM_OUT
wxID_UNDELETE = __core.wxID_UNDELETE
wxID_REVERT_TO_SAVED = __core.wxID_REVERT_TO_SAVED
wxID_HIGHEST = __core.wxID_HIGHEST
wxOPEN = __core.wxOPEN
wxSAVE = __core.wxSAVE
wxHIDE_READONLY = __core.wxHIDE_READONLY
wxOVERWRITE_PROMPT = __core.wxOVERWRITE_PROMPT
wxFILE_MUST_EXIST = __core.wxFILE_MUST_EXIST
wxMULTIPLE = __core.wxMULTIPLE
wxCHANGE_DIR = __core.wxCHANGE_DIR
wxACCEL_ALT = __core.wxACCEL_ALT
wxACCEL_CTRL = __core.wxACCEL_CTRL
wxACCEL_SHIFT = __core.wxACCEL_SHIFT
wxACCEL_NORMAL = __core.wxACCEL_NORMAL
wxPD_AUTO_HIDE = __core.wxPD_AUTO_HIDE
wxPD_APP_MODAL = __core.wxPD_APP_MODAL
wxPD_CAN_ABORT = __core.wxPD_CAN_ABORT
wxPD_ELAPSED_TIME = __core.wxPD_ELAPSED_TIME
wxPD_ESTIMATED_TIME = __core.wxPD_ESTIMATED_TIME
wxPD_REMAINING_TIME = __core.wxPD_REMAINING_TIME
wxPD_SMOOTH = __core.wxPD_SMOOTH
wxPD_CAN_SKIP = __core.wxPD_CAN_SKIP
wxDD_NEW_DIR_BUTTON = __core.wxDD_NEW_DIR_BUTTON
wxDD_DEFAULT_STYLE = __core.wxDD_DEFAULT_STYLE
wxMENU_TEAROFF = __core.wxMENU_TEAROFF
wxMB_DOCKABLE = __core.wxMB_DOCKABLE
wxNO_FULL_REPAINT_ON_RESIZE = __core.wxNO_FULL_REPAINT_ON_RESIZE
wxFULL_REPAINT_ON_RESIZE = __core.wxFULL_REPAINT_ON_RESIZE
wxLI_HORIZONTAL = __core.wxLI_HORIZONTAL
wxLI_VERTICAL = __core.wxLI_VERTICAL
wxWS_EX_VALIDATE_RECURSIVELY = __core.wxWS_EX_VALIDATE_RECURSIVELY
wxWS_EX_BLOCK_EVENTS = __core.wxWS_EX_BLOCK_EVENTS
wxWS_EX_TRANSIENT = __core.wxWS_EX_TRANSIENT
wxWS_EX_THEMED_BACKGROUND = __core.wxWS_EX_THEMED_BACKGROUND
wxWS_EX_PROCESS_IDLE = __core.wxWS_EX_PROCESS_IDLE
wxWS_EX_PROCESS_UI_UPDATES = __core.wxWS_EX_PROCESS_UI_UPDATES
wxMM_TEXT = __core.wxMM_TEXT
wxMM_LOMETRIC = __core.wxMM_LOMETRIC
wxMM_HIMETRIC = __core.wxMM_HIMETRIC
wxMM_LOENGLISH = __core.wxMM_LOENGLISH
wxMM_HIENGLISH = __core.wxMM_HIENGLISH
wxMM_TWIPS = __core.wxMM_TWIPS
wxMM_ISOTROPIC = __core.wxMM_ISOTROPIC
wxMM_ANISOTROPIC = __core.wxMM_ANISOTROPIC
wxMM_POINTS = __core.wxMM_POINTS
wxMM_METRIC = __core.wxMM_METRIC
wxCENTRE = __core.wxCENTRE
wxCENTER = __core.wxCENTER
wxHORIZONTAL = __core.wxHORIZONTAL
wxVERTICAL = __core.wxVERTICAL
wxBOTH = __core.wxBOTH
wxLEFT = __core.wxLEFT
wxRIGHT = __core.wxRIGHT
wxUP = __core.wxUP
wxDOWN = __core.wxDOWN
wxTOP = __core.wxTOP
wxBOTTOM = __core.wxBOTTOM
wxNORTH = __core.wxNORTH
wxSOUTH = __core.wxSOUTH
wxWEST = __core.wxWEST
wxEAST = __core.wxEAST
wxALL = __core.wxALL
wxALIGN_NOT = __core.wxALIGN_NOT
wxALIGN_CENTER_HORIZONTAL = __core.wxALIGN_CENTER_HORIZONTAL
wxALIGN_CENTRE_HORIZONTAL = __core.wxALIGN_CENTRE_HORIZONTAL
wxALIGN_LEFT = __core.wxALIGN_LEFT
wxALIGN_TOP = __core.wxALIGN_TOP
wxALIGN_RIGHT = __core.wxALIGN_RIGHT
wxALIGN_BOTTOM = __core.wxALIGN_BOTTOM
wxALIGN_CENTER_VERTICAL = __core.wxALIGN_CENTER_VERTICAL
wxALIGN_CENTRE_VERTICAL = __core.wxALIGN_CENTRE_VERTICAL
wxALIGN_CENTER = __core.wxALIGN_CENTER
wxALIGN_CENTRE = __core.wxALIGN_CENTRE
wxALIGN_MASK = __core.wxALIGN_MASK
wxSTRETCH_NOT = __core.wxSTRETCH_NOT
wxSHRINK = __core.wxSHRINK
wxGROW = __core.wxGROW
wxEXPAND = __core.wxEXPAND
wxSHAPED = __core.wxSHAPED
wxFIXED_MINSIZE = __core.wxFIXED_MINSIZE
wxTILE = __core.wxTILE
wxADJUST_MINSIZE = __core.wxADJUST_MINSIZE
wxBORDER_DEFAULT = __core.wxBORDER_DEFAULT
wxBORDER_NONE = __core.wxBORDER_NONE
wxBORDER_STATIC = __core.wxBORDER_STATIC
wxBORDER_SIMPLE = __core.wxBORDER_SIMPLE
wxBORDER_RAISED = __core.wxBORDER_RAISED
wxBORDER_SUNKEN = __core.wxBORDER_SUNKEN
wxBORDER_DOUBLE = __core.wxBORDER_DOUBLE
wxBORDER_MASK = __core.wxBORDER_MASK
wxBG_STYLE_SYSTEM = __core.wxBG_STYLE_SYSTEM
wxBG_STYLE_COLOUR = __core.wxBG_STYLE_COLOUR
wxBG_STYLE_CUSTOM = __core.wxBG_STYLE_CUSTOM
wxDEFAULT = __core.wxDEFAULT
wxDECORATIVE = __core.wxDECORATIVE
wxROMAN = __core.wxROMAN
wxSCRIPT = __core.wxSCRIPT
wxSWISS = __core.wxSWISS
wxMODERN = __core.wxMODERN
wxTELETYPE = __core.wxTELETYPE
wxVARIABLE = __core.wxVARIABLE
wxFIXED = __core.wxFIXED
wxNORMAL = __core.wxNORMAL
wxLIGHT = __core.wxLIGHT
wxBOLD = __core.wxBOLD
wxITALIC = __core.wxITALIC
wxSLANT = __core.wxSLANT
wxSOLID = __core.wxSOLID
wxDOT = __core.wxDOT
wxLONG_DASH = __core.wxLONG_DASH
wxSHORT_DASH = __core.wxSHORT_DASH
wxDOT_DASH = __core.wxDOT_DASH
wxUSER_DASH = __core.wxUSER_DASH
wxTRANSPARENT = __core.wxTRANSPARENT
wxSTIPPLE = __core.wxSTIPPLE
wxBDIAGONAL_HATCH = __core.wxBDIAGONAL_HATCH
wxCROSSDIAG_HATCH = __core.wxCROSSDIAG_HATCH
wxFDIAGONAL_HATCH = __core.wxFDIAGONAL_HATCH
wxCROSS_HATCH = __core.wxCROSS_HATCH
wxHORIZONTAL_HATCH = __core.wxHORIZONTAL_HATCH
wxVERTICAL_HATCH = __core.wxVERTICAL_HATCH
wxJOIN_BEVEL = __core.wxJOIN_BEVEL
wxJOIN_MITER = __core.wxJOIN_MITER
wxJOIN_ROUND = __core.wxJOIN_ROUND
wxCAP_ROUND = __core.wxCAP_ROUND
wxCAP_PROJECTING = __core.wxCAP_PROJECTING
wxCAP_BUTT = __core.wxCAP_BUTT
wxCLEAR = __core.wxCLEAR
wxXOR = __core.wxXOR
wxINVERT = __core.wxINVERT
wxOR_REVERSE = __core.wxOR_REVERSE
wxAND_REVERSE = __core.wxAND_REVERSE
wxCOPY = __core.wxCOPY
wxAND = __core.wxAND
wxAND_INVERT = __core.wxAND_INVERT
wxNO_OP = __core.wxNO_OP
wxNOR = __core.wxNOR
wxEQUIV = __core.wxEQUIV
wxSRC_INVERT = __core.wxSRC_INVERT
wxOR_INVERT = __core.wxOR_INVERT
wxNAND = __core.wxNAND
wxOR = __core.wxOR
wxSET = __core.wxSET
WXK_BACK = __core.WXK_BACK
WXK_TAB = __core.WXK_TAB
WXK_RETURN = __core.WXK_RETURN
WXK_ESCAPE = __core.WXK_ESCAPE
WXK_SPACE = __core.WXK_SPACE
WXK_DELETE = __core.WXK_DELETE
WXK_START = __core.WXK_START
WXK_LBUTTON = __core.WXK_LBUTTON
WXK_RBUTTON = __core.WXK_RBUTTON
WXK_CANCEL = __core.WXK_CANCEL
WXK_MBUTTON = __core.WXK_MBUTTON
WXK_CLEAR = __core.WXK_CLEAR
WXK_SHIFT = __core.WXK_SHIFT
WXK_ALT = __core.WXK_ALT
WXK_CONTROL = __core.WXK_CONTROL
WXK_MENU = __core.WXK_MENU
WXK_PAUSE = __core.WXK_PAUSE
WXK_CAPITAL = __core.WXK_CAPITAL
WXK_PRIOR = __core.WXK_PRIOR
WXK_NEXT = __core.WXK_NEXT
WXK_END = __core.WXK_END
WXK_HOME = __core.WXK_HOME
WXK_LEFT = __core.WXK_LEFT
WXK_UP = __core.WXK_UP
WXK_RIGHT = __core.WXK_RIGHT
WXK_DOWN = __core.WXK_DOWN
WXK_SELECT = __core.WXK_SELECT
WXK_PRINT = __core.WXK_PRINT
WXK_EXECUTE = __core.WXK_EXECUTE
WXK_SNAPSHOT = __core.WXK_SNAPSHOT
WXK_INSERT = __core.WXK_INSERT
WXK_HELP = __core.WXK_HELP
WXK_NUMPAD0 = __core.WXK_NUMPAD0
WXK_NUMPAD1 = __core.WXK_NUMPAD1
WXK_NUMPAD2 = __core.WXK_NUMPAD2
WXK_NUMPAD3 = __core.WXK_NUMPAD3
WXK_NUMPAD4 = __core.WXK_NUMPAD4
WXK_NUMPAD5 = __core.WXK_NUMPAD5
WXK_NUMPAD6 = __core.WXK_NUMPAD6
WXK_NUMPAD7 = __core.WXK_NUMPAD7
WXK_NUMPAD8 = __core.WXK_NUMPAD8
WXK_NUMPAD9 = __core.WXK_NUMPAD9
WXK_MULTIPLY = __core.WXK_MULTIPLY
WXK_ADD = __core.WXK_ADD
WXK_SEPARATOR = __core.WXK_SEPARATOR
WXK_SUBTRACT = __core.WXK_SUBTRACT
WXK_DECIMAL = __core.WXK_DECIMAL
WXK_DIVIDE = __core.WXK_DIVIDE
WXK_F1 = __core.WXK_F1
WXK_F2 = __core.WXK_F2
WXK_F3 = __core.WXK_F3
WXK_F4 = __core.WXK_F4
WXK_F5 = __core.WXK_F5
WXK_F6 = __core.WXK_F6
WXK_F7 = __core.WXK_F7
WXK_F8 = __core.WXK_F8
WXK_F9 = __core.WXK_F9
WXK_F10 = __core.WXK_F10
WXK_F11 = __core.WXK_F11
WXK_F12 = __core.WXK_F12
WXK_F13 = __core.WXK_F13
WXK_F14 = __core.WXK_F14
WXK_F15 = __core.WXK_F15
WXK_F16 = __core.WXK_F16
WXK_F17 = __core.WXK_F17
WXK_F18 = __core.WXK_F18
WXK_F19 = __core.WXK_F19
WXK_F20 = __core.WXK_F20
WXK_F21 = __core.WXK_F21
WXK_F22 = __core.WXK_F22
WXK_F23 = __core.WXK_F23
WXK_F24 = __core.WXK_F24
WXK_NUMLOCK = __core.WXK_NUMLOCK
WXK_SCROLL = __core.WXK_SCROLL
WXK_PAGEUP = __core.WXK_PAGEUP
WXK_PAGEDOWN = __core.WXK_PAGEDOWN
WXK_NUMPAD_SPACE = __core.WXK_NUMPAD_SPACE
WXK_NUMPAD_TAB = __core.WXK_NUMPAD_TAB
WXK_NUMPAD_ENTER = __core.WXK_NUMPAD_ENTER
WXK_NUMPAD_F1 = __core.WXK_NUMPAD_F1
WXK_NUMPAD_F2 = __core.WXK_NUMPAD_F2
WXK_NUMPAD_F3 = __core.WXK_NUMPAD_F3
WXK_NUMPAD_F4 = __core.WXK_NUMPAD_F4
WXK_NUMPAD_HOME = __core.WXK_NUMPAD_HOME
WXK_NUMPAD_LEFT = __core.WXK_NUMPAD_LEFT
WXK_NUMPAD_UP = __core.WXK_NUMPAD_UP
WXK_NUMPAD_RIGHT = __core.WXK_NUMPAD_RIGHT
WXK_NUMPAD_DOWN = __core.WXK_NUMPAD_DOWN
WXK_NUMPAD_PRIOR = __core.WXK_NUMPAD_PRIOR
WXK_NUMPAD_PAGEUP = __core.WXK_NUMPAD_PAGEUP
WXK_NUMPAD_NEXT = __core.WXK_NUMPAD_NEXT
WXK_NUMPAD_PAGEDOWN = __core.WXK_NUMPAD_PAGEDOWN
WXK_NUMPAD_END = __core.WXK_NUMPAD_END
WXK_NUMPAD_BEGIN = __core.WXK_NUMPAD_BEGIN
WXK_NUMPAD_INSERT = __core.WXK_NUMPAD_INSERT
WXK_NUMPAD_DELETE = __core.WXK_NUMPAD_DELETE
WXK_NUMPAD_EQUAL = __core.WXK_NUMPAD_EQUAL
WXK_NUMPAD_MULTIPLY = __core.WXK_NUMPAD_MULTIPLY
WXK_NUMPAD_ADD = __core.WXK_NUMPAD_ADD
WXK_NUMPAD_SEPARATOR = __core.WXK_NUMPAD_SEPARATOR
WXK_NUMPAD_SUBTRACT = __core.WXK_NUMPAD_SUBTRACT
WXK_NUMPAD_DECIMAL = __core.WXK_NUMPAD_DECIMAL
WXK_NUMPAD_DIVIDE = __core.WXK_NUMPAD_DIVIDE
WXK_WINDOWS_LEFT = __core.WXK_WINDOWS_LEFT
WXK_WINDOWS_RIGHT = __core.WXK_WINDOWS_RIGHT
WXK_WINDOWS_MENU = __core.WXK_WINDOWS_MENU
WXK_COMMAND = __core.WXK_COMMAND
WXK_SPECIAL1 = __core.WXK_SPECIAL1
WXK_SPECIAL2 = __core.WXK_SPECIAL2
WXK_SPECIAL3 = __core.WXK_SPECIAL3
WXK_SPECIAL4 = __core.WXK_SPECIAL4
WXK_SPECIAL5 = __core.WXK_SPECIAL5
WXK_SPECIAL6 = __core.WXK_SPECIAL6
WXK_SPECIAL7 = __core.WXK_SPECIAL7
WXK_SPECIAL8 = __core.WXK_SPECIAL8
WXK_SPECIAL9 = __core.WXK_SPECIAL9
WXK_SPECIAL10 = __core.WXK_SPECIAL10
WXK_SPECIAL11 = __core.WXK_SPECIAL11
WXK_SPECIAL12 = __core.WXK_SPECIAL12
WXK_SPECIAL13 = __core.WXK_SPECIAL13
WXK_SPECIAL14 = __core.WXK_SPECIAL14
WXK_SPECIAL15 = __core.WXK_SPECIAL15
WXK_SPECIAL16 = __core.WXK_SPECIAL16
WXK_SPECIAL17 = __core.WXK_SPECIAL17
WXK_SPECIAL18 = __core.WXK_SPECIAL18
WXK_SPECIAL19 = __core.WXK_SPECIAL19
WXK_SPECIAL20 = __core.WXK_SPECIAL20
wxPAPER_NONE = __core.wxPAPER_NONE
wxPAPER_LETTER = __core.wxPAPER_LETTER
wxPAPER_LEGAL = __core.wxPAPER_LEGAL
wxPAPER_A4 = __core.wxPAPER_A4
wxPAPER_CSHEET = __core.wxPAPER_CSHEET
wxPAPER_DSHEET = __core.wxPAPER_DSHEET
wxPAPER_ESHEET = __core.wxPAPER_ESHEET
wxPAPER_LETTERSMALL = __core.wxPAPER_LETTERSMALL
wxPAPER_TABLOID = __core.wxPAPER_TABLOID
wxPAPER_LEDGER = __core.wxPAPER_LEDGER
wxPAPER_STATEMENT = __core.wxPAPER_STATEMENT
wxPAPER_EXECUTIVE = __core.wxPAPER_EXECUTIVE
wxPAPER_A3 = __core.wxPAPER_A3
wxPAPER_A4SMALL = __core.wxPAPER_A4SMALL
wxPAPER_A5 = __core.wxPAPER_A5
wxPAPER_B4 = __core.wxPAPER_B4
wxPAPER_B5 = __core.wxPAPER_B5
wxPAPER_FOLIO = __core.wxPAPER_FOLIO
wxPAPER_QUARTO = __core.wxPAPER_QUARTO
wxPAPER_10X14 = __core.wxPAPER_10X14
wxPAPER_11X17 = __core.wxPAPER_11X17
wxPAPER_NOTE = __core.wxPAPER_NOTE
wxPAPER_ENV_9 = __core.wxPAPER_ENV_9
wxPAPER_ENV_10 = __core.wxPAPER_ENV_10
wxPAPER_ENV_11 = __core.wxPAPER_ENV_11
wxPAPER_ENV_12 = __core.wxPAPER_ENV_12
wxPAPER_ENV_14 = __core.wxPAPER_ENV_14
wxPAPER_ENV_DL = __core.wxPAPER_ENV_DL
wxPAPER_ENV_C5 = __core.wxPAPER_ENV_C5
wxPAPER_ENV_C3 = __core.wxPAPER_ENV_C3
wxPAPER_ENV_C4 = __core.wxPAPER_ENV_C4
wxPAPER_ENV_C6 = __core.wxPAPER_ENV_C6
wxPAPER_ENV_C65 = __core.wxPAPER_ENV_C65
wxPAPER_ENV_B4 = __core.wxPAPER_ENV_B4
wxPAPER_ENV_B5 = __core.wxPAPER_ENV_B5
wxPAPER_ENV_B6 = __core.wxPAPER_ENV_B6
wxPAPER_ENV_ITALY = __core.wxPAPER_ENV_ITALY
wxPAPER_ENV_MONARCH = __core.wxPAPER_ENV_MONARCH
wxPAPER_ENV_PERSONAL = __core.wxPAPER_ENV_PERSONAL
wxPAPER_FANFOLD_US = __core.wxPAPER_FANFOLD_US
wxPAPER_FANFOLD_STD_GERMAN = __core.wxPAPER_FANFOLD_STD_GERMAN
wxPAPER_FANFOLD_LGL_GERMAN = __core.wxPAPER_FANFOLD_LGL_GERMAN
wxPAPER_ISO_B4 = __core.wxPAPER_ISO_B4
wxPAPER_JAPANESE_POSTCARD = __core.wxPAPER_JAPANESE_POSTCARD
wxPAPER_9X11 = __core.wxPAPER_9X11
wxPAPER_10X11 = __core.wxPAPER_10X11
wxPAPER_15X11 = __core.wxPAPER_15X11
wxPAPER_ENV_INVITE = __core.wxPAPER_ENV_INVITE
wxPAPER_LETTER_EXTRA = __core.wxPAPER_LETTER_EXTRA
wxPAPER_LEGAL_EXTRA = __core.wxPAPER_LEGAL_EXTRA
wxPAPER_TABLOID_EXTRA = __core.wxPAPER_TABLOID_EXTRA
wxPAPER_A4_EXTRA = __core.wxPAPER_A4_EXTRA
wxPAPER_LETTER_TRANSVERSE = __core.wxPAPER_LETTER_TRANSVERSE
wxPAPER_A4_TRANSVERSE = __core.wxPAPER_A4_TRANSVERSE
wxPAPER_LETTER_EXTRA_TRANSVERSE = __core.wxPAPER_LETTER_EXTRA_TRANSVERSE
wxPAPER_A_PLUS = __core.wxPAPER_A_PLUS
wxPAPER_B_PLUS = __core.wxPAPER_B_PLUS
wxPAPER_LETTER_PLUS = __core.wxPAPER_LETTER_PLUS
wxPAPER_A4_PLUS = __core.wxPAPER_A4_PLUS
wxPAPER_A5_TRANSVERSE = __core.wxPAPER_A5_TRANSVERSE
wxPAPER_B5_TRANSVERSE = __core.wxPAPER_B5_TRANSVERSE
wxPAPER_A3_EXTRA = __core.wxPAPER_A3_EXTRA
wxPAPER_A5_EXTRA = __core.wxPAPER_A5_EXTRA
wxPAPER_B5_EXTRA = __core.wxPAPER_B5_EXTRA
wxPAPER_A2 = __core.wxPAPER_A2
wxPAPER_A3_TRANSVERSE = __core.wxPAPER_A3_TRANSVERSE
wxPAPER_A3_EXTRA_TRANSVERSE = __core.wxPAPER_A3_EXTRA_TRANSVERSE
wxDUPLEX_SIMPLEX = __core.wxDUPLEX_SIMPLEX
wxDUPLEX_HORIZONTAL = __core.wxDUPLEX_HORIZONTAL
wxDUPLEX_VERTICAL = __core.wxDUPLEX_VERTICAL
wxITEM_SEPARATOR = __core.wxITEM_SEPARATOR
wxITEM_NORMAL = __core.wxITEM_NORMAL
wxITEM_CHECK = __core.wxITEM_CHECK
wxITEM_RADIO = __core.wxITEM_RADIO
wxITEM_MAX = __core.wxITEM_MAX
wxHT_NOWHERE = __core.wxHT_NOWHERE
wxHT_SCROLLBAR_FIRST = __core.wxHT_SCROLLBAR_FIRST
wxHT_SCROLLBAR_ARROW_LINE_1 = __core.wxHT_SCROLLBAR_ARROW_LINE_1
wxHT_SCROLLBAR_ARROW_LINE_2 = __core.wxHT_SCROLLBAR_ARROW_LINE_2
wxHT_SCROLLBAR_ARROW_PAGE_1 = __core.wxHT_SCROLLBAR_ARROW_PAGE_1
wxHT_SCROLLBAR_ARROW_PAGE_2 = __core.wxHT_SCROLLBAR_ARROW_PAGE_2
wxHT_SCROLLBAR_THUMB = __core.wxHT_SCROLLBAR_THUMB
wxHT_SCROLLBAR_BAR_1 = __core.wxHT_SCROLLBAR_BAR_1
wxHT_SCROLLBAR_BAR_2 = __core.wxHT_SCROLLBAR_BAR_2
wxHT_SCROLLBAR_LAST = __core.wxHT_SCROLLBAR_LAST
wxHT_WINDOW_OUTSIDE = __core.wxHT_WINDOW_OUTSIDE
wxHT_WINDOW_INSIDE = __core.wxHT_WINDOW_INSIDE
wxHT_WINDOW_VERT_SCROLLBAR = __core.wxHT_WINDOW_VERT_SCROLLBAR
wxHT_WINDOW_HORZ_SCROLLBAR = __core.wxHT_WINDOW_HORZ_SCROLLBAR
wxHT_WINDOW_CORNER = __core.wxHT_WINDOW_CORNER
wxHT_MAX = __core.wxHT_MAX
wxMOD_NONE = __core.wxMOD_NONE
wxMOD_ALT = __core.wxMOD_ALT
wxMOD_CONTROL = __core.wxMOD_CONTROL
wxMOD_SHIFT = __core.wxMOD_SHIFT
wxMOD_WIN = __core.wxMOD_WIN
wxUPDATE_UI_NONE = __core.wxUPDATE_UI_NONE
wxUPDATE_UI_RECURSE = __core.wxUPDATE_UI_RECURSE
wxUPDATE_UI_FROMIDLE = __core.wxUPDATE_UI_FROMIDLE
#---------------------------------------------------------------------------

class wxObject(object):
    """
    The base class for most wx objects, although in wxPython not
    much functionality is needed nor exposed.
    """
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxObject instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetClassName(*args, **kwargs):
        """
        GetClassName(self) -> wxString

        Returns the class name of the C++ class using wxRTTI.
        """
        return __core.wxObject_GetClassName(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """
        Destroy(self)

        Deletes the C++ object this Python object is a proxy for.
        """
        return __core.wxObject_Destroy(*args, **kwargs)


class wxObjectPtr(wxObject):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxObject
__core.wxObject_swigregister(wxObjectPtr)
_wxPySetDictionary = __core._wxPySetDictionary

_wxPyFixStockObjects = __core._wxPyFixStockObjects

cvar = __core.cvar
EmptyString = cvar.EmptyString

#---------------------------------------------------------------------------

wxBITMAP_TYPE_INVALID = __core.wxBITMAP_TYPE_INVALID
wxBITMAP_TYPE_BMP = __core.wxBITMAP_TYPE_BMP
wxBITMAP_TYPE_ICO = __core.wxBITMAP_TYPE_ICO
wxBITMAP_TYPE_CUR = __core.wxBITMAP_TYPE_CUR
wxBITMAP_TYPE_XBM = __core.wxBITMAP_TYPE_XBM
wxBITMAP_TYPE_XBM_DATA = __core.wxBITMAP_TYPE_XBM_DATA
wxBITMAP_TYPE_XPM = __core.wxBITMAP_TYPE_XPM
wxBITMAP_TYPE_XPM_DATA = __core.wxBITMAP_TYPE_XPM_DATA
wxBITMAP_TYPE_TIF = __core.wxBITMAP_TYPE_TIF
wxBITMAP_TYPE_GIF = __core.wxBITMAP_TYPE_GIF
wxBITMAP_TYPE_PNG = __core.wxBITMAP_TYPE_PNG
wxBITMAP_TYPE_JPEG = __core.wxBITMAP_TYPE_JPEG
wxBITMAP_TYPE_PNM = __core.wxBITMAP_TYPE_PNM
wxBITMAP_TYPE_PCX = __core.wxBITMAP_TYPE_PCX
wxBITMAP_TYPE_PICT = __core.wxBITMAP_TYPE_PICT
wxBITMAP_TYPE_ICON = __core.wxBITMAP_TYPE_ICON
wxBITMAP_TYPE_ANI = __core.wxBITMAP_TYPE_ANI
wxBITMAP_TYPE_IFF = __core.wxBITMAP_TYPE_IFF
wxBITMAP_TYPE_MACCURSOR = __core.wxBITMAP_TYPE_MACCURSOR
wxBITMAP_TYPE_ANY = __core.wxBITMAP_TYPE_ANY
wxCURSOR_NONE = __core.wxCURSOR_NONE
wxCURSOR_ARROW = __core.wxCURSOR_ARROW
wxCURSOR_RIGHT_ARROW = __core.wxCURSOR_RIGHT_ARROW
wxCURSOR_BULLSEYE = __core.wxCURSOR_BULLSEYE
wxCURSOR_CHAR = __core.wxCURSOR_CHAR
wxCURSOR_CROSS = __core.wxCURSOR_CROSS
wxCURSOR_HAND = __core.wxCURSOR_HAND
wxCURSOR_IBEAM = __core.wxCURSOR_IBEAM
wxCURSOR_LEFT_BUTTON = __core.wxCURSOR_LEFT_BUTTON
wxCURSOR_MAGNIFIER = __core.wxCURSOR_MAGNIFIER
wxCURSOR_MIDDLE_BUTTON = __core.wxCURSOR_MIDDLE_BUTTON
wxCURSOR_NO_ENTRY = __core.wxCURSOR_NO_ENTRY
wxCURSOR_PAINT_BRUSH = __core.wxCURSOR_PAINT_BRUSH
wxCURSOR_PENCIL = __core.wxCURSOR_PENCIL
wxCURSOR_POINT_LEFT = __core.wxCURSOR_POINT_LEFT
wxCURSOR_POINT_RIGHT = __core.wxCURSOR_POINT_RIGHT
wxCURSOR_QUESTION_ARROW = __core.wxCURSOR_QUESTION_ARROW
wxCURSOR_RIGHT_BUTTON = __core.wxCURSOR_RIGHT_BUTTON
wxCURSOR_SIZENESW = __core.wxCURSOR_SIZENESW
wxCURSOR_SIZENS = __core.wxCURSOR_SIZENS
wxCURSOR_SIZENWSE = __core.wxCURSOR_SIZENWSE
wxCURSOR_SIZEWE = __core.wxCURSOR_SIZEWE
wxCURSOR_SIZING = __core.wxCURSOR_SIZING
wxCURSOR_SPRAYCAN = __core.wxCURSOR_SPRAYCAN
wxCURSOR_WAIT = __core.wxCURSOR_WAIT
wxCURSOR_WATCH = __core.wxCURSOR_WATCH
wxCURSOR_BLANK = __core.wxCURSOR_BLANK
wxCURSOR_DEFAULT = __core.wxCURSOR_DEFAULT
wxCURSOR_COPY_ARROW = __core.wxCURSOR_COPY_ARROW
wxCURSOR_ARROWWAIT = __core.wxCURSOR_ARROWWAIT
wxCURSOR_MAX = __core.wxCURSOR_MAX
#---------------------------------------------------------------------------

class wxSize(object):
    """
    wx.Size is a useful data structure used to represent the size of
    something.  It simply contians integer width and height
    proprtites.  In most places in wxPython where a wx.Size is
    expected a (width, height) tuple can be used instead.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSize instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    width = property(__core.wxSize_width_get, __core.wxSize_width_set)
    height = property(__core.wxSize_height_get, __core.wxSize_height_set)
    x = width; y = height 
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int w=0, int h=0) -> wxSize

        Creates a size object.
        """
        newobj = __core.new_wxSize(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxSize):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def __eq__(*args, **kwargs):
        """
        __eq__(self, wxSize sz) -> bool

        Test for equality of wx.Size objects.
        """
        return __core.wxSize___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, wxSize sz) -> bool

        Test for inequality.
        """
        return __core.wxSize___ne__(*args, **kwargs)

    def __add__(*args, **kwargs):
        """
        __add__(self, wxSize sz) -> wxSize

        Add sz's proprties to this and return the result.
        """
        return __core.wxSize___add__(*args, **kwargs)

    def __sub__(*args, **kwargs):
        """
        __sub__(self, wxSize sz) -> wxSize

        Subtract sz's properties from this and return the result.
        """
        return __core.wxSize___sub__(*args, **kwargs)

    def IncTo(*args, **kwargs):
        """
        IncTo(self, wxSize sz)

        Increments this object so that both of its dimensions are not less
        than the corresponding dimensions of the size.
        """
        return __core.wxSize_IncTo(*args, **kwargs)

    def DecTo(*args, **kwargs):
        """
        DecTo(self, wxSize sz)

        Decrements this object so that both of its dimensions are not greater
        than the corresponding dimensions of the size.
        """
        return __core.wxSize_DecTo(*args, **kwargs)

    def Set(*args, **kwargs):
        """
        Set(self, int w, int h)

        Set both width and height.
        """
        return __core.wxSize_Set(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(self, int w)"""
        return __core.wxSize_SetWidth(*args, **kwargs)

    def SetHeight(*args, **kwargs):
        """SetHeight(self, int h)"""
        return __core.wxSize_SetHeight(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return __core.wxSize_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return __core.wxSize_GetHeight(*args, **kwargs)

    def IsFullySpecified(*args, **kwargs):
        """
        IsFullySpecified(self) -> bool

        Returns True if both components of the size are non-default values.
        """
        return __core.wxSize_IsFullySpecified(*args, **kwargs)

    def SetDefaults(*args, **kwargs):
        """
        SetDefaults(self, wxSize size)

        Combine this size with the other one replacing the default components
        of this object (i.e. equal to -1) with those of the other.
        """
        return __core.wxSize_SetDefaults(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (width,height)

        Returns the width and height properties as a tuple.
        """
        return __core.wxSize_Get(*args, **kwargs)

    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.Size'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.width = val
        elif index == 1: self.height = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.Size, self.Get())


class wxSizePtr(wxSize):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxSize
__core.wxSize_swigregister(wxSizePtr)

#---------------------------------------------------------------------------

class wxRealPoint(object):
    """
    A data structure for representing a point or position with floating
    point x and y properties.  In wxPython most places that expect a
    wx.RealPoint can also accept a (x,y) tuple.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRealPoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    x = property(__core.wxRealPoint_x_get, __core.wxRealPoint_x_set)
    y = property(__core.wxRealPoint_y_get, __core.wxRealPoint_y_set)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, double x=0.0, double y=0.0) -> wxRealPoint

        Create a wx.RealPoint object
        """
        newobj = __core.new_wxRealPoint(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxRealPoint):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def __eq__(*args, **kwargs):
        """
        __eq__(self, wxRealPoint pt) -> bool

        Test for equality of wx.RealPoint objects.
        """
        return __core.wxRealPoint___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, wxRealPoint pt) -> bool

        Test for inequality of wx.RealPoint objects.
        """
        return __core.wxRealPoint___ne__(*args, **kwargs)

    def __add__(*args, **kwargs):
        """
        __add__(self, wxRealPoint pt) -> wxRealPoint

        Add pt's proprties to this and return the result.
        """
        return __core.wxRealPoint___add__(*args, **kwargs)

    def __sub__(*args, **kwargs):
        """
        __sub__(self, wxRealPoint pt) -> wxRealPoint

        Subtract pt's proprties from this and return the result
        """
        return __core.wxRealPoint___sub__(*args, **kwargs)

    def Set(*args, **kwargs):
        """
        Set(self, double x, double y)

        Set both the x and y properties
        """
        return __core.wxRealPoint_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (x,y)

        Return the x and y properties as a tuple. 
        """
        return __core.wxRealPoint_Get(*args, **kwargs)

    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.RealPoint'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0.0, 0.0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.RealPoint, self.Get())


class wxRealPointPtr(wxRealPoint):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxRealPoint
__core.wxRealPoint_swigregister(wxRealPointPtr)

#---------------------------------------------------------------------------

class wxPoint(object):
    """
    A data structure for representing a point or position with integer x
    and y properties.  Most places in wxPython that expect a wx.Point can
    also accept a (x,y) tuple.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    x = property(__core.wxPoint_x_get, __core.wxPoint_x_set)
    y = property(__core.wxPoint_y_get, __core.wxPoint_y_set)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int x=0, int y=0) -> wxPoint

        Create a wx.Point object
        """
        newobj = __core.new_wxPoint(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxPoint):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def __eq__(*args, **kwargs):
        """
        __eq__(self, wxPoint pt) -> bool

        Test for equality of wx.Point objects.
        """
        return __core.wxPoint___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, wxPoint pt) -> bool

        Test for inequality of wx.Point objects.
        """
        return __core.wxPoint___ne__(*args, **kwargs)

    def __add__(*args, **kwargs):
        """
        __add__(self, wxPoint pt) -> wxPoint

        Add pt's proprties to this and return the result.
        """
        return __core.wxPoint___add__(*args, **kwargs)

    def __sub__(*args, **kwargs):
        """
        __sub__(self, wxPoint pt) -> wxPoint

        Subtract pt's proprties from this and return the result
        """
        return __core.wxPoint___sub__(*args, **kwargs)

    def __iadd__(*args, **kwargs):
        """
        __iadd__(self, wxPoint pt) -> wxPoint

        Add pt to this object.
        """
        return __core.wxPoint___iadd__(*args, **kwargs)

    def __isub__(*args, **kwargs):
        """
        __isub__(self, wxPoint pt) -> wxPoint

        Subtract pt from this object.
        """
        return __core.wxPoint___isub__(*args, **kwargs)

    def Set(*args, **kwargs):
        """
        Set(self, long x, long y)

        Set both the x and y properties
        """
        return __core.wxPoint_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (x,y)

        Return the x and y properties as a tuple. 
        """
        return __core.wxPoint_Get(*args, **kwargs)

    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.Point'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.Point, self.Get())


class wxPointPtr(wxPoint):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPoint
__core.wxPoint_swigregister(wxPointPtr)

#---------------------------------------------------------------------------

class wxRect(object):
    """
    A class for representing and manipulating rectangles.  It has x, y,
    width and height properties.  In wxPython most palces that expect a
    wx.Rect can also accept a (x,y,width,height) tuple.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRect instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int x=0, int y=0, int width=0, int height=0) -> wxRect

        Create a new Rect object.
        """
        newobj = __core.new_wxRect(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxRect):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetX(*args, **kwargs):
        """GetX(self) -> int"""
        return __core.wxRect_GetX(*args, **kwargs)

    def SetX(*args, **kwargs):
        """SetX(self, int x)"""
        return __core.wxRect_SetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return __core.wxRect_GetY(*args, **kwargs)

    def SetY(*args, **kwargs):
        """SetY(self, int y)"""
        return __core.wxRect_SetY(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return __core.wxRect_GetWidth(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(self, int w)"""
        return __core.wxRect_SetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return __core.wxRect_GetHeight(*args, **kwargs)

    def SetHeight(*args, **kwargs):
        """SetHeight(self, int h)"""
        return __core.wxRect_SetHeight(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> wxPoint"""
        return __core.wxRect_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, wxPoint p)"""
        return __core.wxRect_SetPosition(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self) -> wxSize"""
        return __core.wxRect_GetSize(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, wxSize s)"""
        return __core.wxRect_SetSize(*args, **kwargs)

    def IsEmpty(*args, **kwargs):
        """IsEmpty(self) -> bool"""
        return __core.wxRect_IsEmpty(*args, **kwargs)

    def GetTopLeft(*args, **kwargs):
        """GetTopLeft(self) -> wxPoint"""
        return __core.wxRect_GetTopLeft(*args, **kwargs)

    def SetTopLeft(*args, **kwargs):
        """SetTopLeft(self, wxPoint p)"""
        return __core.wxRect_SetTopLeft(*args, **kwargs)

    def GetBottomRight(*args, **kwargs):
        """GetBottomRight(self) -> wxPoint"""
        return __core.wxRect_GetBottomRight(*args, **kwargs)

    def SetBottomRight(*args, **kwargs):
        """SetBottomRight(self, wxPoint p)"""
        return __core.wxRect_SetBottomRight(*args, **kwargs)

    def GetLeft(*args, **kwargs):
        """GetLeft(self) -> int"""
        return __core.wxRect_GetLeft(*args, **kwargs)

    def GetTop(*args, **kwargs):
        """GetTop(self) -> int"""
        return __core.wxRect_GetTop(*args, **kwargs)

    def GetBottom(*args, **kwargs):
        """GetBottom(self) -> int"""
        return __core.wxRect_GetBottom(*args, **kwargs)

    def GetRight(*args, **kwargs):
        """GetRight(self) -> int"""
        return __core.wxRect_GetRight(*args, **kwargs)

    def SetLeft(*args, **kwargs):
        """SetLeft(self, int left)"""
        return __core.wxRect_SetLeft(*args, **kwargs)

    def SetRight(*args, **kwargs):
        """SetRight(self, int right)"""
        return __core.wxRect_SetRight(*args, **kwargs)

    def SetTop(*args, **kwargs):
        """SetTop(self, int top)"""
        return __core.wxRect_SetTop(*args, **kwargs)

    def SetBottom(*args, **kwargs):
        """SetBottom(self, int bottom)"""
        return __core.wxRect_SetBottom(*args, **kwargs)

    position = property(GetPosition, SetPosition)
    size = property(GetSize, SetSize)
    left = property(GetLeft, SetLeft)
    right = property(GetRight, SetRight)
    top = property(GetTop, SetTop)
    bottom = property(GetBottom, SetBottom)

    def Inflate(*args, **kwargs):
        """
        Inflate(self, int dx, int dy) -> wxRect

        Increases the size of the rectangle.

        The left border is moved farther left and the right border is moved
        farther right by ``dx``. The upper border is moved farther up and the
        bottom border is moved farther down by ``dy``. (Note the the width and
        height of the rectangle thus change by ``2*dx`` and ``2*dy``,
        respectively.) If one or both of ``dx`` and ``dy`` are negative, the
        opposite happens: the rectangle size decreases in the respective
        direction.

        The change is made to the rectangle inplace, if instead you need a
        copy that is inflated, preserving the original then make the copy
        first::

            copy = wx.Rect(*original)
            copy.Inflate(10,15)


        """
        return __core.wxRect_Inflate(*args, **kwargs)

    def Deflate(*args, **kwargs):
        """
        Deflate(self, int dx, int dy) -> wxRect

        Decrease the rectangle size. This method is the opposite of `Inflate`
        in that Deflate(a,b) is equivalent to Inflate(-a,-b).  Please refer to
        `Inflate` for a full description.
        """
        return __core.wxRect_Deflate(*args, **kwargs)

    def OffsetXY(*args, **kwargs):
        """
        OffsetXY(self, int dx, int dy)

        Moves the rectangle by the specified offset. If dx is positive, the
        rectangle is moved to the right, if dy is positive, it is moved to the
        bottom, otherwise it is moved to the left or top respectively.
        """
        return __core.wxRect_OffsetXY(*args, **kwargs)

    def Offset(*args, **kwargs):
        """
        Offset(self, wxPoint pt)

        Same as OffsetXY but uses dx,dy from Point
        """
        return __core.wxRect_Offset(*args, **kwargs)

    def Intersect(*args, **kwargs):
        """
        Intersect(self, wxRect rect) -> wxRect

        Returns the intersectsion of this rectangle and rect.
        """
        return __core.wxRect_Intersect(*args, **kwargs)

    def Union(*args, **kwargs):
        """
        Union(self, wxRect rect) -> wxRect

        Returns the union of this rectangle and rect.
        """
        return __core.wxRect_Union(*args, **kwargs)

    def __add__(*args, **kwargs):
        """
        __add__(self, wxRect rect) -> wxRect

        Add the properties of rect to this rectangle and return the result.
        """
        return __core.wxRect___add__(*args, **kwargs)

    def __iadd__(*args, **kwargs):
        """
        __iadd__(self, wxRect rect) -> wxRect

        Add the properties of rect to this rectangle, updating this rectangle.
        """
        return __core.wxRect___iadd__(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """
        __eq__(self, wxRect rect) -> bool

        Test for equality.
        """
        return __core.wxRect___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, wxRect rect) -> bool

        Test for inequality.
        """
        return __core.wxRect___ne__(*args, **kwargs)

    def InsideXY(*args, **kwargs):
        """
        InsideXY(self, int x, int y) -> bool

        Return True if the point is (not strcitly) inside the rect.
        """
        return __core.wxRect_InsideXY(*args, **kwargs)

    def Inside(*args, **kwargs):
        """
        Inside(self, wxPoint pt) -> bool

        Return True if the point is (not strcitly) inside the rect.
        """
        return __core.wxRect_Inside(*args, **kwargs)

    def Intersects(*args, **kwargs):
        """
        Intersects(self, wxRect rect) -> bool

        Returns True if the rectangles have a non empty intersection.
        """
        return __core.wxRect_Intersects(*args, **kwargs)

    x = property(__core.wxRect_x_get, __core.wxRect_x_set)
    y = property(__core.wxRect_y_get, __core.wxRect_y_set)
    width = property(__core.wxRect_width_get, __core.wxRect_width_set)
    height = property(__core.wxRect_height_get, __core.wxRect_height_set)
    def Set(*args, **kwargs):
        """
        Set(self, int x=0, int y=0, int width=0, int height=0)

        Set all rectangle properties.
        """
        return __core.wxRect_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (x,y,width,height)

        Return the rectangle properties as a tuple.
        """
        return __core.wxRect_Get(*args, **kwargs)

    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.Rect'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        elif index == 2: self.width = val
        elif index == 3: self.height = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0,0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.Rect, self.Get())


class wxRectPtr(wxRect):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxRect
__core.wxRect_swigregister(wxRectPtr)

def RectPP(*args, **kwargs):
    """
    RectPP(wxPoint topLeft, wxPoint bottomRight) -> wxRect

    Create a new Rect object from Points representing two corners.
    """
    val = __core.new_RectPP(*args, **kwargs)
    val.thisown = 1
    return val

def RectPS(*args, **kwargs):
    """
    RectPS(wxPoint pos, wxSize size) -> wxRect

    Create a new Rect from a position and size.
    """
    val = __core.new_RectPS(*args, **kwargs)
    val.thisown = 1
    return val

def RectS(*args, **kwargs):
    """
    RectS(wxSize size) -> wxRect

    Create a new Rect from a size only.
    """
    val = __core.new_RectS(*args, **kwargs)
    val.thisown = 1
    return val


def wxIntersectRect(*args, **kwargs):
    """
    IntersectRect(Rect r1, Rect r2) -> Rect

    Calculate and return the intersection of r1 and r2.
    """
    return __core.wxIntersectRect(*args, **kwargs)
#---------------------------------------------------------------------------

class wxPoint2D(object):
    """
    wx.Point2Ds represent a point or a vector in a 2d coordinate system
    with floating point values.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPoint2D instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, double x=0.0, double y=0.0) -> wxPoint2D

        Create a w.Point2D object.
        """
        newobj = __core.new_wxPoint2D(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetFloor(*args, **kwargs):
        """
        GetFloor() -> (x,y)

        Convert to integer
        """
        return __core.wxPoint2D_GetFloor(*args, **kwargs)

    def GetRounded(*args, **kwargs):
        """
        GetRounded() -> (x,y)

        Convert to integer
        """
        return __core.wxPoint2D_GetRounded(*args, **kwargs)

    def GetVectorLength(*args, **kwargs):
        """GetVectorLength(self) -> double"""
        return __core.wxPoint2D_GetVectorLength(*args, **kwargs)

    def GetVectorAngle(*args, **kwargs):
        """GetVectorAngle(self) -> double"""
        return __core.wxPoint2D_GetVectorAngle(*args, **kwargs)

    def SetVectorLength(*args, **kwargs):
        """SetVectorLength(self, double length)"""
        return __core.wxPoint2D_SetVectorLength(*args, **kwargs)

    def SetVectorAngle(*args, **kwargs):
        """SetVectorAngle(self, double degrees)"""
        return __core.wxPoint2D_SetVectorAngle(*args, **kwargs)

    def SetPolarCoordinates(self, angle, length):
        self.SetVectorLength(length)
        self.SetVectorAngle(angle)
    def Normalize(self):
        self.SetVectorLength(1.0)

    def GetDistance(*args, **kwargs):
        """GetDistance(self, wxPoint2D pt) -> double"""
        return __core.wxPoint2D_GetDistance(*args, **kwargs)

    def GetDistanceSquare(*args, **kwargs):
        """GetDistanceSquare(self, wxPoint2D pt) -> double"""
        return __core.wxPoint2D_GetDistanceSquare(*args, **kwargs)

    def GetDotProduct(*args, **kwargs):
        """GetDotProduct(self, wxPoint2D vec) -> double"""
        return __core.wxPoint2D_GetDotProduct(*args, **kwargs)

    def GetCrossProduct(*args, **kwargs):
        """GetCrossProduct(self, wxPoint2D vec) -> double"""
        return __core.wxPoint2D_GetCrossProduct(*args, **kwargs)

    def __neg__(*args, **kwargs):
        """
        __neg__(self) -> wxPoint2D

        the reflection of this point
        """
        return __core.wxPoint2D___neg__(*args, **kwargs)

    def __iadd__(*args, **kwargs):
        """__iadd__(self, wxPoint2D pt) -> wxPoint2D"""
        return __core.wxPoint2D___iadd__(*args, **kwargs)

    def __isub__(*args, **kwargs):
        """__isub__(self, wxPoint2D pt) -> wxPoint2D"""
        return __core.wxPoint2D___isub__(*args, **kwargs)

    def __imul__(*args, **kwargs):
        """__imul__(self, wxPoint2D pt) -> wxPoint2D"""
        return __core.wxPoint2D___imul__(*args, **kwargs)

    def __idiv__(*args, **kwargs):
        """__idiv__(self, wxPoint2D pt) -> wxPoint2D"""
        return __core.wxPoint2D___idiv__(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """
        __eq__(self, wxPoint2D pt) -> bool

        Test for equality
        """
        return __core.wxPoint2D___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, wxPoint2D pt) -> bool

        Test for inequality
        """
        return __core.wxPoint2D___ne__(*args, **kwargs)

    x = property(__core.wxPoint2D_x_get, __core.wxPoint2D_x_set)
    y = property(__core.wxPoint2D_y_get, __core.wxPoint2D_y_set)
    def Set(*args, **kwargs):
        """Set(self, double x=0, double y=0)"""
        return __core.wxPoint2D_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (x,y)

        Return x and y properties as a tuple.
        """
        return __core.wxPoint2D_Get(*args, **kwargs)

    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.Point2D'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0.0, 0.0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.Point2D, self.Get())


class wxPoint2DPtr(wxPoint2D):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPoint2D
__core.wxPoint2D_swigregister(wxPoint2DPtr)

def Point2DCopy(*args, **kwargs):
    """
    Point2DCopy(wxPoint2D pt) -> wxPoint2D

    Create a w.Point2D object.
    """
    val = __core.new_Point2DCopy(*args, **kwargs)
    val.thisown = 1
    return val

def Point2DFromPoint(*args, **kwargs):
    """
    Point2DFromPoint(wxPoint pt) -> wxPoint2D

    Create a w.Point2D object.
    """
    val = __core.new_Point2DFromPoint(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

wxFromStart = __core.wxFromStart
wxFromCurrent = __core.wxFromCurrent
wxFromEnd = __core.wxFromEnd
class InputStream(object):
    """Proxy of C++ InputStream class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyInputStream instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, PyObject p) -> InputStream"""
        newobj = __core.new_InputStream(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_InputStream):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def close(*args, **kwargs):
        """close(self)"""
        return __core.InputStream_close(*args, **kwargs)

    def flush(*args, **kwargs):
        """flush(self)"""
        return __core.InputStream_flush(*args, **kwargs)

    def eof(*args, **kwargs):
        """eof(self) -> bool"""
        return __core.InputStream_eof(*args, **kwargs)

    def read(*args, **kwargs):
        """read(self, int size=-1) -> PyObject"""
        return __core.InputStream_read(*args, **kwargs)

    def readline(*args, **kwargs):
        """readline(self, int size=-1) -> PyObject"""
        return __core.InputStream_readline(*args, **kwargs)

    def readlines(*args, **kwargs):
        """readlines(self, int sizehint=-1) -> PyObject"""
        return __core.InputStream_readlines(*args, **kwargs)

    def seek(*args, **kwargs):
        """seek(self, int offset, int whence=0)"""
        return __core.InputStream_seek(*args, **kwargs)

    def tell(*args, **kwargs):
        """tell(self) -> int"""
        return __core.InputStream_tell(*args, **kwargs)

    def Peek(*args, **kwargs):
        """Peek(self) -> char"""
        return __core.InputStream_Peek(*args, **kwargs)

    def GetC(*args, **kwargs):
        """GetC(self) -> char"""
        return __core.InputStream_GetC(*args, **kwargs)

    def LastRead(*args, **kwargs):
        """LastRead(self) -> size_t"""
        return __core.InputStream_LastRead(*args, **kwargs)

    def CanRead(*args, **kwargs):
        """CanRead(self) -> bool"""
        return __core.InputStream_CanRead(*args, **kwargs)

    def Eof(*args, **kwargs):
        """Eof(self) -> bool"""
        return __core.InputStream_Eof(*args, **kwargs)

    def Ungetch(*args, **kwargs):
        """Ungetch(self, char c) -> bool"""
        return __core.InputStream_Ungetch(*args, **kwargs)

    def SeekI(*args, **kwargs):
        """SeekI(self, long pos, int mode=wxFromStart) -> long"""
        return __core.InputStream_SeekI(*args, **kwargs)

    def TellI(*args, **kwargs):
        """TellI(self) -> long"""
        return __core.InputStream_TellI(*args, **kwargs)


class InputStreamPtr(InputStream):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = InputStream
__core.InputStream_swigregister(InputStreamPtr)
wxDefaultPosition = cvar.wxDefaultPosition
wxDefaultSize = cvar.wxDefaultSize

class wxOutputStream(object):
    """Proxy of C++ wxOutputStream class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxOutputStream instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def write(*args, **kwargs):
        """write(self, PyObject obj)"""
        return __core.wxOutputStream_write(*args, **kwargs)


class wxOutputStreamPtr(wxOutputStream):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxOutputStream
__core.wxOutputStream_swigregister(wxOutputStreamPtr)

#---------------------------------------------------------------------------

class wxFSFile(wxObject):
    """Proxy of C++ wxFSFile class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFSFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxInputStream stream, wxString loc, wxString mimetype, 
            wxString anchor, wxDateTime modif) -> wxFSFile
        """
        newobj = __core.new_wxFSFile(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self.thisown = 0   # It will normally be deleted by the user of the wx.FileSystem

    def __del__(self, destroy=__core.delete_wxFSFile):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetStream(*args, **kwargs):
        """GetStream(self) -> wxInputStream"""
        return __core.wxFSFile_GetStream(*args, **kwargs)

    def GetMimeType(*args, **kwargs):
        """GetMimeType(self) -> wxString"""
        return __core.wxFSFile_GetMimeType(*args, **kwargs)

    def GetLocation(*args, **kwargs):
        """GetLocation(self) -> wxString"""
        return __core.wxFSFile_GetLocation(*args, **kwargs)

    def GetAnchor(*args, **kwargs):
        """GetAnchor(self) -> wxString"""
        return __core.wxFSFile_GetAnchor(*args, **kwargs)

    def GetModificationTime(*args, **kwargs):
        """GetModificationTime(self) -> wxDateTime"""
        return __core.wxFSFile_GetModificationTime(*args, **kwargs)


class wxFSFilePtr(wxFSFile):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxFSFile
__core.wxFSFile_swigregister(wxFSFilePtr)

class CPPFileSystemHandler(object):
    """Proxy of C++ CPPFileSystemHandler class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFileSystemHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class CPPFileSystemHandlerPtr(CPPFileSystemHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CPPFileSystemHandler
__core.CPPFileSystemHandler_swigregister(CPPFileSystemHandlerPtr)

class FileSystemHandler(CPPFileSystemHandler):
    """Proxy of C++ FileSystemHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyFileSystemHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> FileSystemHandler"""
        newobj = __core.new_FileSystemHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, FileSystemHandler)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return __core.FileSystemHandler__setCallbackInfo(*args, **kwargs)

    def CanOpen(*args, **kwargs):
        """CanOpen(self, wxString location) -> bool"""
        return __core.FileSystemHandler_CanOpen(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, wxFileSystem fs, wxString location) -> wxFSFile"""
        return __core.FileSystemHandler_OpenFile(*args, **kwargs)

    def FindFirst(*args, **kwargs):
        """FindFirst(self, wxString spec, int flags=0) -> wxString"""
        return __core.FileSystemHandler_FindFirst(*args, **kwargs)

    def FindNext(*args, **kwargs):
        """FindNext(self) -> wxString"""
        return __core.FileSystemHandler_FindNext(*args, **kwargs)

    def GetProtocol(*args, **kwargs):
        """GetProtocol(self, wxString location) -> wxString"""
        return __core.FileSystemHandler_GetProtocol(*args, **kwargs)

    def GetLeftLocation(*args, **kwargs):
        """GetLeftLocation(self, wxString location) -> wxString"""
        return __core.FileSystemHandler_GetLeftLocation(*args, **kwargs)

    def GetAnchor(*args, **kwargs):
        """GetAnchor(self, wxString location) -> wxString"""
        return __core.FileSystemHandler_GetAnchor(*args, **kwargs)

    def GetRightLocation(*args, **kwargs):
        """GetRightLocation(self, wxString location) -> wxString"""
        return __core.FileSystemHandler_GetRightLocation(*args, **kwargs)

    def GetMimeTypeFromExt(*args, **kwargs):
        """GetMimeTypeFromExt(self, wxString location) -> wxString"""
        return __core.FileSystemHandler_GetMimeTypeFromExt(*args, **kwargs)


class FileSystemHandlerPtr(FileSystemHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FileSystemHandler
__core.FileSystemHandler_swigregister(FileSystemHandlerPtr)

class wxFileSystem(wxObject):
    """Proxy of C++ wxFileSystem class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFileSystem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxFileSystem"""
        newobj = __core.new_wxFileSystem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxFileSystem):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def ChangePathTo(*args, **kwargs):
        """ChangePathTo(self, wxString location, bool is_dir=False)"""
        return __core.wxFileSystem_ChangePathTo(*args, **kwargs)

    def GetPath(*args, **kwargs):
        """GetPath(self) -> wxString"""
        return __core.wxFileSystem_GetPath(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, wxString location) -> wxFSFile"""
        return __core.wxFileSystem_OpenFile(*args, **kwargs)

    def FindFirst(*args, **kwargs):
        """FindFirst(self, wxString spec, int flags=0) -> wxString"""
        return __core.wxFileSystem_FindFirst(*args, **kwargs)

    def FindNext(*args, **kwargs):
        """FindNext(self) -> wxString"""
        return __core.wxFileSystem_FindNext(*args, **kwargs)

    def AddHandler(*args, **kwargs):
        """AddHandler(CPPFileSystemHandler handler)"""
        return __core.wxFileSystem_AddHandler(*args, **kwargs)

    AddHandler = staticmethod(AddHandler)
    def CleanUpHandlers(*args, **kwargs):
        """CleanUpHandlers()"""
        return __core.wxFileSystem_CleanUpHandlers(*args, **kwargs)

    CleanUpHandlers = staticmethod(CleanUpHandlers)
    def FileNameToURL(*args, **kwargs):
        """FileNameToURL(wxString filename) -> wxString"""
        return __core.wxFileSystem_FileNameToURL(*args, **kwargs)

    FileNameToURL = staticmethod(FileNameToURL)
    def URLToFileName(*args, **kwargs):
        """URLToFileName(wxString url) -> wxString"""
        return __core.wxFileSystem_URLToFileName(*args, **kwargs)

    URLToFileName = staticmethod(URLToFileName)

class wxFileSystemPtr(wxFileSystem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxFileSystem
__core.wxFileSystem_swigregister(wxFileSystemPtr)

def wxFileSystem_AddHandler(*args, **kwargs):
    """wxFileSystem_AddHandler(CPPFileSystemHandler handler)"""
    return __core.wxFileSystem_AddHandler(*args, **kwargs)

def wxFileSystem_CleanUpHandlers(*args, **kwargs):
    """wxFileSystem_CleanUpHandlers()"""
    return __core.wxFileSystem_CleanUpHandlers(*args, **kwargs)

def wxFileSystem_FileNameToURL(*args, **kwargs):
    """wxFileSystem_FileNameToURL(wxString filename) -> wxString"""
    return __core.wxFileSystem_FileNameToURL(*args, **kwargs)

def wxFileSystem_URLToFileName(*args, **kwargs):
    """wxFileSystem_URLToFileName(wxString url) -> wxString"""
    return __core.wxFileSystem_URLToFileName(*args, **kwargs)

class wxInternetFSHandler(CPPFileSystemHandler):
    """Proxy of C++ wxInternetFSHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxInternetFSHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxInternetFSHandler"""
        newobj = __core.new_wxInternetFSHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def CanOpen(*args, **kwargs):
        """CanOpen(self, wxString location) -> bool"""
        return __core.wxInternetFSHandler_CanOpen(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, wxFileSystem fs, wxString location) -> wxFSFile"""
        return __core.wxInternetFSHandler_OpenFile(*args, **kwargs)


class wxInternetFSHandlerPtr(wxInternetFSHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxInternetFSHandler
__core.wxInternetFSHandler_swigregister(wxInternetFSHandlerPtr)

class wxZipFSHandler(CPPFileSystemHandler):
    """Proxy of C++ wxZipFSHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxZipFSHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxZipFSHandler"""
        newobj = __core.new_wxZipFSHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def CanOpen(*args, **kwargs):
        """CanOpen(self, wxString location) -> bool"""
        return __core.wxZipFSHandler_CanOpen(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, wxFileSystem fs, wxString location) -> wxFSFile"""
        return __core.wxZipFSHandler_OpenFile(*args, **kwargs)

    def FindFirst(*args, **kwargs):
        """FindFirst(self, wxString spec, int flags=0) -> wxString"""
        return __core.wxZipFSHandler_FindFirst(*args, **kwargs)

    def FindNext(*args, **kwargs):
        """FindNext(self) -> wxString"""
        return __core.wxZipFSHandler_FindNext(*args, **kwargs)


class wxZipFSHandlerPtr(wxZipFSHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxZipFSHandler
__core.wxZipFSHandler_swigregister(wxZipFSHandlerPtr)


def __wxMemoryFSHandler_AddFile_wxImage(*args, **kwargs):
    """__wxMemoryFSHandler_AddFile_wxImage(wxString filename, wxImage image, long type)"""
    return __core.__wxMemoryFSHandler_AddFile_wxImage(*args, **kwargs)

def __wxMemoryFSHandler_AddFile_wxBitmap(*args, **kwargs):
    """__wxMemoryFSHandler_AddFile_wxBitmap(wxString filename, wxBitmap bitmap, long type)"""
    return __core.__wxMemoryFSHandler_AddFile_wxBitmap(*args, **kwargs)

def __wxMemoryFSHandler_AddFile_Data(*args, **kwargs):
    """__wxMemoryFSHandler_AddFile_Data(wxString filename, PyObject data)"""
    return __core.__wxMemoryFSHandler_AddFile_Data(*args, **kwargs)
def MemoryFSHandler_AddFile(filename, dataItem, imgType=-1):
    """
    Add 'file' to the memory filesystem.  The dataItem parameter can
    either be a `wx.Bitmap`, `wx.Image` or a string that can contain
    arbitrary data.  If a bitmap or image is used then the imgType
    parameter should specify what kind of image file it should be
    written as, wx.BITMAP_TYPE_PNG, etc.
    """
    if isinstance(dataItem, wx.Image):
        __wxMemoryFSHandler_AddFile_wxImage(filename, dataItem, imgType)
    elif isinstance(dataItem, wx.Bitmap):
        __wxMemoryFSHandler_AddFile_wxBitmap(filename, dataItem, imgType)
    elif type(dataItem) == str:
        __wxMemoryFSHandler_AddFile_Data(filename, dataItem)
    else:
        raise TypeError, 'wx.Image, wx.Bitmap or string expected'

class wxMemoryFSHandler(CPPFileSystemHandler):
    """Proxy of C++ wxMemoryFSHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMemoryFSHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxMemoryFSHandler"""
        newobj = __core.new_wxMemoryFSHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def RemoveFile(*args, **kwargs):
        """RemoveFile(wxString filename)"""
        return __core.wxMemoryFSHandler_RemoveFile(*args, **kwargs)

    RemoveFile = staticmethod(RemoveFile)
    AddFile = staticmethod(MemoryFSHandler_AddFile) 
    def CanOpen(*args, **kwargs):
        """CanOpen(self, wxString location) -> bool"""
        return __core.wxMemoryFSHandler_CanOpen(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, wxFileSystem fs, wxString location) -> wxFSFile"""
        return __core.wxMemoryFSHandler_OpenFile(*args, **kwargs)

    def FindFirst(*args, **kwargs):
        """FindFirst(self, wxString spec, int flags=0) -> wxString"""
        return __core.wxMemoryFSHandler_FindFirst(*args, **kwargs)

    def FindNext(*args, **kwargs):
        """FindNext(self) -> wxString"""
        return __core.wxMemoryFSHandler_FindNext(*args, **kwargs)


class wxMemoryFSHandlerPtr(wxMemoryFSHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxMemoryFSHandler
__core.wxMemoryFSHandler_swigregister(wxMemoryFSHandlerPtr)

def wxMemoryFSHandler_RemoveFile(*args, **kwargs):
    """wxMemoryFSHandler_RemoveFile(wxString filename)"""
    return __core.wxMemoryFSHandler_RemoveFile(*args, **kwargs)

#---------------------------------------------------------------------------

class wxImageHandler(wxObject):
    """Proxy of C++ wxImageHandler class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxImageHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetName(*args, **kwargs):
        """GetName(self) -> wxString"""
        return __core.wxImageHandler_GetName(*args, **kwargs)

    def GetExtension(*args, **kwargs):
        """GetExtension(self) -> wxString"""
        return __core.wxImageHandler_GetExtension(*args, **kwargs)

    def GetType(*args, **kwargs):
        """GetType(self) -> long"""
        return __core.wxImageHandler_GetType(*args, **kwargs)

    def GetMimeType(*args, **kwargs):
        """GetMimeType(self) -> wxString"""
        return __core.wxImageHandler_GetMimeType(*args, **kwargs)

    def CanRead(*args, **kwargs):
        """CanRead(self, wxString name) -> bool"""
        return __core.wxImageHandler_CanRead(*args, **kwargs)

    def SetName(*args, **kwargs):
        """SetName(self, wxString name)"""
        return __core.wxImageHandler_SetName(*args, **kwargs)

    def SetExtension(*args, **kwargs):
        """SetExtension(self, wxString extension)"""
        return __core.wxImageHandler_SetExtension(*args, **kwargs)

    def SetType(*args, **kwargs):
        """SetType(self, long type)"""
        return __core.wxImageHandler_SetType(*args, **kwargs)

    def SetMimeType(*args, **kwargs):
        """SetMimeType(self, wxString mimetype)"""
        return __core.wxImageHandler_SetMimeType(*args, **kwargs)


class wxImageHandlerPtr(wxImageHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxImageHandler
__core.wxImageHandler_swigregister(wxImageHandlerPtr)

class wxImageHistogram(object):
    """Proxy of C++ wxImageHistogram class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxImageHistogram instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxImageHistogram"""
        newobj = __core.new_wxImageHistogram(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def MakeKey(*args, **kwargs):
        """
        MakeKey(unsigned char r, unsigned char g, unsigned char b) -> unsigned long

        Get the key in the histogram for the given RGB values
        """
        return __core.wxImageHistogram_MakeKey(*args, **kwargs)

    MakeKey = staticmethod(MakeKey)
    def FindFirstUnusedColour(*args, **kwargs):
        """
        FindFirstUnusedColour(int startR=1, int startG=0, int startB=0) -> (success, r, g, b)

        Find first colour that is not used in the image and has higher RGB
        values than startR, startG, startB.  Returns a tuple consisting of a
        success flag and rgb values.
        """
        return __core.wxImageHistogram_FindFirstUnusedColour(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """
        GetCount(self, unsigned long key) -> unsigned long

        Returns the pixel count for the given key.  Use `MakeKey` to create a
        key value from a RGB tripple.
        """
        return __core.wxImageHistogram_GetCount(*args, **kwargs)

    def GetCountRGB(*args, **kwargs):
        """
        GetCountRGB(self, unsigned char r, unsigned char g, unsigned char b) -> unsigned long

        Returns the pixel count for the given RGB values.
        """
        return __core.wxImageHistogram_GetCountRGB(*args, **kwargs)

    def GetCountColour(*args, **kwargs):
        """
        GetCountColour(self, wxColour colour) -> unsigned long

        Returns the pixel count for the given `wx.Colour` value.
        """
        return __core.wxImageHistogram_GetCountColour(*args, **kwargs)


class wxImageHistogramPtr(wxImageHistogram):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxImageHistogram
__core.wxImageHistogram_swigregister(wxImageHistogramPtr)

def wxImageHistogram_MakeKey(*args, **kwargs):
    """
    wxImageHistogram_MakeKey(unsigned char r, unsigned char g, unsigned char b) -> unsigned long

    Get the key in the histogram for the given RGB values
    """
    return __core.wxImageHistogram_MakeKey(*args, **kwargs)

class wxImage(wxObject):
    """Proxy of C++ wxImage class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxImage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxString name, long type=wxBITMAP_TYPE_ANY, int index=-1) -> wxImage"""
        newobj = __core.new_wxImage(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxImage):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Create(*args, **kwargs):
        """Create(self, int width, int height)"""
        return __core.wxImage_Create(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """
        Destroy(self)

        Deletes the C++ object this Python object is a proxy for.
        """
        return __core.wxImage_Destroy(*args, **kwargs)

    def Scale(*args, **kwargs):
        """Scale(self, int width, int height) -> wxImage"""
        return __core.wxImage_Scale(*args, **kwargs)

    def ShrinkBy(*args, **kwargs):
        """ShrinkBy(self, int xFactor, int yFactor) -> wxImage"""
        return __core.wxImage_ShrinkBy(*args, **kwargs)

    def Rescale(*args, **kwargs):
        """Rescale(self, int width, int height) -> wxImage"""
        return __core.wxImage_Rescale(*args, **kwargs)

    def Resize(*args, **kwargs):
        """Resize(self, wxSize size, wxPoint pos, int r=-1, int g=-1, int b=-1) -> wxImage"""
        return __core.wxImage_Resize(*args, **kwargs)

    def SetRGB(*args, **kwargs):
        """SetRGB(self, int x, int y, unsigned char r, unsigned char g, unsigned char b)"""
        return __core.wxImage_SetRGB(*args, **kwargs)

    def SetRGBRect(*args, **kwargs):
        """SetRGBRect(self, wxRect rect, unsigned char r, unsigned char g, unsigned char b)"""
        return __core.wxImage_SetRGBRect(*args, **kwargs)

    def GetRed(*args, **kwargs):
        """GetRed(self, int x, int y) -> unsigned char"""
        return __core.wxImage_GetRed(*args, **kwargs)

    def GetGreen(*args, **kwargs):
        """GetGreen(self, int x, int y) -> unsigned char"""
        return __core.wxImage_GetGreen(*args, **kwargs)

    def GetBlue(*args, **kwargs):
        """GetBlue(self, int x, int y) -> unsigned char"""
        return __core.wxImage_GetBlue(*args, **kwargs)

    def SetAlpha(*args, **kwargs):
        """SetAlpha(self, int x, int y, unsigned char alpha)"""
        return __core.wxImage_SetAlpha(*args, **kwargs)

    def GetAlpha(*args, **kwargs):
        """GetAlpha(self, int x, int y) -> unsigned char"""
        return __core.wxImage_GetAlpha(*args, **kwargs)

    def HasAlpha(*args, **kwargs):
        """HasAlpha(self) -> bool"""
        return __core.wxImage_HasAlpha(*args, **kwargs)

    def InitAlpha(*args, **kwargs):
        """
        InitAlpha(self)

        Initializes the image alpha channel data. It is an error to call it if
        the image already has alpha data. If it doesn't, alpha data will be by
        default initialized to all pixels being fully opaque. But if the image
        has a a mask colour, all mask pixels will be completely transparent.
        """
        return __core.wxImage_InitAlpha(*args, **kwargs)

    def FindFirstUnusedColour(*args, **kwargs):
        """
        FindFirstUnusedColour(int startR=1, int startG=0, int startB=0) -> (success, r, g, b)

        Find first colour that is not used in the image and has higher RGB
        values than startR, startG, startB.  Returns a tuple consisting of a
        success flag and rgb values.
        """
        return __core.wxImage_FindFirstUnusedColour(*args, **kwargs)

    def ConvertAlphaToMask(*args, **kwargs):
        """
        ConvertAlphaToMask(self, byte threshold=128) -> bool

        If the image has alpha channel, this method converts it to mask. All pixels
        with alpha value less than ``threshold`` are replaced with mask colour and the
        alpha channel is removed. Mask colour is chosen automatically using
        `FindFirstUnusedColour`.

        If the image image doesn't have alpha channel, ConvertAlphaToMask does
        nothing.
        """
        return __core.wxImage_ConvertAlphaToMask(*args, **kwargs)

    def ConvertColourToAlpha(*args, **kwargs):
        """
        ConvertColourToAlpha(self, unsigned char r, unsigned char g, unsigned char b) -> bool

        This method converts an image where the original alpha information is
        only available as a shades of a colour (actually shades of grey)
        typically when you draw anti-aliased text into a bitmap. The DC
        drawing routines draw grey values on the black background although
        they actually mean to draw white with differnt alpha values.  This
        method reverses it, assuming a black (!) background and white text.
        The method will then fill up the whole image with the colour given.
        """
        return __core.wxImage_ConvertColourToAlpha(*args, **kwargs)

    def SetMaskFromImage(*args, **kwargs):
        """SetMaskFromImage(self, wxImage mask, byte mr, byte mg, byte mb) -> bool"""
        return __core.wxImage_SetMaskFromImage(*args, **kwargs)

    def CanRead(*args, **kwargs):
        """CanRead(wxString name) -> bool"""
        return __core.wxImage_CanRead(*args, **kwargs)

    CanRead = staticmethod(CanRead)
    def GetImageCount(*args, **kwargs):
        """GetImageCount(wxString name, long type=wxBITMAP_TYPE_ANY) -> int"""
        return __core.wxImage_GetImageCount(*args, **kwargs)

    GetImageCount = staticmethod(GetImageCount)
    def LoadFile(*args, **kwargs):
        """LoadFile(self, wxString name, long type=wxBITMAP_TYPE_ANY, int index=-1) -> bool"""
        return __core.wxImage_LoadFile(*args, **kwargs)

    def LoadMimeFile(*args, **kwargs):
        """LoadMimeFile(self, wxString name, wxString mimetype, int index=-1) -> bool"""
        return __core.wxImage_LoadMimeFile(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """SaveFile(self, wxString name, int type) -> bool"""
        return __core.wxImage_SaveFile(*args, **kwargs)

    def SaveMimeFile(*args, **kwargs):
        """SaveMimeFile(self, wxString name, wxString mimetype) -> bool"""
        return __core.wxImage_SaveMimeFile(*args, **kwargs)

    def CanReadStream(*args, **kwargs):
        """CanReadStream(wxInputStream stream) -> bool"""
        return __core.wxImage_CanReadStream(*args, **kwargs)

    CanReadStream = staticmethod(CanReadStream)
    def LoadStream(*args, **kwargs):
        """
        LoadStream(self, wxInputStream stream, long type=wxBITMAP_TYPE_ANY, 
            int index=-1) -> bool
        """
        return __core.wxImage_LoadStream(*args, **kwargs)

    def LoadMimeStream(*args, **kwargs):
        """LoadMimeStream(self, wxInputStream stream, wxString mimetype, int index=-1) -> bool"""
        return __core.wxImage_LoadMimeStream(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return __core.wxImage_Ok(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return __core.wxImage_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return __core.wxImage_GetHeight(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self) -> wxSize"""
        return __core.wxImage_GetSize(*args, **kwargs)

    def GetSubImage(*args, **kwargs):
        """GetSubImage(self, wxRect rect) -> wxImage"""
        return __core.wxImage_GetSubImage(*args, **kwargs)

    def Size(*args, **kwargs):
        """Size(self, wxSize size, wxPoint pos, int r=-1, int g=-1, int b=-1) -> wxImage"""
        return __core.wxImage_Size(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self) -> wxImage"""
        return __core.wxImage_Copy(*args, **kwargs)

    def Paste(*args, **kwargs):
        """Paste(self, wxImage image, int x, int y)"""
        return __core.wxImage_Paste(*args, **kwargs)

    def GetData(*args, **kwargs):
        """
        GetData(self) -> PyObject

        Returns a string containing a copy of the RGB bytes of the image.
        """
        return __core.wxImage_GetData(*args, **kwargs)

    def SetData(*args, **kwargs):
        """
        SetData(self, buffer data)

        Resets the Image's RGB data from a buffer of RGB bytes.  Accepts
        either a string or a buffer object holding the data and the length of
        the data must be width*height*3.
        """
        return __core.wxImage_SetData(*args, **kwargs)

    def GetDataBuffer(*args, **kwargs):
        """
        GetDataBuffer(self) -> PyObject

        Returns a writable Python buffer object that is pointing at the RGB
        image data buffer inside the wx.Image.
        """
        return __core.wxImage_GetDataBuffer(*args, **kwargs)

    def SetDataBuffer(*args, **kwargs):
        """
        SetDataBuffer(self, buffer data)

        Sets the internal image data pointer to point at a Python buffer
        object.  This can save a copy of the data but you must ensure that the
        buffer object lives longer than the wx.Image does.
        """
        return __core.wxImage_SetDataBuffer(*args, **kwargs)

    def GetAlphaData(*args, **kwargs):
        """
        GetAlphaData(self) -> PyObject

        Returns a string containing a copy of the alpha bytes of the image.
        """
        return __core.wxImage_GetAlphaData(*args, **kwargs)

    def SetAlphaData(*args, **kwargs):
        """
        SetAlphaData(self, buffer alpha)

        Resets the Image's alpha data from a buffer of bytes.  Accepts either
        a string or a buffer object holding the data and the length of the
        data must be width*height.
        """
        return __core.wxImage_SetAlphaData(*args, **kwargs)

    def GetAlphaBuffer(*args, **kwargs):
        """GetAlphaBuffer(self) -> PyObject"""
        return __core.wxImage_GetAlphaBuffer(*args, **kwargs)

    def SetAlphaBuffer(*args, **kwargs):
        """SetAlphaBuffer(self, buffer alpha)"""
        return __core.wxImage_SetAlphaBuffer(*args, **kwargs)

    def SetMaskColour(*args, **kwargs):
        """SetMaskColour(self, unsigned char r, unsigned char g, unsigned char b)"""
        return __core.wxImage_SetMaskColour(*args, **kwargs)

    def GetOrFindMaskColour(*args, **kwargs):
        """
        GetOrFindMaskColour() -> (r,g,b)

        Get the current mask colour or find a suitable colour.
        """
        return __core.wxImage_GetOrFindMaskColour(*args, **kwargs)

    def GetMaskRed(*args, **kwargs):
        """GetMaskRed(self) -> unsigned char"""
        return __core.wxImage_GetMaskRed(*args, **kwargs)

    def GetMaskGreen(*args, **kwargs):
        """GetMaskGreen(self) -> unsigned char"""
        return __core.wxImage_GetMaskGreen(*args, **kwargs)

    def GetMaskBlue(*args, **kwargs):
        """GetMaskBlue(self) -> unsigned char"""
        return __core.wxImage_GetMaskBlue(*args, **kwargs)

    def SetMask(*args, **kwargs):
        """SetMask(self, bool mask=True)"""
        return __core.wxImage_SetMask(*args, **kwargs)

    def HasMask(*args, **kwargs):
        """HasMask(self) -> bool"""
        return __core.wxImage_HasMask(*args, **kwargs)

    def Rotate(*args, **kwargs):
        """
        Rotate(self, double angle, wxPoint centre_of_rotation, bool interpolating=True, 
            wxPoint offset_after_rotation=None) -> wxImage
        """
        return __core.wxImage_Rotate(*args, **kwargs)

    def Rotate90(*args, **kwargs):
        """Rotate90(self, bool clockwise=True) -> wxImage"""
        return __core.wxImage_Rotate90(*args, **kwargs)

    def Mirror(*args, **kwargs):
        """Mirror(self, bool horizontally=True) -> wxImage"""
        return __core.wxImage_Mirror(*args, **kwargs)

    def Replace(*args, **kwargs):
        """
        Replace(self, unsigned char r1, unsigned char g1, unsigned char b1, 
            unsigned char r2, unsigned char g2, unsigned char b2)
        """
        return __core.wxImage_Replace(*args, **kwargs)

    def ConvertToMono(*args, **kwargs):
        """ConvertToMono(self, unsigned char r, unsigned char g, unsigned char b) -> wxImage"""
        return __core.wxImage_ConvertToMono(*args, **kwargs)

    def SetOption(*args, **kwargs):
        """SetOption(self, wxString name, wxString value)"""
        return __core.wxImage_SetOption(*args, **kwargs)

    def SetOptionInt(*args, **kwargs):
        """SetOptionInt(self, wxString name, int value)"""
        return __core.wxImage_SetOptionInt(*args, **kwargs)

    def GetOption(*args, **kwargs):
        """GetOption(self, wxString name) -> wxString"""
        return __core.wxImage_GetOption(*args, **kwargs)

    def GetOptionInt(*args, **kwargs):
        """GetOptionInt(self, wxString name) -> int"""
        return __core.wxImage_GetOptionInt(*args, **kwargs)

    def HasOption(*args, **kwargs):
        """HasOption(self, wxString name) -> bool"""
        return __core.wxImage_HasOption(*args, **kwargs)

    def CountColours(*args, **kwargs):
        """CountColours(self, unsigned long stopafter=(unsigned long) -1) -> unsigned long"""
        return __core.wxImage_CountColours(*args, **kwargs)

    def ComputeHistogram(*args, **kwargs):
        """ComputeHistogram(self, wxImageHistogram h) -> unsigned long"""
        return __core.wxImage_ComputeHistogram(*args, **kwargs)

    def AddHandler(*args, **kwargs):
        """AddHandler(wxImageHandler handler)"""
        return __core.wxImage_AddHandler(*args, **kwargs)

    AddHandler = staticmethod(AddHandler)
    def InsertHandler(*args, **kwargs):
        """InsertHandler(wxImageHandler handler)"""
        return __core.wxImage_InsertHandler(*args, **kwargs)

    InsertHandler = staticmethod(InsertHandler)
    def RemoveHandler(*args, **kwargs):
        """RemoveHandler(wxString name) -> bool"""
        return __core.wxImage_RemoveHandler(*args, **kwargs)

    RemoveHandler = staticmethod(RemoveHandler)
    def GetImageExtWildcard(*args, **kwargs):
        """GetImageExtWildcard() -> wxString"""
        return __core.wxImage_GetImageExtWildcard(*args, **kwargs)

    GetImageExtWildcard = staticmethod(GetImageExtWildcard)
    def ConvertToBitmap(*args, **kwargs):
        """ConvertToBitmap(self, int depth=-1) -> wxBitmap"""
        return __core.wxImage_ConvertToBitmap(*args, **kwargs)

    def ConvertToMonoBitmap(*args, **kwargs):
        """ConvertToMonoBitmap(self, unsigned char red, unsigned char green, unsigned char blue) -> wxBitmap"""
        return __core.wxImage_ConvertToMonoBitmap(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class wxImagePtr(wxImage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxImage
__core.wxImage_swigregister(wxImagePtr)

def ImageFromMime(*args, **kwargs):
    """ImageFromMime(wxString name, wxString mimetype, int index=-1) -> wxImage"""
    val = __core.new_ImageFromMime(*args, **kwargs)
    val.thisown = 1
    return val

def ImageFromStream(*args, **kwargs):
    """
    ImageFromStream(wxInputStream stream, long type=wxBITMAP_TYPE_ANY, 
        int index=-1) -> wxImage
    """
    val = __core.new_ImageFromStream(*args, **kwargs)
    val.thisown = 1
    return val

def ImageFromStreamMime(*args, **kwargs):
    """ImageFromStreamMime(wxInputStream stream, wxString mimetype, int index=-1) -> wxImage"""
    val = __core.new_ImageFromStreamMime(*args, **kwargs)
    val.thisown = 1
    return val

def EmptyImage(*args, **kwargs):
    """
    EmptyImage(int width=0, int height=0, bool clear=True) -> wxImage

    Construct an empty image of a given size, optionally setting all
    pixels to black.
    """
    val = __core.new_EmptyImage(*args, **kwargs)
    val.thisown = 1
    return val

def ImageFromBitmap(*args, **kwargs):
    """
    ImageFromBitmap(wxBitmap bitmap) -> wxImage

    Construct an Image from a `wx.Bitmap`.
    """
    val = __core.new_ImageFromBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def ImageFromData(*args, **kwargs):
    """
    ImageFromData(int width, int height, buffer data) -> wxImage

    Construct an Image from a buffer of RGB bytes.  Accepts either a
    string or a buffer object holding the data and the length of the data
    must be width*height*3.
    """
    val = __core.new_ImageFromData(*args, **kwargs)
    val.thisown = 1
    return val

def ImageFromDataWithAlpha(*args, **kwargs):
    """
    ImageFromDataWithAlpha(int width, int height, buffer data, buffer alpha) -> wxImage

    Construct an Image from a buffer of RGB bytes with an Alpha channel.
    Accepts either a string or a buffer object holding the data and the
    length of the data must be width*height*3.
    """
    val = __core.new_ImageFromDataWithAlpha(*args, **kwargs)
    val.thisown = 1
    return val

def wxImage_CanRead(*args, **kwargs):
    """wxImage_CanRead(wxString name) -> bool"""
    return __core.wxImage_CanRead(*args, **kwargs)

def wxImage_GetImageCount(*args, **kwargs):
    """wxImage_GetImageCount(wxString name, long type=wxBITMAP_TYPE_ANY) -> int"""
    return __core.wxImage_GetImageCount(*args, **kwargs)

def wxImage_CanReadStream(*args, **kwargs):
    """wxImage_CanReadStream(wxInputStream stream) -> bool"""
    return __core.wxImage_CanReadStream(*args, **kwargs)

def wxImage_AddHandler(*args, **kwargs):
    """wxImage_AddHandler(wxImageHandler handler)"""
    return __core.wxImage_AddHandler(*args, **kwargs)

def wxImage_InsertHandler(*args, **kwargs):
    """wxImage_InsertHandler(wxImageHandler handler)"""
    return __core.wxImage_InsertHandler(*args, **kwargs)

def wxImage_RemoveHandler(*args, **kwargs):
    """wxImage_RemoveHandler(wxString name) -> bool"""
    return __core.wxImage_RemoveHandler(*args, **kwargs)

def wxImage_GetImageExtWildcard(*args, **kwargs):
    """wxImage_GetImageExtWildcard() -> wxString"""
    return __core.wxImage_GetImageExtWildcard(*args, **kwargs)

def InitAllImageHandlers():
    """
    The former functionality of InitAllImageHanders is now done internal to
    the _core_ extension module and so this function has become a simple NOP.
    """
    pass

wxIMAGE_RESOLUTION_INCHES = __core.wxIMAGE_RESOLUTION_INCHES
wxIMAGE_RESOLUTION_CM = __core.wxIMAGE_RESOLUTION_CM
wxPNG_TYPE_COLOUR = __core.wxPNG_TYPE_COLOUR
wxPNG_TYPE_GREY = __core.wxPNG_TYPE_GREY
wxPNG_TYPE_GREY_RED = __core.wxPNG_TYPE_GREY_RED
wxBMP_24BPP = __core.wxBMP_24BPP
wxBMP_8BPP = __core.wxBMP_8BPP
wxBMP_8BPP_GREY = __core.wxBMP_8BPP_GREY
wxBMP_8BPP_GRAY = __core.wxBMP_8BPP_GRAY
wxBMP_8BPP_RED = __core.wxBMP_8BPP_RED
wxBMP_8BPP_PALETTE = __core.wxBMP_8BPP_PALETTE
wxBMP_4BPP = __core.wxBMP_4BPP
wxBMP_1BPP = __core.wxBMP_1BPP
wxBMP_1BPP_BW = __core.wxBMP_1BPP_BW
class wxBMPHandler(wxImageHandler):
    """Proxy of C++ wxBMPHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBMPHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxBMPHandler"""
        newobj = __core.new_wxBMPHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxBMPHandlerPtr(wxBMPHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxBMPHandler
__core.wxBMPHandler_swigregister(wxBMPHandlerPtr)
wxNullImage = cvar.wxNullImage
IMAGE_OPTION_FILENAME = cvar.IMAGE_OPTION_FILENAME
IMAGE_OPTION_BMP_FORMAT = cvar.IMAGE_OPTION_BMP_FORMAT
IMAGE_OPTION_CUR_HOTSPOT_X = cvar.IMAGE_OPTION_CUR_HOTSPOT_X
IMAGE_OPTION_CUR_HOTSPOT_Y = cvar.IMAGE_OPTION_CUR_HOTSPOT_Y
IMAGE_OPTION_RESOLUTION = cvar.IMAGE_OPTION_RESOLUTION
IMAGE_OPTION_RESOLUTIONX = cvar.IMAGE_OPTION_RESOLUTIONX
IMAGE_OPTION_RESOLUTIONY = cvar.IMAGE_OPTION_RESOLUTIONY
IMAGE_OPTION_RESOLUTIONUNIT = cvar.IMAGE_OPTION_RESOLUTIONUNIT
IMAGE_OPTION_QUALITY = cvar.IMAGE_OPTION_QUALITY
IMAGE_OPTION_BITSPERSAMPLE = cvar.IMAGE_OPTION_BITSPERSAMPLE
IMAGE_OPTION_SAMPLESPERPIXEL = cvar.IMAGE_OPTION_SAMPLESPERPIXEL
IMAGE_OPTION_COMPRESSION = cvar.IMAGE_OPTION_COMPRESSION
IMAGE_OPTION_IMAGEDESCRIPTOR = cvar.IMAGE_OPTION_IMAGEDESCRIPTOR
IMAGE_OPTION_PNG_FORMAT = cvar.IMAGE_OPTION_PNG_FORMAT
IMAGE_OPTION_PNG_BITDEPTH = cvar.IMAGE_OPTION_PNG_BITDEPTH

class wxICOHandler(wxBMPHandler):
    """Proxy of C++ wxICOHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxICOHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxICOHandler"""
        newobj = __core.new_wxICOHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxICOHandlerPtr(wxICOHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxICOHandler
__core.wxICOHandler_swigregister(wxICOHandlerPtr)

class wxCURHandler(wxICOHandler):
    """Proxy of C++ wxCURHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCURHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxCURHandler"""
        newobj = __core.new_wxCURHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxCURHandlerPtr(wxCURHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxCURHandler
__core.wxCURHandler_swigregister(wxCURHandlerPtr)

class wxANIHandler(wxCURHandler):
    """Proxy of C++ wxANIHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxANIHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxANIHandler"""
        newobj = __core.new_wxANIHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxANIHandlerPtr(wxANIHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxANIHandler
__core.wxANIHandler_swigregister(wxANIHandlerPtr)

class wxPNGHandler(wxImageHandler):
    """Proxy of C++ wxPNGHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPNGHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxPNGHandler"""
        newobj = __core.new_wxPNGHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxPNGHandlerPtr(wxPNGHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPNGHandler
__core.wxPNGHandler_swigregister(wxPNGHandlerPtr)

class wxGIFHandler(wxImageHandler):
    """Proxy of C++ wxGIFHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGIFHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxGIFHandler"""
        newobj = __core.new_wxGIFHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxGIFHandlerPtr(wxGIFHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxGIFHandler
__core.wxGIFHandler_swigregister(wxGIFHandlerPtr)

class wxPCXHandler(wxImageHandler):
    """Proxy of C++ wxPCXHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPCXHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxPCXHandler"""
        newobj = __core.new_wxPCXHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxPCXHandlerPtr(wxPCXHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPCXHandler
__core.wxPCXHandler_swigregister(wxPCXHandlerPtr)

class wxJPEGHandler(wxImageHandler):
    """Proxy of C++ wxJPEGHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxJPEGHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxJPEGHandler"""
        newobj = __core.new_wxJPEGHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxJPEGHandlerPtr(wxJPEGHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxJPEGHandler
__core.wxJPEGHandler_swigregister(wxJPEGHandlerPtr)

class wxPNMHandler(wxImageHandler):
    """Proxy of C++ wxPNMHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPNMHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxPNMHandler"""
        newobj = __core.new_wxPNMHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxPNMHandlerPtr(wxPNMHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPNMHandler
__core.wxPNMHandler_swigregister(wxPNMHandlerPtr)

class wxXPMHandler(wxImageHandler):
    """Proxy of C++ wxXPMHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxXPMHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxXPMHandler"""
        newobj = __core.new_wxXPMHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxXPMHandlerPtr(wxXPMHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxXPMHandler
__core.wxXPMHandler_swigregister(wxXPMHandlerPtr)

class wxTIFFHandler(wxImageHandler):
    """Proxy of C++ wxTIFFHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTIFFHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxTIFFHandler"""
        newobj = __core.new_wxTIFFHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxTIFFHandlerPtr(wxTIFFHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxTIFFHandler
__core.wxTIFFHandler_swigregister(wxTIFFHandlerPtr)

wxQUANTIZE_INCLUDE_WINDOWS_COLOURS = __core.wxQUANTIZE_INCLUDE_WINDOWS_COLOURS
wxQUANTIZE_FILL_DESTINATION_IMAGE = __core.wxQUANTIZE_FILL_DESTINATION_IMAGE
class wxQuantize(object):
    """Performs quantization, or colour reduction, on a wxImage."""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxQuantize instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def Quantize(*args, **kwargs):
        """
        Quantize(wxImage src, wxImage dest, int desiredNoColours=236, 
            int flags=wxQUANTIZE_INCLUDE_WINDOWS_COLOURS|wxQUANTIZE_FILL_DESTINATION_IMAGE) -> bool

        Reduce the colours in the source image and put the result into the
        destination image, setting the palette in the destination if
        needed. Both images may be the same, to overwrite the source image.
        """
        return __core.wxQuantize_Quantize(*args, **kwargs)

    Quantize = staticmethod(Quantize)

class wxQuantizePtr(wxQuantize):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxQuantize
__core.wxQuantize_swigregister(wxQuantizePtr)

def wxQuantize_Quantize(*args, **kwargs):
    """
    wxQuantize_Quantize(wxImage src, wxImage dest, int desiredNoColours=236, 
        int flags=wxQUANTIZE_INCLUDE_WINDOWS_COLOURS|wxQUANTIZE_FILL_DESTINATION_IMAGE) -> bool

    Reduce the colours in the source image and put the result into the
    destination image, setting the palette in the destination if
    needed. Both images may be the same, to overwrite the source image.
    """
    return __core.wxQuantize_Quantize(*args, **kwargs)

#---------------------------------------------------------------------------

class wxEvtHandler(wxObject):
    """Proxy of C++ wxEvtHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEvtHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxEvtHandler"""
        newobj = __core.new_wxEvtHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetNextHandler(*args, **kwargs):
        """GetNextHandler(self) -> wxEvtHandler"""
        return __core.wxEvtHandler_GetNextHandler(*args, **kwargs)

    def GetPreviousHandler(*args, **kwargs):
        """GetPreviousHandler(self) -> wxEvtHandler"""
        return __core.wxEvtHandler_GetPreviousHandler(*args, **kwargs)

    def SetNextHandler(*args, **kwargs):
        """SetNextHandler(self, wxEvtHandler handler)"""
        return __core.wxEvtHandler_SetNextHandler(*args, **kwargs)

    def SetPreviousHandler(*args, **kwargs):
        """SetPreviousHandler(self, wxEvtHandler handler)"""
        return __core.wxEvtHandler_SetPreviousHandler(*args, **kwargs)

    def GetEvtHandlerEnabled(*args, **kwargs):
        """GetEvtHandlerEnabled(self) -> bool"""
        return __core.wxEvtHandler_GetEvtHandlerEnabled(*args, **kwargs)

    def SetEvtHandlerEnabled(*args, **kwargs):
        """SetEvtHandlerEnabled(self, bool enabled)"""
        return __core.wxEvtHandler_SetEvtHandlerEnabled(*args, **kwargs)

    def ProcessEvent(*args, **kwargs):
        """ProcessEvent(self, wxEvent event) -> bool"""
        return __core.wxEvtHandler_ProcessEvent(*args, **kwargs)

    def AddPendingEvent(*args, **kwargs):
        """AddPendingEvent(self, wxEvent event)"""
        return __core.wxEvtHandler_AddPendingEvent(*args, **kwargs)

    def ProcessPendingEvents(*args, **kwargs):
        """ProcessPendingEvents(self)"""
        return __core.wxEvtHandler_ProcessPendingEvents(*args, **kwargs)

    def Connect(*args, **kwargs):
        """Connect(self, int id, int lastId, int eventType, PyObject func)"""
        return __core.wxEvtHandler_Connect(*args, **kwargs)

    def Disconnect(*args, **kwargs):
        """Disconnect(self, int id, int lastId=-1, wxEventType eventType=wxEVT_NULL) -> bool"""
        return __core.wxEvtHandler_Disconnect(*args, **kwargs)

    def _setOORInfo(*args, **kwargs):
        """_setOORInfo(self, PyObject _self, bool incref=True)"""
        return __core.wxEvtHandler__setOORInfo(*args, **kwargs)

    def Bind(self, event, handler, source=None, id=wx.ID_ANY, id2=wx.ID_ANY):
        """
        Bind an event to an event handler.

        :param event: One of the EVT_* objects that specifies the
                      type of event to bind,

        :param handler: A callable object to be invoked when the
                      event is delivered to self.  Pass None to
                      disconnect an event handler.

        :param source: Sometimes the event originates from a
                      different window than self, but you still
                      want to catch it in self.  (For example, a
                      button event delivered to a frame.)  By
                      passing the source of the event, the event
                      handling system is able to differentiate
                      between the same event type from different
                      controls.

        :param id: Used to spcify the event source by ID instead
                   of instance.

        :param id2: Used when it is desirable to bind a handler
                      to a range of IDs, such as with EVT_MENU_RANGE.
        """
        if source is not None:
            id  = source.GetId()
        event.Bind(self, id, id2, handler)              

    def Unbind(self, event, source=None, id=wx.ID_ANY, id2=wx.ID_ANY):
        """
        Disconencts the event handler binding for event from self.
        Returns True if successful.
        """
        if source is not None:
            id  = source.GetId()
        return event.Unbind(self, id, id2)              


class wxEvtHandlerPtr(wxEvtHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxEvtHandler
__core.wxEvtHandler_swigregister(wxEvtHandlerPtr)

#---------------------------------------------------------------------------

class PyEventBinder(object):
    """
    Instances of this class are used to bind specific events to event
    handlers.
    """
    def __init__(self, evtType, expectedIDs=0):
        if expectedIDs not in [0, 1, 2]:
            raise ValueError, "Invalid number of expectedIDs"
        self.expectedIDs = expectedIDs

        if type(evtType) == list or type(evtType) == tuple:
            self.evtType = evtType
        else:
            self.evtType = [evtType]


    def Bind(self, target, id1, id2, function):
        """Bind this set of event types to target."""
        for et in self.evtType:
            target.Connect(id1, id2, et, function)


    def Unbind(self, target, id1, id2):
        """Remove an event binding."""
        success = 0
        for et in self.evtType:
            success += target.Disconnect(id1, id2, et)
        return success != 0

    
    def __call__(self, *args):
        """
        For backwards compatibility with the old EVT_* functions.
        Should be called with either (window, func), (window, ID,
        func) or (window, ID1, ID2, func) parameters depending on the
        type of the event.
        """
        assert len(args) == 2 + self.expectedIDs
        id1 = wx.ID_ANY
        id2 = wx.ID_ANY
        target = args[0]
        if self.expectedIDs == 0:
            func = args[1]
        elif self.expectedIDs == 1:
            id1 = args[1]
            func = args[2]
        elif self.expectedIDs == 2:
            id1 = args[1]
            id2 = args[2]
            func = args[3]
        else:
            raise ValueError, "Unexpected number of IDs"

        self.Bind(target, id1, id2, func)


# These two are square pegs that don't fit the PyEventBinder hole...
def EVT_COMMAND(win, id, cmd, func):
    win.Connect(id, -1, cmd, func)
def EVT_COMMAND_RANGE(win, id1, id2, cmd, func):
    win.Connect(id1, id2, cmd, func)

    
#---------------------------------------------------------------------------

#---------------------------------------------------------------------------

wxEVENT_PROPAGATE_NONE = __core.wxEVENT_PROPAGATE_NONE
wxEVENT_PROPAGATE_MAX = __core.wxEVENT_PROPAGATE_MAX

def wxNewEventType(*args, **kwargs):
    """wxNewEventType() -> wxEventType"""
    return __core.wxNewEventType(*args, **kwargs)
wxEVT_NULL = __core.wxEVT_NULL
wxEVT_FIRST = __core.wxEVT_FIRST
wxEVT_USER_FIRST = __core.wxEVT_USER_FIRST
wxEVT_COMMAND_BUTTON_CLICKED = __core.wxEVT_COMMAND_BUTTON_CLICKED
wxEVT_COMMAND_CHECKBOX_CLICKED = __core.wxEVT_COMMAND_CHECKBOX_CLICKED
wxEVT_COMMAND_CHOICE_SELECTED = __core.wxEVT_COMMAND_CHOICE_SELECTED
wxEVT_COMMAND_LISTBOX_SELECTED = __core.wxEVT_COMMAND_LISTBOX_SELECTED
wxEVT_COMMAND_LISTBOX_DOUBLECLICKED = __core.wxEVT_COMMAND_LISTBOX_DOUBLECLICKED
wxEVT_COMMAND_CHECKLISTBOX_TOGGLED = __core.wxEVT_COMMAND_CHECKLISTBOX_TOGGLED
wxEVT_COMMAND_MENU_SELECTED = __core.wxEVT_COMMAND_MENU_SELECTED
wxEVT_COMMAND_TOOL_CLICKED = __core.wxEVT_COMMAND_TOOL_CLICKED
wxEVT_COMMAND_SLIDER_UPDATED = __core.wxEVT_COMMAND_SLIDER_UPDATED
wxEVT_COMMAND_RADIOBOX_SELECTED = __core.wxEVT_COMMAND_RADIOBOX_SELECTED
wxEVT_COMMAND_RADIOBUTTON_SELECTED = __core.wxEVT_COMMAND_RADIOBUTTON_SELECTED
wxEVT_COMMAND_SCROLLBAR_UPDATED = __core.wxEVT_COMMAND_SCROLLBAR_UPDATED
wxEVT_COMMAND_VLBOX_SELECTED = __core.wxEVT_COMMAND_VLBOX_SELECTED
wxEVT_COMMAND_COMBOBOX_SELECTED = __core.wxEVT_COMMAND_COMBOBOX_SELECTED
wxEVT_COMMAND_TOOL_RCLICKED = __core.wxEVT_COMMAND_TOOL_RCLICKED
wxEVT_COMMAND_TOOL_ENTER = __core.wxEVT_COMMAND_TOOL_ENTER
wxEVT_LEFT_DOWN = __core.wxEVT_LEFT_DOWN
wxEVT_LEFT_UP = __core.wxEVT_LEFT_UP
wxEVT_MIDDLE_DOWN = __core.wxEVT_MIDDLE_DOWN
wxEVT_MIDDLE_UP = __core.wxEVT_MIDDLE_UP
wxEVT_RIGHT_DOWN = __core.wxEVT_RIGHT_DOWN
wxEVT_RIGHT_UP = __core.wxEVT_RIGHT_UP
wxEVT_MOTION = __core.wxEVT_MOTION
wxEVT_ENTER_WINDOW = __core.wxEVT_ENTER_WINDOW
wxEVT_LEAVE_WINDOW = __core.wxEVT_LEAVE_WINDOW
wxEVT_LEFT_DCLICK = __core.wxEVT_LEFT_DCLICK
wxEVT_MIDDLE_DCLICK = __core.wxEVT_MIDDLE_DCLICK
wxEVT_RIGHT_DCLICK = __core.wxEVT_RIGHT_DCLICK
wxEVT_SET_FOCUS = __core.wxEVT_SET_FOCUS
wxEVT_KILL_FOCUS = __core.wxEVT_KILL_FOCUS
wxEVT_CHILD_FOCUS = __core.wxEVT_CHILD_FOCUS
wxEVT_MOUSEWHEEL = __core.wxEVT_MOUSEWHEEL
wxEVT_NC_LEFT_DOWN = __core.wxEVT_NC_LEFT_DOWN
wxEVT_NC_LEFT_UP = __core.wxEVT_NC_LEFT_UP
wxEVT_NC_MIDDLE_DOWN = __core.wxEVT_NC_MIDDLE_DOWN
wxEVT_NC_MIDDLE_UP = __core.wxEVT_NC_MIDDLE_UP
wxEVT_NC_RIGHT_DOWN = __core.wxEVT_NC_RIGHT_DOWN
wxEVT_NC_RIGHT_UP = __core.wxEVT_NC_RIGHT_UP
wxEVT_NC_MOTION = __core.wxEVT_NC_MOTION
wxEVT_NC_ENTER_WINDOW = __core.wxEVT_NC_ENTER_WINDOW
wxEVT_NC_LEAVE_WINDOW = __core.wxEVT_NC_LEAVE_WINDOW
wxEVT_NC_LEFT_DCLICK = __core.wxEVT_NC_LEFT_DCLICK
wxEVT_NC_MIDDLE_DCLICK = __core.wxEVT_NC_MIDDLE_DCLICK
wxEVT_NC_RIGHT_DCLICK = __core.wxEVT_NC_RIGHT_DCLICK
wxEVT_CHAR = __core.wxEVT_CHAR
wxEVT_CHAR_HOOK = __core.wxEVT_CHAR_HOOK
wxEVT_NAVIGATION_KEY = __core.wxEVT_NAVIGATION_KEY
wxEVT_KEY_DOWN = __core.wxEVT_KEY_DOWN
wxEVT_KEY_UP = __core.wxEVT_KEY_UP
wxEVT_HOTKEY = __core.wxEVT_HOTKEY
wxEVT_SET_CURSOR = __core.wxEVT_SET_CURSOR
wxEVT_SCROLL_TOP = __core.wxEVT_SCROLL_TOP
wxEVT_SCROLL_BOTTOM = __core.wxEVT_SCROLL_BOTTOM
wxEVT_SCROLL_LINEUP = __core.wxEVT_SCROLL_LINEUP
wxEVT_SCROLL_LINEDOWN = __core.wxEVT_SCROLL_LINEDOWN
wxEVT_SCROLL_PAGEUP = __core.wxEVT_SCROLL_PAGEUP
wxEVT_SCROLL_PAGEDOWN = __core.wxEVT_SCROLL_PAGEDOWN
wxEVT_SCROLL_THUMBTRACK = __core.wxEVT_SCROLL_THUMBTRACK
wxEVT_SCROLL_THUMBRELEASE = __core.wxEVT_SCROLL_THUMBRELEASE
wxEVT_SCROLL_ENDSCROLL = __core.wxEVT_SCROLL_ENDSCROLL
wxEVT_SCROLLWIN_TOP = __core.wxEVT_SCROLLWIN_TOP
wxEVT_SCROLLWIN_BOTTOM = __core.wxEVT_SCROLLWIN_BOTTOM
wxEVT_SCROLLWIN_LINEUP = __core.wxEVT_SCROLLWIN_LINEUP
wxEVT_SCROLLWIN_LINEDOWN = __core.wxEVT_SCROLLWIN_LINEDOWN
wxEVT_SCROLLWIN_PAGEUP = __core.wxEVT_SCROLLWIN_PAGEUP
wxEVT_SCROLLWIN_PAGEDOWN = __core.wxEVT_SCROLLWIN_PAGEDOWN
wxEVT_SCROLLWIN_THUMBTRACK = __core.wxEVT_SCROLLWIN_THUMBTRACK
wxEVT_SCROLLWIN_THUMBRELEASE = __core.wxEVT_SCROLLWIN_THUMBRELEASE
wxEVT_SIZE = __core.wxEVT_SIZE
wxEVT_MOVE = __core.wxEVT_MOVE
wxEVT_CLOSE_WINDOW = __core.wxEVT_CLOSE_WINDOW
wxEVT_END_SESSION = __core.wxEVT_END_SESSION
wxEVT_QUERY_END_SESSION = __core.wxEVT_QUERY_END_SESSION
wxEVT_ACTIVATE_APP = __core.wxEVT_ACTIVATE_APP
wxEVT_POWER = __core.wxEVT_POWER
wxEVT_ACTIVATE = __core.wxEVT_ACTIVATE
wxEVT_CREATE = __core.wxEVT_CREATE
wxEVT_DESTROY = __core.wxEVT_DESTROY
wxEVT_SHOW = __core.wxEVT_SHOW
wxEVT_ICONIZE = __core.wxEVT_ICONIZE
wxEVT_MAXIMIZE = __core.wxEVT_MAXIMIZE
wxEVT_MOUSE_CAPTURE_CHANGED = __core.wxEVT_MOUSE_CAPTURE_CHANGED
wxEVT_PAINT = __core.wxEVT_PAINT
wxEVT_ERASE_BACKGROUND = __core.wxEVT_ERASE_BACKGROUND
wxEVT_NC_PAINT = __core.wxEVT_NC_PAINT
wxEVT_PAINT_ICON = __core.wxEVT_PAINT_ICON
wxEVT_MENU_OPEN = __core.wxEVT_MENU_OPEN
wxEVT_MENU_CLOSE = __core.wxEVT_MENU_CLOSE
wxEVT_MENU_HIGHLIGHT = __core.wxEVT_MENU_HIGHLIGHT
wxEVT_CONTEXT_MENU = __core.wxEVT_CONTEXT_MENU
wxEVT_SYS_COLOUR_CHANGED = __core.wxEVT_SYS_COLOUR_CHANGED
wxEVT_DISPLAY_CHANGED = __core.wxEVT_DISPLAY_CHANGED
wxEVT_SETTING_CHANGED = __core.wxEVT_SETTING_CHANGED
wxEVT_QUERY_NEW_PALETTE = __core.wxEVT_QUERY_NEW_PALETTE
wxEVT_PALETTE_CHANGED = __core.wxEVT_PALETTE_CHANGED
wxEVT_DROP_FILES = __core.wxEVT_DROP_FILES
wxEVT_DRAW_ITEM = __core.wxEVT_DRAW_ITEM
wxEVT_MEASURE_ITEM = __core.wxEVT_MEASURE_ITEM
wxEVT_COMPARE_ITEM = __core.wxEVT_COMPARE_ITEM
wxEVT_INIT_DIALOG = __core.wxEVT_INIT_DIALOG
wxEVT_IDLE = __core.wxEVT_IDLE
wxEVT_UPDATE_UI = __core.wxEVT_UPDATE_UI
wxEVT_SIZING = __core.wxEVT_SIZING
wxEVT_MOVING = __core.wxEVT_MOVING
wxEVT_HIBERNATE = __core.wxEVT_HIBERNATE
wxEVT_COMMAND_LEFT_CLICK = __core.wxEVT_COMMAND_LEFT_CLICK
wxEVT_COMMAND_LEFT_DCLICK = __core.wxEVT_COMMAND_LEFT_DCLICK
wxEVT_COMMAND_RIGHT_CLICK = __core.wxEVT_COMMAND_RIGHT_CLICK
wxEVT_COMMAND_RIGHT_DCLICK = __core.wxEVT_COMMAND_RIGHT_DCLICK
wxEVT_COMMAND_SET_FOCUS = __core.wxEVT_COMMAND_SET_FOCUS
wxEVT_COMMAND_KILL_FOCUS = __core.wxEVT_COMMAND_KILL_FOCUS
wxEVT_COMMAND_ENTER = __core.wxEVT_COMMAND_ENTER
#
# Create some event binders
EVT_SIZE = wx.PyEventBinder( wxEVT_SIZE )
EVT_SIZING = wx.PyEventBinder( wxEVT_SIZING )
EVT_MOVE = wx.PyEventBinder( wxEVT_MOVE )
EVT_MOVING = wx.PyEventBinder( wxEVT_MOVING )
EVT_CLOSE = wx.PyEventBinder( wxEVT_CLOSE_WINDOW )
EVT_END_SESSION = wx.PyEventBinder( wxEVT_END_SESSION )
EVT_QUERY_END_SESSION = wx.PyEventBinder( wxEVT_QUERY_END_SESSION )
EVT_PAINT = wx.PyEventBinder( wxEVT_PAINT )
EVT_NC_PAINT = wx.PyEventBinder( wxEVT_NC_PAINT )
EVT_ERASE_BACKGROUND = wx.PyEventBinder( wxEVT_ERASE_BACKGROUND )
EVT_CHAR = wx.PyEventBinder( wxEVT_CHAR )
EVT_KEY_DOWN = wx.PyEventBinder( wxEVT_KEY_DOWN )
EVT_KEY_UP = wx.PyEventBinder( wxEVT_KEY_UP )
EVT_HOTKEY = wx.PyEventBinder( wxEVT_HOTKEY, 1) 
EVT_CHAR_HOOK = wx.PyEventBinder( wxEVT_CHAR_HOOK )
EVT_MENU_OPEN = wx.PyEventBinder( wxEVT_MENU_OPEN )
EVT_MENU_CLOSE = wx.PyEventBinder( wxEVT_MENU_CLOSE )
EVT_MENU_HIGHLIGHT = wx.PyEventBinder( wxEVT_MENU_HIGHLIGHT, 1)
EVT_MENU_HIGHLIGHT_ALL = wx.PyEventBinder( wxEVT_MENU_HIGHLIGHT )
EVT_SET_FOCUS = wx.PyEventBinder( wxEVT_SET_FOCUS )
EVT_KILL_FOCUS = wx.PyEventBinder( wxEVT_KILL_FOCUS )
EVT_CHILD_FOCUS = wx.PyEventBinder( wxEVT_CHILD_FOCUS )
EVT_ACTIVATE = wx.PyEventBinder( wxEVT_ACTIVATE )
EVT_ACTIVATE_APP = wx.PyEventBinder( wxEVT_ACTIVATE_APP )
EVT_HIBERNATE = wx.PyEventBinder( wxEVT_HIBERNATE )     
EVT_END_SESSION = wx.PyEventBinder( wxEVT_END_SESSION )
EVT_QUERY_END_SESSION = wx.PyEventBinder( wxEVT_QUERY_END_SESSION )
EVT_DROP_FILES = wx.PyEventBinder( wxEVT_DROP_FILES )
EVT_INIT_DIALOG = wx.PyEventBinder( wxEVT_INIT_DIALOG )
EVT_SYS_COLOUR_CHANGED = wx.PyEventBinder( wxEVT_SYS_COLOUR_CHANGED )
EVT_DISPLAY_CHANGED = wx.PyEventBinder( wxEVT_DISPLAY_CHANGED )
EVT_SHOW = wx.PyEventBinder( wxEVT_SHOW )
EVT_MAXIMIZE = wx.PyEventBinder( wxEVT_MAXIMIZE )
EVT_ICONIZE = wx.PyEventBinder( wxEVT_ICONIZE )
EVT_NAVIGATION_KEY = wx.PyEventBinder( wxEVT_NAVIGATION_KEY )
EVT_PALETTE_CHANGED = wx.PyEventBinder( wxEVT_PALETTE_CHANGED )
EVT_QUERY_NEW_PALETTE = wx.PyEventBinder( wxEVT_QUERY_NEW_PALETTE )
EVT_WINDOW_CREATE = wx.PyEventBinder( wxEVT_CREATE )
EVT_WINDOW_DESTROY = wx.PyEventBinder( wxEVT_DESTROY )
EVT_SET_CURSOR = wx.PyEventBinder( wxEVT_SET_CURSOR )
EVT_MOUSE_CAPTURE_CHANGED = wx.PyEventBinder( wxEVT_MOUSE_CAPTURE_CHANGED )

EVT_LEFT_DOWN = wx.PyEventBinder( wxEVT_LEFT_DOWN )
EVT_LEFT_UP = wx.PyEventBinder( wxEVT_LEFT_UP )
EVT_MIDDLE_DOWN = wx.PyEventBinder( wxEVT_MIDDLE_DOWN )
EVT_MIDDLE_UP = wx.PyEventBinder( wxEVT_MIDDLE_UP )
EVT_RIGHT_DOWN = wx.PyEventBinder( wxEVT_RIGHT_DOWN )
EVT_RIGHT_UP = wx.PyEventBinder( wxEVT_RIGHT_UP )
EVT_MOTION = wx.PyEventBinder( wxEVT_MOTION )
EVT_LEFT_DCLICK = wx.PyEventBinder( wxEVT_LEFT_DCLICK )
EVT_MIDDLE_DCLICK = wx.PyEventBinder( wxEVT_MIDDLE_DCLICK )
EVT_RIGHT_DCLICK = wx.PyEventBinder( wxEVT_RIGHT_DCLICK )
EVT_LEAVE_WINDOW = wx.PyEventBinder( wxEVT_LEAVE_WINDOW )
EVT_ENTER_WINDOW = wx.PyEventBinder( wxEVT_ENTER_WINDOW )
EVT_MOUSEWHEEL = wx.PyEventBinder( wxEVT_MOUSEWHEEL )

EVT_MOUSE_EVENTS = wx.PyEventBinder([ wxEVT_LEFT_DOWN,
                                     wxEVT_LEFT_UP,
                                     wxEVT_MIDDLE_DOWN,
                                     wxEVT_MIDDLE_UP,
                                     wxEVT_RIGHT_DOWN,
                                     wxEVT_RIGHT_UP,
                                     wxEVT_MOTION,
                                     wxEVT_LEFT_DCLICK,
                                     wxEVT_MIDDLE_DCLICK,
                                     wxEVT_RIGHT_DCLICK,
                                     wxEVT_ENTER_WINDOW,
                                     wxEVT_LEAVE_WINDOW,
                                     wxEVT_MOUSEWHEEL
                                     ])


# Scrolling from wxWindow (sent to wxScrolledWindow)
EVT_SCROLLWIN = wx.PyEventBinder([ wxEVT_SCROLLWIN_TOP, 
                                  wxEVT_SCROLLWIN_BOTTOM,
                                  wxEVT_SCROLLWIN_LINEUP,
                                  wxEVT_SCROLLWIN_LINEDOWN,
                                  wxEVT_SCROLLWIN_PAGEUP, 
                                  wxEVT_SCROLLWIN_PAGEDOWN,
                                  wxEVT_SCROLLWIN_THUMBTRACK,
                                  wxEVT_SCROLLWIN_THUMBRELEASE,
                                  ])

EVT_SCROLLWIN_TOP = wx.PyEventBinder( wxEVT_SCROLLWIN_TOP )
EVT_SCROLLWIN_BOTTOM = wx.PyEventBinder( wxEVT_SCROLLWIN_BOTTOM )
EVT_SCROLLWIN_LINEUP = wx.PyEventBinder( wxEVT_SCROLLWIN_LINEUP )
EVT_SCROLLWIN_LINEDOWN = wx.PyEventBinder( wxEVT_SCROLLWIN_LINEDOWN )
EVT_SCROLLWIN_PAGEUP = wx.PyEventBinder( wxEVT_SCROLLWIN_PAGEUP )
EVT_SCROLLWIN_PAGEDOWN = wx.PyEventBinder( wxEVT_SCROLLWIN_PAGEDOWN )
EVT_SCROLLWIN_THUMBTRACK = wx.PyEventBinder( wxEVT_SCROLLWIN_THUMBTRACK )
EVT_SCROLLWIN_THUMBRELEASE = wx.PyEventBinder( wxEVT_SCROLLWIN_THUMBRELEASE )

# Scrolling from wxSlider and wxScrollBar
EVT_SCROLL = wx.PyEventBinder([ wxEVT_SCROLL_TOP, 
                               wxEVT_SCROLL_BOTTOM, 
                               wxEVT_SCROLL_LINEUP, 
                               wxEVT_SCROLL_LINEDOWN, 
                               wxEVT_SCROLL_PAGEUP, 
                               wxEVT_SCROLL_PAGEDOWN, 
                               wxEVT_SCROLL_THUMBTRACK, 
                               wxEVT_SCROLL_THUMBRELEASE, 
                               wxEVT_SCROLL_ENDSCROLL,
                               ])

EVT_SCROLL_TOP = wx.PyEventBinder( wxEVT_SCROLL_TOP )
EVT_SCROLL_BOTTOM = wx.PyEventBinder( wxEVT_SCROLL_BOTTOM )
EVT_SCROLL_LINEUP = wx.PyEventBinder( wxEVT_SCROLL_LINEUP )
EVT_SCROLL_LINEDOWN = wx.PyEventBinder( wxEVT_SCROLL_LINEDOWN )
EVT_SCROLL_PAGEUP = wx.PyEventBinder( wxEVT_SCROLL_PAGEUP )
EVT_SCROLL_PAGEDOWN = wx.PyEventBinder( wxEVT_SCROLL_PAGEDOWN )
EVT_SCROLL_THUMBTRACK = wx.PyEventBinder( wxEVT_SCROLL_THUMBTRACK )
EVT_SCROLL_THUMBRELEASE = wx.PyEventBinder( wxEVT_SCROLL_THUMBRELEASE )
EVT_SCROLL_ENDSCROLL = wx.PyEventBinder( wxEVT_SCROLL_ENDSCROLL )

# Scrolling from wxSlider and wxScrollBar, with an id
EVT_COMMAND_SCROLL = wx.PyEventBinder([ wxEVT_SCROLL_TOP, 
                                       wxEVT_SCROLL_BOTTOM, 
                                       wxEVT_SCROLL_LINEUP, 
                                       wxEVT_SCROLL_LINEDOWN, 
                                       wxEVT_SCROLL_PAGEUP, 
                                       wxEVT_SCROLL_PAGEDOWN, 
                                       wxEVT_SCROLL_THUMBTRACK, 
                                       wxEVT_SCROLL_THUMBRELEASE,
                                       wxEVT_SCROLL_ENDSCROLL,
                                       ], 1)

EVT_COMMAND_SCROLL_TOP = wx.PyEventBinder( wxEVT_SCROLL_TOP, 1)
EVT_COMMAND_SCROLL_BOTTOM = wx.PyEventBinder( wxEVT_SCROLL_BOTTOM, 1)
EVT_COMMAND_SCROLL_LINEUP = wx.PyEventBinder( wxEVT_SCROLL_LINEUP, 1)
EVT_COMMAND_SCROLL_LINEDOWN = wx.PyEventBinder( wxEVT_SCROLL_LINEDOWN, 1)
EVT_COMMAND_SCROLL_PAGEUP = wx.PyEventBinder( wxEVT_SCROLL_PAGEUP, 1)
EVT_COMMAND_SCROLL_PAGEDOWN = wx.PyEventBinder( wxEVT_SCROLL_PAGEDOWN, 1)
EVT_COMMAND_SCROLL_THUMBTRACK = wx.PyEventBinder( wxEVT_SCROLL_THUMBTRACK, 1)
EVT_COMMAND_SCROLL_THUMBRELEASE = wx.PyEventBinder( wxEVT_SCROLL_THUMBRELEASE, 1)
EVT_COMMAND_SCROLL_ENDSCROLL = wx.PyEventBinder( wxEVT_SCROLL_ENDSCROLL, 1)

EVT_BUTTON = wx.PyEventBinder( wxEVT_COMMAND_BUTTON_CLICKED, 1)
EVT_CHECKBOX = wx.PyEventBinder( wxEVT_COMMAND_CHECKBOX_CLICKED, 1)
EVT_CHOICE = wx.PyEventBinder( wxEVT_COMMAND_CHOICE_SELECTED, 1)
EVT_LISTBOX = wx.PyEventBinder( wxEVT_COMMAND_LISTBOX_SELECTED, 1)
EVT_LISTBOX_DCLICK = wx.PyEventBinder( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, 1)
EVT_MENU = wx.PyEventBinder( wxEVT_COMMAND_MENU_SELECTED, 1)
EVT_MENU_RANGE = wx.PyEventBinder( wxEVT_COMMAND_MENU_SELECTED, 2)
EVT_SLIDER = wx.PyEventBinder( wxEVT_COMMAND_SLIDER_UPDATED, 1)
EVT_RADIOBOX = wx.PyEventBinder( wxEVT_COMMAND_RADIOBOX_SELECTED, 1)
EVT_RADIOBUTTON = wx.PyEventBinder( wxEVT_COMMAND_RADIOBUTTON_SELECTED, 1)

EVT_SCROLLBAR = wx.PyEventBinder( wxEVT_COMMAND_SCROLLBAR_UPDATED, 1)
EVT_VLBOX = wx.PyEventBinder( wxEVT_COMMAND_VLBOX_SELECTED, 1)
EVT_COMBOBOX = wx.PyEventBinder( wxEVT_COMMAND_COMBOBOX_SELECTED, 1)
EVT_TOOL = wx.PyEventBinder( wxEVT_COMMAND_TOOL_CLICKED, 1)
EVT_TOOL_RANGE = wx.PyEventBinder( wxEVT_COMMAND_TOOL_CLICKED, 2)
EVT_TOOL_RCLICKED = wx.PyEventBinder( wxEVT_COMMAND_TOOL_RCLICKED, 1)
EVT_TOOL_RCLICKED_RANGE = wx.PyEventBinder( wxEVT_COMMAND_TOOL_RCLICKED, 2)
EVT_TOOL_ENTER = wx.PyEventBinder( wxEVT_COMMAND_TOOL_ENTER, 1)
EVT_CHECKLISTBOX = wx.PyEventBinder( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, 1)


EVT_COMMAND_LEFT_CLICK = wx.PyEventBinder( wxEVT_COMMAND_LEFT_CLICK, 1)
EVT_COMMAND_LEFT_DCLICK = wx.PyEventBinder( wxEVT_COMMAND_LEFT_DCLICK, 1)
EVT_COMMAND_RIGHT_CLICK = wx.PyEventBinder( wxEVT_COMMAND_RIGHT_CLICK, 1)
EVT_COMMAND_RIGHT_DCLICK = wx.PyEventBinder( wxEVT_COMMAND_RIGHT_DCLICK, 1)
EVT_COMMAND_SET_FOCUS = wx.PyEventBinder( wxEVT_COMMAND_SET_FOCUS, 1)
EVT_COMMAND_KILL_FOCUS = wx.PyEventBinder( wxEVT_COMMAND_KILL_FOCUS, 1)
EVT_COMMAND_ENTER = wx.PyEventBinder( wxEVT_COMMAND_ENTER, 1)

EVT_IDLE = wx.PyEventBinder( wxEVT_IDLE )

EVT_UPDATE_UI = wx.PyEventBinder( wxEVT_UPDATE_UI, 1)
EVT_UPDATE_UI_RANGE = wx.PyEventBinder( wxEVT_UPDATE_UI, 2)

EVT_CONTEXT_MENU = wx.PyEventBinder( wxEVT_CONTEXT_MENU )



#---------------------------------------------------------------------------

class wxEvent(wxObject):
    """Proxy of C++ wxEvent class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=__core.delete_wxEvent):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetEventType(*args, **kwargs):
        """SetEventType(self, wxEventType typ)"""
        return __core.wxEvent_SetEventType(*args, **kwargs)

    def GetEventType(*args, **kwargs):
        """GetEventType(self) -> wxEventType"""
        return __core.wxEvent_GetEventType(*args, **kwargs)

    def GetEventObject(*args, **kwargs):
        """GetEventObject(self) -> wxObject"""
        return __core.wxEvent_GetEventObject(*args, **kwargs)

    def SetEventObject(*args, **kwargs):
        """SetEventObject(self, wxObject obj)"""
        return __core.wxEvent_SetEventObject(*args, **kwargs)

    def GetTimestamp(*args, **kwargs):
        """GetTimestamp(self) -> long"""
        return __core.wxEvent_GetTimestamp(*args, **kwargs)

    def SetTimestamp(*args, **kwargs):
        """SetTimestamp(self, long ts=0)"""
        return __core.wxEvent_SetTimestamp(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> int"""
        return __core.wxEvent_GetId(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(self, int Id)"""
        return __core.wxEvent_SetId(*args, **kwargs)

    def IsCommandEvent(*args, **kwargs):
        """IsCommandEvent(self) -> bool"""
        return __core.wxEvent_IsCommandEvent(*args, **kwargs)

    def Skip(*args, **kwargs):
        """
        Skip(self, bool skip=True)

        Called by an event handler, it controls whether additional event
        handlers bound to this event will be called after the current event
        handler returns.  Skip(false) (the default setting) will prevent
        additional event handlers from being called and control will be
        returned to the sender of the event immediately after the current
        handler has finished.  Skip(True) will cause the event processing
        system to continue searching for a handler function for this event.

        """
        return __core.wxEvent_Skip(*args, **kwargs)

    def GetSkipped(*args, **kwargs):
        """GetSkipped(self) -> bool"""
        return __core.wxEvent_GetSkipped(*args, **kwargs)

    def ShouldPropagate(*args, **kwargs):
        """ShouldPropagate(self) -> bool"""
        return __core.wxEvent_ShouldPropagate(*args, **kwargs)

    def StopPropagation(*args, **kwargs):
        """StopPropagation(self) -> int"""
        return __core.wxEvent_StopPropagation(*args, **kwargs)

    def ResumePropagation(*args, **kwargs):
        """ResumePropagation(self, int propagationLevel)"""
        return __core.wxEvent_ResumePropagation(*args, **kwargs)

    def Clone(*args, **kwargs):
        """Clone(self) -> wxEvent"""
        return __core.wxEvent_Clone(*args, **kwargs)


class wxEventPtr(wxEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxEvent
__core.wxEvent_swigregister(wxEventPtr)

#---------------------------------------------------------------------------

class wxPropagationDisabler(object):
    """Proxy of C++ wxPropagationDisabler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropagationDisabler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEvent event) -> wxPropagationDisabler"""
        newobj = __core.new_wxPropagationDisabler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxPropagationDisabler):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class wxPropagationDisablerPtr(wxPropagationDisabler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPropagationDisabler
__core.wxPropagationDisabler_swigregister(wxPropagationDisablerPtr)

class wxPropagateOnce(object):
    """Proxy of C++ wxPropagateOnce class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropagateOnce instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEvent event) -> wxPropagateOnce"""
        newobj = __core.new_wxPropagateOnce(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxPropagateOnce):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class wxPropagateOncePtr(wxPropagateOnce):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPropagateOnce
__core.wxPropagateOnce_swigregister(wxPropagateOncePtr)

#---------------------------------------------------------------------------

class wxCommandEvent(wxEvent):
    """Proxy of C++ wxCommandEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCommandEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int winid=0) -> wxCommandEvent"""
        newobj = __core.new_wxCommandEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> int"""
        return __core.wxCommandEvent_GetSelection(*args, **kwargs)

    def SetString(*args, **kwargs):
        """SetString(self, wxString s)"""
        return __core.wxCommandEvent_SetString(*args, **kwargs)

    def GetString(*args, **kwargs):
        """GetString(self) -> wxString"""
        return __core.wxCommandEvent_GetString(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked(self) -> bool"""
        return __core.wxCommandEvent_IsChecked(*args, **kwargs)

    Checked = IsChecked 
    def IsSelection(*args, **kwargs):
        """IsSelection(self) -> bool"""
        return __core.wxCommandEvent_IsSelection(*args, **kwargs)

    def SetExtraLong(*args, **kwargs):
        """SetExtraLong(self, long extraLong)"""
        return __core.wxCommandEvent_SetExtraLong(*args, **kwargs)

    def GetExtraLong(*args, **kwargs):
        """GetExtraLong(self) -> long"""
        return __core.wxCommandEvent_GetExtraLong(*args, **kwargs)

    def SetInt(*args, **kwargs):
        """SetInt(self, int i)"""
        return __core.wxCommandEvent_SetInt(*args, **kwargs)

    def GetInt(*args, **kwargs):
        """GetInt(self) -> long"""
        return __core.wxCommandEvent_GetInt(*args, **kwargs)

    def Clone(*args, **kwargs):
        """Clone(self) -> wxEvent"""
        return __core.wxCommandEvent_Clone(*args, **kwargs)


class wxCommandEventPtr(wxCommandEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxCommandEvent
__core.wxCommandEvent_swigregister(wxCommandEventPtr)

#---------------------------------------------------------------------------

class wxNotifyEvent(wxCommandEvent):
    """Proxy of C++ wxNotifyEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotifyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int winid=0) -> wxNotifyEvent"""
        newobj = __core.new_wxNotifyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Veto(*args, **kwargs):
        """Veto(self)"""
        return __core.wxNotifyEvent_Veto(*args, **kwargs)

    def Allow(*args, **kwargs):
        """Allow(self)"""
        return __core.wxNotifyEvent_Allow(*args, **kwargs)

    def IsAllowed(*args, **kwargs):
        """IsAllowed(self) -> bool"""
        return __core.wxNotifyEvent_IsAllowed(*args, **kwargs)


class wxNotifyEventPtr(wxNotifyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxNotifyEvent
__core.wxNotifyEvent_swigregister(wxNotifyEventPtr)

#---------------------------------------------------------------------------

class wxScrollEvent(wxCommandEvent):
    """Proxy of C++ wxScrollEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScrollEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxEventType commandType=wxEVT_NULL, int winid=0, int pos=0, 
            int orient=0) -> wxScrollEvent
        """
        newobj = __core.new_wxScrollEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetOrientation(*args, **kwargs):
        """GetOrientation(self) -> int"""
        return __core.wxScrollEvent_GetOrientation(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> int"""
        return __core.wxScrollEvent_GetPosition(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(self, int orient)"""
        return __core.wxScrollEvent_SetOrientation(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, int pos)"""
        return __core.wxScrollEvent_SetPosition(*args, **kwargs)


class wxScrollEventPtr(wxScrollEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxScrollEvent
__core.wxScrollEvent_swigregister(wxScrollEventPtr)

#---------------------------------------------------------------------------

class wxScrollWinEvent(wxEvent):
    """Proxy of C++ wxScrollWinEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScrollWinEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int pos=0, int orient=0) -> wxScrollWinEvent"""
        newobj = __core.new_wxScrollWinEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetOrientation(*args, **kwargs):
        """GetOrientation(self) -> int"""
        return __core.wxScrollWinEvent_GetOrientation(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> int"""
        return __core.wxScrollWinEvent_GetPosition(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(self, int orient)"""
        return __core.wxScrollWinEvent_SetOrientation(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, int pos)"""
        return __core.wxScrollWinEvent_SetPosition(*args, **kwargs)


class wxScrollWinEventPtr(wxScrollWinEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxScrollWinEvent
__core.wxScrollWinEvent_swigregister(wxScrollWinEventPtr)

#---------------------------------------------------------------------------

wxMOUSE_BTN_ANY = __core.wxMOUSE_BTN_ANY
wxMOUSE_BTN_NONE = __core.wxMOUSE_BTN_NONE
wxMOUSE_BTN_LEFT = __core.wxMOUSE_BTN_LEFT
wxMOUSE_BTN_MIDDLE = __core.wxMOUSE_BTN_MIDDLE
wxMOUSE_BTN_RIGHT = __core.wxMOUSE_BTN_RIGHT
class wxMouseEvent(wxEvent):
    """Proxy of C++ wxMouseEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMouseEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType mouseType=wxEVT_NULL) -> wxMouseEvent"""
        newobj = __core.new_wxMouseEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def IsButton(*args, **kwargs):
        """IsButton(self) -> bool"""
        return __core.wxMouseEvent_IsButton(*args, **kwargs)

    def ButtonDown(*args, **kwargs):
        """ButtonDown(self, int but=wxMOUSE_BTN_ANY) -> bool"""
        return __core.wxMouseEvent_ButtonDown(*args, **kwargs)

    def ButtonDClick(*args, **kwargs):
        """ButtonDClick(self, int but=wxMOUSE_BTN_ANY) -> bool"""
        return __core.wxMouseEvent_ButtonDClick(*args, **kwargs)

    def ButtonUp(*args, **kwargs):
        """ButtonUp(self, int but=wxMOUSE_BTN_ANY) -> bool"""
        return __core.wxMouseEvent_ButtonUp(*args, **kwargs)

    def Button(*args, **kwargs):
        """Button(self, int but) -> bool"""
        return __core.wxMouseEvent_Button(*args, **kwargs)

    def ButtonIsDown(*args, **kwargs):
        """ButtonIsDown(self, int but) -> bool"""
        return __core.wxMouseEvent_ButtonIsDown(*args, **kwargs)

    def GetButton(*args, **kwargs):
        """GetButton(self) -> int"""
        return __core.wxMouseEvent_GetButton(*args, **kwargs)

    def ControlDown(*args, **kwargs):
        """ControlDown(self) -> bool"""
        return __core.wxMouseEvent_ControlDown(*args, **kwargs)

    def MetaDown(*args, **kwargs):
        """MetaDown(self) -> bool"""
        return __core.wxMouseEvent_MetaDown(*args, **kwargs)

    def AltDown(*args, **kwargs):
        """AltDown(self) -> bool"""
        return __core.wxMouseEvent_AltDown(*args, **kwargs)

    def ShiftDown(*args, **kwargs):
        """ShiftDown(self) -> bool"""
        return __core.wxMouseEvent_ShiftDown(*args, **kwargs)

    def CmdDown(*args, **kwargs):
        """
        CmdDown(self) -> bool

        "Cmd" is a pseudo key which is the same as Control for PC and Unix
        platforms but the special "Apple" (a.k.a as "Command") key on
        Macs: it makes often sense to use it instead of, say, `ControlDown`
        because Cmd key is used for the same thing under Mac as Ctrl
        elsewhere. The Ctrl still exists, it's just not used for this
        purpose. So for non-Mac platforms this is the same as `ControlDown`
        and Macs this is the same as `MetaDown`.
        """
        return __core.wxMouseEvent_CmdDown(*args, **kwargs)

    def LeftDown(*args, **kwargs):
        """LeftDown(self) -> bool"""
        return __core.wxMouseEvent_LeftDown(*args, **kwargs)

    def MiddleDown(*args, **kwargs):
        """MiddleDown(self) -> bool"""
        return __core.wxMouseEvent_MiddleDown(*args, **kwargs)

    def RightDown(*args, **kwargs):
        """RightDown(self) -> bool"""
        return __core.wxMouseEvent_RightDown(*args, **kwargs)

    def LeftUp(*args, **kwargs):
        """LeftUp(self) -> bool"""
        return __core.wxMouseEvent_LeftUp(*args, **kwargs)

    def MiddleUp(*args, **kwargs):
        """MiddleUp(self) -> bool"""
        return __core.wxMouseEvent_MiddleUp(*args, **kwargs)

    def RightUp(*args, **kwargs):
        """RightUp(self) -> bool"""
        return __core.wxMouseEvent_RightUp(*args, **kwargs)

    def LeftDClick(*args, **kwargs):
        """LeftDClick(self) -> bool"""
        return __core.wxMouseEvent_LeftDClick(*args, **kwargs)

    def MiddleDClick(*args, **kwargs):
        """MiddleDClick(self) -> bool"""
        return __core.wxMouseEvent_MiddleDClick(*args, **kwargs)

    def RightDClick(*args, **kwargs):
        """RightDClick(self) -> bool"""
        return __core.wxMouseEvent_RightDClick(*args, **kwargs)

    def LeftIsDown(*args, **kwargs):
        """LeftIsDown(self) -> bool"""
        return __core.wxMouseEvent_LeftIsDown(*args, **kwargs)

    def MiddleIsDown(*args, **kwargs):
        """MiddleIsDown(self) -> bool"""
        return __core.wxMouseEvent_MiddleIsDown(*args, **kwargs)

    def RightIsDown(*args, **kwargs):
        """RightIsDown(self) -> bool"""
        return __core.wxMouseEvent_RightIsDown(*args, **kwargs)

    def Dragging(*args, **kwargs):
        """Dragging(self) -> bool"""
        return __core.wxMouseEvent_Dragging(*args, **kwargs)

    def Moving(*args, **kwargs):
        """Moving(self) -> bool"""
        return __core.wxMouseEvent_Moving(*args, **kwargs)

    def Entering(*args, **kwargs):
        """Entering(self) -> bool"""
        return __core.wxMouseEvent_Entering(*args, **kwargs)

    def Leaving(*args, **kwargs):
        """Leaving(self) -> bool"""
        return __core.wxMouseEvent_Leaving(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> wxPoint

        Returns the position of the mouse in window coordinates when the event
        happened.
        """
        return __core.wxMouseEvent_GetPosition(*args, **kwargs)

    def GetPositionTuple(*args, **kwargs):
        """
        GetPositionTuple() -> (x,y)

        Returns the position of the mouse in window coordinates when the event
        happened.
        """
        return __core.wxMouseEvent_GetPositionTuple(*args, **kwargs)

    def GetLogicalPosition(*args, **kwargs):
        """GetLogicalPosition(self, wxDC dc) -> wxPoint"""
        return __core.wxMouseEvent_GetLogicalPosition(*args, **kwargs)

    def GetX(*args, **kwargs):
        """GetX(self) -> int"""
        return __core.wxMouseEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return __core.wxMouseEvent_GetY(*args, **kwargs)

    def GetWheelRotation(*args, **kwargs):
        """GetWheelRotation(self) -> int"""
        return __core.wxMouseEvent_GetWheelRotation(*args, **kwargs)

    def GetWheelDelta(*args, **kwargs):
        """GetWheelDelta(self) -> int"""
        return __core.wxMouseEvent_GetWheelDelta(*args, **kwargs)

    def GetLinesPerAction(*args, **kwargs):
        """GetLinesPerAction(self) -> int"""
        return __core.wxMouseEvent_GetLinesPerAction(*args, **kwargs)

    def IsPageScroll(*args, **kwargs):
        """IsPageScroll(self) -> bool"""
        return __core.wxMouseEvent_IsPageScroll(*args, **kwargs)

    m_x = property(__core.wxMouseEvent_m_x_get, __core.wxMouseEvent_m_x_set)
    m_y = property(__core.wxMouseEvent_m_y_get, __core.wxMouseEvent_m_y_set)
    m_leftDown = property(__core.wxMouseEvent_m_leftDown_get, __core.wxMouseEvent_m_leftDown_set)
    m_middleDown = property(__core.wxMouseEvent_m_middleDown_get, __core.wxMouseEvent_m_middleDown_set)
    m_rightDown = property(__core.wxMouseEvent_m_rightDown_get, __core.wxMouseEvent_m_rightDown_set)
    m_controlDown = property(__core.wxMouseEvent_m_controlDown_get, __core.wxMouseEvent_m_controlDown_set)
    m_shiftDown = property(__core.wxMouseEvent_m_shiftDown_get, __core.wxMouseEvent_m_shiftDown_set)
    m_altDown = property(__core.wxMouseEvent_m_altDown_get, __core.wxMouseEvent_m_altDown_set)
    m_metaDown = property(__core.wxMouseEvent_m_metaDown_get, __core.wxMouseEvent_m_metaDown_set)
    m_wheelRotation = property(__core.wxMouseEvent_m_wheelRotation_get, __core.wxMouseEvent_m_wheelRotation_set)
    m_wheelDelta = property(__core.wxMouseEvent_m_wheelDelta_get, __core.wxMouseEvent_m_wheelDelta_set)
    m_linesPerAction = property(__core.wxMouseEvent_m_linesPerAction_get, __core.wxMouseEvent_m_linesPerAction_set)

class wxMouseEventPtr(wxMouseEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxMouseEvent
__core.wxMouseEvent_swigregister(wxMouseEventPtr)

#---------------------------------------------------------------------------

class wxSetCursorEvent(wxEvent):
    """Proxy of C++ wxSetCursorEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSetCursorEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int x=0, int y=0) -> wxSetCursorEvent"""
        newobj = __core.new_wxSetCursorEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetX(*args, **kwargs):
        """GetX(self) -> int"""
        return __core.wxSetCursorEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return __core.wxSetCursorEvent_GetY(*args, **kwargs)

    def SetCursor(*args, **kwargs):
        """SetCursor(self, wxCursor cursor)"""
        return __core.wxSetCursorEvent_SetCursor(*args, **kwargs)

    def GetCursor(*args, **kwargs):
        """GetCursor(self) -> wxCursor"""
        return __core.wxSetCursorEvent_GetCursor(*args, **kwargs)

    def HasCursor(*args, **kwargs):
        """HasCursor(self) -> bool"""
        return __core.wxSetCursorEvent_HasCursor(*args, **kwargs)


class wxSetCursorEventPtr(wxSetCursorEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxSetCursorEvent
__core.wxSetCursorEvent_swigregister(wxSetCursorEventPtr)

#---------------------------------------------------------------------------

class wxKeyEvent(wxEvent):
    """Proxy of C++ wxKeyEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxKeyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType keyType=wxEVT_NULL) -> wxKeyEvent"""
        newobj = __core.new_wxKeyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def ControlDown(*args, **kwargs):
        """ControlDown(self) -> bool"""
        return __core.wxKeyEvent_ControlDown(*args, **kwargs)

    def MetaDown(*args, **kwargs):
        """MetaDown(self) -> bool"""
        return __core.wxKeyEvent_MetaDown(*args, **kwargs)

    def AltDown(*args, **kwargs):
        """AltDown(self) -> bool"""
        return __core.wxKeyEvent_AltDown(*args, **kwargs)

    def ShiftDown(*args, **kwargs):
        """ShiftDown(self) -> bool"""
        return __core.wxKeyEvent_ShiftDown(*args, **kwargs)

    def CmdDown(*args, **kwargs):
        """
        CmdDown(self) -> bool

        "Cmd" is a pseudo key which is the same as Control for PC and Unix
        platforms but the special "Apple" (a.k.a as "Command") key on
        Macs: it makes often sense to use it instead of, say, `ControlDown`
        because Cmd key is used for the same thing under Mac as Ctrl
        elsewhere. The Ctrl still exists, it's just not used for this
        purpose. So for non-Mac platforms this is the same as `ControlDown`
        and Macs this is the same as `MetaDown`.
        """
        return __core.wxKeyEvent_CmdDown(*args, **kwargs)

    def HasModifiers(*args, **kwargs):
        """HasModifiers(self) -> bool"""
        return __core.wxKeyEvent_HasModifiers(*args, **kwargs)

    def GetKeyCode(*args, **kwargs):
        """GetKeyCode(self) -> int"""
        return __core.wxKeyEvent_GetKeyCode(*args, **kwargs)

    KeyCode = GetKeyCode 
    def GetUnicodeKey(*args, **kwargs):
        """GetUnicodeKey(self) -> int"""
        return __core.wxKeyEvent_GetUnicodeKey(*args, **kwargs)

    GetUniChar = GetUnicodeKey 
    def GetRawKeyCode(*args, **kwargs):
        """GetRawKeyCode(self) -> unsigned int"""
        return __core.wxKeyEvent_GetRawKeyCode(*args, **kwargs)

    def GetRawKeyFlags(*args, **kwargs):
        """GetRawKeyFlags(self) -> unsigned int"""
        return __core.wxKeyEvent_GetRawKeyFlags(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> wxPoint

        Find the position of the event.
        """
        return __core.wxKeyEvent_GetPosition(*args, **kwargs)

    def GetPositionTuple(*args, **kwargs):
        """
        GetPositionTuple() -> (x,y)

        Find the position of the event.
        """
        return __core.wxKeyEvent_GetPositionTuple(*args, **kwargs)

    def GetX(*args, **kwargs):
        """GetX(self) -> int"""
        return __core.wxKeyEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return __core.wxKeyEvent_GetY(*args, **kwargs)

    m_x = property(__core.wxKeyEvent_m_x_get, __core.wxKeyEvent_m_x_set)
    m_y = property(__core.wxKeyEvent_m_y_get, __core.wxKeyEvent_m_y_set)
    m_keyCode = property(__core.wxKeyEvent_m_keyCode_get, __core.wxKeyEvent_m_keyCode_set)
    m_controlDown = property(__core.wxKeyEvent_m_controlDown_get, __core.wxKeyEvent_m_controlDown_set)
    m_shiftDown = property(__core.wxKeyEvent_m_shiftDown_get, __core.wxKeyEvent_m_shiftDown_set)
    m_altDown = property(__core.wxKeyEvent_m_altDown_get, __core.wxKeyEvent_m_altDown_set)
    m_metaDown = property(__core.wxKeyEvent_m_metaDown_get, __core.wxKeyEvent_m_metaDown_set)
    m_scanCode = property(__core.wxKeyEvent_m_scanCode_get, __core.wxKeyEvent_m_scanCode_set)
    m_rawCode = property(__core.wxKeyEvent_m_rawCode_get, __core.wxKeyEvent_m_rawCode_set)
    m_rawFlags = property(__core.wxKeyEvent_m_rawFlags_get, __core.wxKeyEvent_m_rawFlags_set)

class wxKeyEventPtr(wxKeyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxKeyEvent
__core.wxKeyEvent_swigregister(wxKeyEventPtr)

#---------------------------------------------------------------------------

class wxSizeEvent(wxEvent):
    """Proxy of C++ wxSizeEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSizeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxSize sz=wxDefaultSize, int winid=0) -> wxSizeEvent"""
        newobj = __core.new_wxSizeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetSize(*args, **kwargs):
        """GetSize(self) -> wxSize"""
        return __core.wxSizeEvent_GetSize(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """GetRect(self) -> wxRect"""
        return __core.wxSizeEvent_GetRect(*args, **kwargs)

    def SetRect(*args, **kwargs):
        """SetRect(self, wxRect rect)"""
        return __core.wxSizeEvent_SetRect(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, wxSize size)"""
        return __core.wxSizeEvent_SetSize(*args, **kwargs)

    m_size = property(__core.wxSizeEvent_m_size_get, __core.wxSizeEvent_m_size_set)
    m_rect = property(__core.wxSizeEvent_m_rect_get, __core.wxSizeEvent_m_rect_set)

class wxSizeEventPtr(wxSizeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxSizeEvent
__core.wxSizeEvent_swigregister(wxSizeEventPtr)

#---------------------------------------------------------------------------

class wxMoveEvent(wxEvent):
    """Proxy of C++ wxMoveEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMoveEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxPoint pos=wxDefaultPosition, int winid=0) -> wxMoveEvent"""
        newobj = __core.new_wxMoveEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> wxPoint"""
        return __core.wxMoveEvent_GetPosition(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """GetRect(self) -> wxRect"""
        return __core.wxMoveEvent_GetRect(*args, **kwargs)

    def SetRect(*args, **kwargs):
        """SetRect(self, wxRect rect)"""
        return __core.wxMoveEvent_SetRect(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, wxPoint pos)"""
        return __core.wxMoveEvent_SetPosition(*args, **kwargs)

    m_pos =  property(GetPosition, SetPosition)
    m_rect = property(GetRect, SetRect)


class wxMoveEventPtr(wxMoveEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxMoveEvent
__core.wxMoveEvent_swigregister(wxMoveEventPtr)

#---------------------------------------------------------------------------

class wxPaintEvent(wxEvent):
    """Proxy of C++ wxPaintEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPaintEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int Id=0) -> wxPaintEvent"""
        newobj = __core.new_wxPaintEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxPaintEventPtr(wxPaintEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPaintEvent
__core.wxPaintEvent_swigregister(wxPaintEventPtr)

class wxNcPaintEvent(wxEvent):
    """Proxy of C++ wxNcPaintEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNcPaintEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid=0) -> wxNcPaintEvent"""
        newobj = __core.new_wxNcPaintEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxNcPaintEventPtr(wxNcPaintEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxNcPaintEvent
__core.wxNcPaintEvent_swigregister(wxNcPaintEventPtr)

#---------------------------------------------------------------------------

class wxEraseEvent(wxEvent):
    """Proxy of C++ wxEraseEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEraseEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int Id=0, wxDC dc=(wxDC *) NULL) -> wxEraseEvent"""
        newobj = __core.new_wxEraseEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetDC(*args, **kwargs):
        """GetDC(self) -> wxDC"""
        return __core.wxEraseEvent_GetDC(*args, **kwargs)


class wxEraseEventPtr(wxEraseEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxEraseEvent
__core.wxEraseEvent_swigregister(wxEraseEventPtr)

#---------------------------------------------------------------------------

class wxFocusEvent(wxEvent):
    """Proxy of C++ wxFocusEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFocusEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, int winid=0) -> wxFocusEvent"""
        newobj = __core.new_wxFocusEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> wxWindow"""
        return __core.wxFocusEvent_GetWindow(*args, **kwargs)

    def SetWindow(*args, **kwargs):
        """SetWindow(self, wxWindow win)"""
        return __core.wxFocusEvent_SetWindow(*args, **kwargs)


class wxFocusEventPtr(wxFocusEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxFocusEvent
__core.wxFocusEvent_swigregister(wxFocusEventPtr)

#---------------------------------------------------------------------------

class wxChildFocusEvent(wxCommandEvent):
    """Proxy of C++ wxChildFocusEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxChildFocusEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxWindow win=None) -> wxChildFocusEvent"""
        newobj = __core.new_wxChildFocusEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> wxWindow"""
        return __core.wxChildFocusEvent_GetWindow(*args, **kwargs)


class wxChildFocusEventPtr(wxChildFocusEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxChildFocusEvent
__core.wxChildFocusEvent_swigregister(wxChildFocusEventPtr)

#---------------------------------------------------------------------------

class wxActivateEvent(wxEvent):
    """Proxy of C++ wxActivateEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxActivateEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, bool active=True, int Id=0) -> wxActivateEvent"""
        newobj = __core.new_wxActivateEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetActive(*args, **kwargs):
        """GetActive(self) -> bool"""
        return __core.wxActivateEvent_GetActive(*args, **kwargs)


class wxActivateEventPtr(wxActivateEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxActivateEvent
__core.wxActivateEvent_swigregister(wxActivateEventPtr)

#---------------------------------------------------------------------------

class wxInitDialogEvent(wxEvent):
    """Proxy of C++ wxInitDialogEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxInitDialogEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int Id=0) -> wxInitDialogEvent"""
        newobj = __core.new_wxInitDialogEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxInitDialogEventPtr(wxInitDialogEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxInitDialogEvent
__core.wxInitDialogEvent_swigregister(wxInitDialogEventPtr)

#---------------------------------------------------------------------------

class wxMenuEvent(wxEvent):
    """Proxy of C++ wxMenuEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMenuEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, int winid=0, wxMenu menu=None) -> wxMenuEvent"""
        newobj = __core.new_wxMenuEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetMenuId(*args, **kwargs):
        """GetMenuId(self) -> int"""
        return __core.wxMenuEvent_GetMenuId(*args, **kwargs)

    def IsPopup(*args, **kwargs):
        """IsPopup(self) -> bool"""
        return __core.wxMenuEvent_IsPopup(*args, **kwargs)

    def GetMenu(*args, **kwargs):
        """GetMenu(self) -> wxMenu"""
        return __core.wxMenuEvent_GetMenu(*args, **kwargs)


class wxMenuEventPtr(wxMenuEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxMenuEvent
__core.wxMenuEvent_swigregister(wxMenuEventPtr)

#---------------------------------------------------------------------------

class wxCloseEvent(wxEvent):
    """Proxy of C++ wxCloseEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCloseEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, int winid=0) -> wxCloseEvent"""
        newobj = __core.new_wxCloseEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetLoggingOff(*args, **kwargs):
        """SetLoggingOff(self, bool logOff)"""
        return __core.wxCloseEvent_SetLoggingOff(*args, **kwargs)

    def GetLoggingOff(*args, **kwargs):
        """GetLoggingOff(self) -> bool"""
        return __core.wxCloseEvent_GetLoggingOff(*args, **kwargs)

    def Veto(*args, **kwargs):
        """Veto(self, bool veto=True)"""
        return __core.wxCloseEvent_Veto(*args, **kwargs)

    def SetCanVeto(*args, **kwargs):
        """SetCanVeto(self, bool canVeto)"""
        return __core.wxCloseEvent_SetCanVeto(*args, **kwargs)

    def CanVeto(*args, **kwargs):
        """CanVeto(self) -> bool"""
        return __core.wxCloseEvent_CanVeto(*args, **kwargs)

    def GetVeto(*args, **kwargs):
        """GetVeto(self) -> bool"""
        return __core.wxCloseEvent_GetVeto(*args, **kwargs)


class wxCloseEventPtr(wxCloseEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxCloseEvent
__core.wxCloseEvent_swigregister(wxCloseEventPtr)

#---------------------------------------------------------------------------

class wxShowEvent(wxEvent):
    """Proxy of C++ wxShowEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxShowEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid=0, bool show=False) -> wxShowEvent"""
        newobj = __core.new_wxShowEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetShow(*args, **kwargs):
        """SetShow(self, bool show)"""
        return __core.wxShowEvent_SetShow(*args, **kwargs)

    def GetShow(*args, **kwargs):
        """GetShow(self) -> bool"""
        return __core.wxShowEvent_GetShow(*args, **kwargs)


class wxShowEventPtr(wxShowEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxShowEvent
__core.wxShowEvent_swigregister(wxShowEventPtr)

#---------------------------------------------------------------------------

class wxIconizeEvent(wxEvent):
    """Proxy of C++ wxIconizeEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIconizeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int id=0, bool iconized=True) -> wxIconizeEvent"""
        newobj = __core.new_wxIconizeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Iconized(*args, **kwargs):
        """Iconized(self) -> bool"""
        return __core.wxIconizeEvent_Iconized(*args, **kwargs)


class wxIconizeEventPtr(wxIconizeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxIconizeEvent
__core.wxIconizeEvent_swigregister(wxIconizeEventPtr)

#---------------------------------------------------------------------------

class wxMaximizeEvent(wxEvent):
    """Proxy of C++ wxMaximizeEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMaximizeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int id=0) -> wxMaximizeEvent"""
        newobj = __core.new_wxMaximizeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxMaximizeEventPtr(wxMaximizeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxMaximizeEvent
__core.wxMaximizeEvent_swigregister(wxMaximizeEventPtr)

#---------------------------------------------------------------------------

class wxDropFilesEvent(wxEvent):
    """Proxy of C++ wxDropFilesEvent class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDropFilesEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> wxPoint"""
        return __core.wxDropFilesEvent_GetPosition(*args, **kwargs)

    def GetNumberOfFiles(*args, **kwargs):
        """GetNumberOfFiles(self) -> int"""
        return __core.wxDropFilesEvent_GetNumberOfFiles(*args, **kwargs)

    def GetFiles(*args, **kwargs):
        """GetFiles(self) -> PyObject"""
        return __core.wxDropFilesEvent_GetFiles(*args, **kwargs)


class wxDropFilesEventPtr(wxDropFilesEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxDropFilesEvent
__core.wxDropFilesEvent_swigregister(wxDropFilesEventPtr)

#---------------------------------------------------------------------------

wxUPDATE_UI_PROCESS_ALL = __core.wxUPDATE_UI_PROCESS_ALL
wxUPDATE_UI_PROCESS_SPECIFIED = __core.wxUPDATE_UI_PROCESS_SPECIFIED
class wxUpdateUIEvent(wxCommandEvent):
    """Proxy of C++ wxUpdateUIEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxUpdateUIEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int commandId=0) -> wxUpdateUIEvent"""
        newobj = __core.new_wxUpdateUIEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetChecked(*args, **kwargs):
        """GetChecked(self) -> bool"""
        return __core.wxUpdateUIEvent_GetChecked(*args, **kwargs)

    def GetEnabled(*args, **kwargs):
        """GetEnabled(self) -> bool"""
        return __core.wxUpdateUIEvent_GetEnabled(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText(self) -> wxString"""
        return __core.wxUpdateUIEvent_GetText(*args, **kwargs)

    def GetSetText(*args, **kwargs):
        """GetSetText(self) -> bool"""
        return __core.wxUpdateUIEvent_GetSetText(*args, **kwargs)

    def GetSetChecked(*args, **kwargs):
        """GetSetChecked(self) -> bool"""
        return __core.wxUpdateUIEvent_GetSetChecked(*args, **kwargs)

    def GetSetEnabled(*args, **kwargs):
        """GetSetEnabled(self) -> bool"""
        return __core.wxUpdateUIEvent_GetSetEnabled(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(self, bool check)"""
        return __core.wxUpdateUIEvent_Check(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(self, bool enable)"""
        return __core.wxUpdateUIEvent_Enable(*args, **kwargs)

    def SetText(*args, **kwargs):
        """SetText(self, wxString text)"""
        return __core.wxUpdateUIEvent_SetText(*args, **kwargs)

    def SetUpdateInterval(*args, **kwargs):
        """SetUpdateInterval(long updateInterval)"""
        return __core.wxUpdateUIEvent_SetUpdateInterval(*args, **kwargs)

    SetUpdateInterval = staticmethod(SetUpdateInterval)
    def GetUpdateInterval(*args, **kwargs):
        """GetUpdateInterval() -> long"""
        return __core.wxUpdateUIEvent_GetUpdateInterval(*args, **kwargs)

    GetUpdateInterval = staticmethod(GetUpdateInterval)
    def CanUpdate(*args, **kwargs):
        """CanUpdate(wxWindow win) -> bool"""
        return __core.wxUpdateUIEvent_CanUpdate(*args, **kwargs)

    CanUpdate = staticmethod(CanUpdate)
    def ResetUpdateTime(*args, **kwargs):
        """ResetUpdateTime()"""
        return __core.wxUpdateUIEvent_ResetUpdateTime(*args, **kwargs)

    ResetUpdateTime = staticmethod(ResetUpdateTime)
    def SetMode(*args, **kwargs):
        """SetMode(int mode)"""
        return __core.wxUpdateUIEvent_SetMode(*args, **kwargs)

    SetMode = staticmethod(SetMode)
    def GetMode(*args, **kwargs):
        """GetMode() -> int"""
        return __core.wxUpdateUIEvent_GetMode(*args, **kwargs)

    GetMode = staticmethod(GetMode)

class wxUpdateUIEventPtr(wxUpdateUIEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxUpdateUIEvent
__core.wxUpdateUIEvent_swigregister(wxUpdateUIEventPtr)

def wxUpdateUIEvent_SetUpdateInterval(*args, **kwargs):
    """wxUpdateUIEvent_SetUpdateInterval(long updateInterval)"""
    return __core.wxUpdateUIEvent_SetUpdateInterval(*args, **kwargs)

def wxUpdateUIEvent_GetUpdateInterval(*args, **kwargs):
    """wxUpdateUIEvent_GetUpdateInterval() -> long"""
    return __core.wxUpdateUIEvent_GetUpdateInterval(*args, **kwargs)

def wxUpdateUIEvent_CanUpdate(*args, **kwargs):
    """wxUpdateUIEvent_CanUpdate(wxWindow win) -> bool"""
    return __core.wxUpdateUIEvent_CanUpdate(*args, **kwargs)

def wxUpdateUIEvent_ResetUpdateTime(*args, **kwargs):
    """wxUpdateUIEvent_ResetUpdateTime()"""
    return __core.wxUpdateUIEvent_ResetUpdateTime(*args, **kwargs)

def wxUpdateUIEvent_SetMode(*args, **kwargs):
    """wxUpdateUIEvent_SetMode(int mode)"""
    return __core.wxUpdateUIEvent_SetMode(*args, **kwargs)

def wxUpdateUIEvent_GetMode(*args, **kwargs):
    """wxUpdateUIEvent_GetMode() -> int"""
    return __core.wxUpdateUIEvent_GetMode(*args, **kwargs)

#---------------------------------------------------------------------------

class wxSysColourChangedEvent(wxEvent):
    """Proxy of C++ wxSysColourChangedEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSysColourChangedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxSysColourChangedEvent"""
        newobj = __core.new_wxSysColourChangedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxSysColourChangedEventPtr(wxSysColourChangedEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxSysColourChangedEvent
__core.wxSysColourChangedEvent_swigregister(wxSysColourChangedEventPtr)

#---------------------------------------------------------------------------

class wxMouseCaptureChangedEvent(wxEvent):
    """Proxy of C++ wxMouseCaptureChangedEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMouseCaptureChangedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid=0, wxWindow gainedCapture=None) -> wxMouseCaptureChangedEvent"""
        newobj = __core.new_wxMouseCaptureChangedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetCapturedWindow(*args, **kwargs):
        """GetCapturedWindow(self) -> wxWindow"""
        return __core.wxMouseCaptureChangedEvent_GetCapturedWindow(*args, **kwargs)


class wxMouseCaptureChangedEventPtr(wxMouseCaptureChangedEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxMouseCaptureChangedEvent
__core.wxMouseCaptureChangedEvent_swigregister(wxMouseCaptureChangedEventPtr)

#---------------------------------------------------------------------------

class wxDisplayChangedEvent(wxEvent):
    """Proxy of C++ wxDisplayChangedEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDisplayChangedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxDisplayChangedEvent"""
        newobj = __core.new_wxDisplayChangedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class wxDisplayChangedEventPtr(wxDisplayChangedEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxDisplayChangedEvent
__core.wxDisplayChangedEvent_swigregister(wxDisplayChangedEventPtr)

#---------------------------------------------------------------------------

class wxPaletteChangedEvent(wxEvent):
    """Proxy of C++ wxPaletteChangedEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPaletteChangedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int id=0) -> wxPaletteChangedEvent"""
        newobj = __core.new_wxPaletteChangedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetChangedWindow(*args, **kwargs):
        """SetChangedWindow(self, wxWindow win)"""
        return __core.wxPaletteChangedEvent_SetChangedWindow(*args, **kwargs)

    def GetChangedWindow(*args, **kwargs):
        """GetChangedWindow(self) -> wxWindow"""
        return __core.wxPaletteChangedEvent_GetChangedWindow(*args, **kwargs)


class wxPaletteChangedEventPtr(wxPaletteChangedEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPaletteChangedEvent
__core.wxPaletteChangedEvent_swigregister(wxPaletteChangedEventPtr)

#---------------------------------------------------------------------------

class wxQueryNewPaletteEvent(wxEvent):
    """Proxy of C++ wxQueryNewPaletteEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxQueryNewPaletteEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid=0) -> wxQueryNewPaletteEvent"""
        newobj = __core.new_wxQueryNewPaletteEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetPaletteRealized(*args, **kwargs):
        """SetPaletteRealized(self, bool realized)"""
        return __core.wxQueryNewPaletteEvent_SetPaletteRealized(*args, **kwargs)

    def GetPaletteRealized(*args, **kwargs):
        """GetPaletteRealized(self) -> bool"""
        return __core.wxQueryNewPaletteEvent_GetPaletteRealized(*args, **kwargs)


class wxQueryNewPaletteEventPtr(wxQueryNewPaletteEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxQueryNewPaletteEvent
__core.wxQueryNewPaletteEvent_swigregister(wxQueryNewPaletteEventPtr)

#---------------------------------------------------------------------------

class wxNavigationKeyEvent(wxEvent):
    """Proxy of C++ wxNavigationKeyEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNavigationKeyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxNavigationKeyEvent"""
        newobj = __core.new_wxNavigationKeyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetDirection(*args, **kwargs):
        """GetDirection(self) -> bool"""
        return __core.wxNavigationKeyEvent_GetDirection(*args, **kwargs)

    def SetDirection(*args, **kwargs):
        """SetDirection(self, bool forward)"""
        return __core.wxNavigationKeyEvent_SetDirection(*args, **kwargs)

    def IsWindowChange(*args, **kwargs):
        """IsWindowChange(self) -> bool"""
        return __core.wxNavigationKeyEvent_IsWindowChange(*args, **kwargs)

    def SetWindowChange(*args, **kwargs):
        """SetWindowChange(self, bool ischange)"""
        return __core.wxNavigationKeyEvent_SetWindowChange(*args, **kwargs)

    def IsFromTab(*args, **kwargs):
        """IsFromTab(self) -> bool"""
        return __core.wxNavigationKeyEvent_IsFromTab(*args, **kwargs)

    def SetFromTab(*args, **kwargs):
        """SetFromTab(self, bool bIs)"""
        return __core.wxNavigationKeyEvent_SetFromTab(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(self, long flags)"""
        return __core.wxNavigationKeyEvent_SetFlags(*args, **kwargs)

    def GetCurrentFocus(*args, **kwargs):
        """GetCurrentFocus(self) -> wxWindow"""
        return __core.wxNavigationKeyEvent_GetCurrentFocus(*args, **kwargs)

    def SetCurrentFocus(*args, **kwargs):
        """SetCurrentFocus(self, wxWindow win)"""
        return __core.wxNavigationKeyEvent_SetCurrentFocus(*args, **kwargs)

    IsBackward = __core.wxNavigationKeyEvent_IsBackward
    IsForward = __core.wxNavigationKeyEvent_IsForward
    WinChange = __core.wxNavigationKeyEvent_WinChange
    FromTab = __core.wxNavigationKeyEvent_FromTab

class wxNavigationKeyEventPtr(wxNavigationKeyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxNavigationKeyEvent
__core.wxNavigationKeyEvent_swigregister(wxNavigationKeyEventPtr)

#---------------------------------------------------------------------------

class wxWindowCreateEvent(wxCommandEvent):
    """Proxy of C++ wxWindowCreateEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWindowCreateEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxWindow win=None) -> wxWindowCreateEvent"""
        newobj = __core.new_wxWindowCreateEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> wxWindow"""
        return __core.wxWindowCreateEvent_GetWindow(*args, **kwargs)


class wxWindowCreateEventPtr(wxWindowCreateEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxWindowCreateEvent
__core.wxWindowCreateEvent_swigregister(wxWindowCreateEventPtr)

class wxWindowDestroyEvent(wxCommandEvent):
    """Proxy of C++ wxWindowDestroyEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWindowDestroyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxWindow win=None) -> wxWindowDestroyEvent"""
        newobj = __core.new_wxWindowDestroyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> wxWindow"""
        return __core.wxWindowDestroyEvent_GetWindow(*args, **kwargs)


class wxWindowDestroyEventPtr(wxWindowDestroyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxWindowDestroyEvent
__core.wxWindowDestroyEvent_swigregister(wxWindowDestroyEventPtr)

#---------------------------------------------------------------------------

class wxContextMenuEvent(wxCommandEvent):
    """Proxy of C++ wxContextMenuEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxContextMenuEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, int winid=0, wxPoint pt=wxDefaultPosition) -> wxContextMenuEvent"""
        newobj = __core.new_wxContextMenuEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> wxPoint"""
        return __core.wxContextMenuEvent_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, wxPoint pos)"""
        return __core.wxContextMenuEvent_SetPosition(*args, **kwargs)


class wxContextMenuEventPtr(wxContextMenuEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxContextMenuEvent
__core.wxContextMenuEvent_swigregister(wxContextMenuEventPtr)

#---------------------------------------------------------------------------

wxIDLE_PROCESS_ALL = __core.wxIDLE_PROCESS_ALL
wxIDLE_PROCESS_SPECIFIED = __core.wxIDLE_PROCESS_SPECIFIED
class wxIdleEvent(wxEvent):
    """Proxy of C++ wxIdleEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIdleEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxIdleEvent"""
        newobj = __core.new_wxIdleEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def RequestMore(*args, **kwargs):
        """RequestMore(self, bool needMore=True)"""
        return __core.wxIdleEvent_RequestMore(*args, **kwargs)

    def MoreRequested(*args, **kwargs):
        """MoreRequested(self) -> bool"""
        return __core.wxIdleEvent_MoreRequested(*args, **kwargs)

    def SetMode(*args, **kwargs):
        """SetMode(int mode)"""
        return __core.wxIdleEvent_SetMode(*args, **kwargs)

    SetMode = staticmethod(SetMode)
    def GetMode(*args, **kwargs):
        """GetMode() -> int"""
        return __core.wxIdleEvent_GetMode(*args, **kwargs)

    GetMode = staticmethod(GetMode)
    def CanSend(*args, **kwargs):
        """CanSend(wxWindow win) -> bool"""
        return __core.wxIdleEvent_CanSend(*args, **kwargs)

    CanSend = staticmethod(CanSend)

class wxIdleEventPtr(wxIdleEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxIdleEvent
__core.wxIdleEvent_swigregister(wxIdleEventPtr)

def wxIdleEvent_SetMode(*args, **kwargs):
    """wxIdleEvent_SetMode(int mode)"""
    return __core.wxIdleEvent_SetMode(*args, **kwargs)

def wxIdleEvent_GetMode(*args, **kwargs):
    """wxIdleEvent_GetMode() -> int"""
    return __core.wxIdleEvent_GetMode(*args, **kwargs)

def wxIdleEvent_CanSend(*args, **kwargs):
    """wxIdleEvent_CanSend(wxWindow win) -> bool"""
    return __core.wxIdleEvent_CanSend(*args, **kwargs)

#---------------------------------------------------------------------------

class wxPyEvent(wxEvent):
    """Proxy of C++ wxPyEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid=0, wxEventType commandType=wxEVT_NULL) -> wxPyEvent"""
        newobj = __core.new_wxPyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self.SetSelf(self)

    def __del__(self, destroy=__core.delete_wxPyEvent):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetSelf(*args, **kwargs):
        """SetSelf(self, PyObject self)"""
        return __core.wxPyEvent_SetSelf(*args, **kwargs)

    def GetSelf(*args, **kwargs):
        """GetSelf(self) -> PyObject"""
        return __core.wxPyEvent_GetSelf(*args, **kwargs)


class wxPyEventPtr(wxPyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPyEvent
__core.wxPyEvent_swigregister(wxPyEventPtr)

class wxPyCommandEvent(wxCommandEvent):
    """Proxy of C++ wxPyCommandEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyCommandEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int id=0) -> wxPyCommandEvent"""
        newobj = __core.new_wxPyCommandEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self.SetSelf(self)

    def __del__(self, destroy=__core.delete_wxPyCommandEvent):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetSelf(*args, **kwargs):
        """SetSelf(self, PyObject self)"""
        return __core.wxPyCommandEvent_SetSelf(*args, **kwargs)

    def GetSelf(*args, **kwargs):
        """GetSelf(self) -> PyObject"""
        return __core.wxPyCommandEvent_GetSelf(*args, **kwargs)


class wxPyCommandEventPtr(wxPyCommandEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPyCommandEvent
__core.wxPyCommandEvent_swigregister(wxPyCommandEventPtr)

class wxDateEvent(wxCommandEvent):
    """Proxy of C++ wxDateEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDateEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxWindow win, wxDateTime dt, wxEventType type) -> wxDateEvent"""
        newobj = __core.new_wxDateEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetDate(*args, **kwargs):
        """GetDate(self) -> wxDateTime"""
        return __core.wxDateEvent_GetDate(*args, **kwargs)

    def SetDate(*args, **kwargs):
        """SetDate(self, wxDateTime date)"""
        return __core.wxDateEvent_SetDate(*args, **kwargs)


class wxDateEventPtr(wxDateEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxDateEvent
__core.wxDateEvent_swigregister(wxDateEventPtr)

wxEVT_DATE_CHANGED = __core.wxEVT_DATE_CHANGED
EVT_DATE_CHANGED = wx.PyEventBinder( wxEVT_DATE_CHANGED, 1 )

#---------------------------------------------------------------------------

wxPYAPP_ASSERT_SUPPRESS = __core.wxPYAPP_ASSERT_SUPPRESS
wxPYAPP_ASSERT_EXCEPTION = __core.wxPYAPP_ASSERT_EXCEPTION
wxPYAPP_ASSERT_DIALOG = __core.wxPYAPP_ASSERT_DIALOG
wxPYAPP_ASSERT_LOG = __core.wxPYAPP_ASSERT_LOG
wxPRINT_WINDOWS = __core.wxPRINT_WINDOWS
wxPRINT_POSTSCRIPT = __core.wxPRINT_POSTSCRIPT
class wxPyApp(wxEvtHandler):
    """
    The ``wx.PyApp`` class is an *implementation detail*, please use the
    `wx.App` class (or some other derived class) instead.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyApp instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> wxPyApp

        Create a new application object, starting the bootstrap process.
        """
        newobj = __core.new_wxPyApp(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyApp)
        self._setOORInfo(self)

    def __del__(self, destroy=__core.delete_wxPyApp):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return __core.wxPyApp__setCallbackInfo(*args, **kwargs)

    def GetAppName(*args, **kwargs):
        """
        GetAppName(self) -> wxString

        Get the application name.
        """
        return __core.wxPyApp_GetAppName(*args, **kwargs)

    def SetAppName(*args, **kwargs):
        """
        SetAppName(self, wxString name)

        Set the application name. This value may be used automatically by
        `wx.Config` and such.
        """
        return __core.wxPyApp_SetAppName(*args, **kwargs)

    def GetClassName(*args, **kwargs):
        """
        GetClassName(self) -> wxString

        Get the application's class name.
        """
        return __core.wxPyApp_GetClassName(*args, **kwargs)

    def SetClassName(*args, **kwargs):
        """
        SetClassName(self, wxString name)

        Set the application's class name. This value may be used for
        X-resources if applicable for the platform
        """
        return __core.wxPyApp_SetClassName(*args, **kwargs)

    def GetVendorName(*args, **kwargs):
        """
        GetVendorName(self) -> wxString

        Get the application's vendor name.
        """
        return __core.wxPyApp_GetVendorName(*args, **kwargs)

    def SetVendorName(*args, **kwargs):
        """
        SetVendorName(self, wxString name)

        Set the application's vendor name. This value may be used
        automatically by `wx.Config` and such.
        """
        return __core.wxPyApp_SetVendorName(*args, **kwargs)

    def GetTraits(*args, **kwargs):
        """
        GetTraits(self) -> wxAppTraits

        Return (and create if necessary) the app traits object to which we
        delegate for everything which either should be configurable by the
        user (then he can change the default behaviour simply by overriding
        CreateTraits() and returning his own traits object) or which is
        GUI/console dependent as then wx.AppTraits allows us to abstract the
        differences behind the common facade.

        :todo: Add support for overriding CreateAppTraits in wxPython.
        """
        return __core.wxPyApp_GetTraits(*args, **kwargs)

    def ProcessPendingEvents(*args, **kwargs):
        """
        ProcessPendingEvents(self)

        Process all events in the Pending Events list -- it is necessary to
        call this function to process posted events. This normally happens
        during each event loop iteration.
        """
        return __core.wxPyApp_ProcessPendingEvents(*args, **kwargs)

    def Yield(*args, **kwargs):
        """
        Yield(self, bool onlyIfNeeded=False) -> bool

        Process all currently pending events right now, instead of waiting
        until return to the event loop.  It is an error to call ``Yield``
        recursively unless the value of ``onlyIfNeeded`` is True.

        :warning: This function is dangerous as it can lead to unexpected
              reentrancies (i.e. when called from an event handler it may
              result in calling the same event handler again), use with
              extreme care or, better, don't use at all!

        :see: `wx.Yield`, `wx.YieldIfNeeded`, `wx.SafeYield`

        """
        return __core.wxPyApp_Yield(*args, **kwargs)

    def WakeUpIdle(*args, **kwargs):
        """
        WakeUpIdle(self)

        Make sure that idle events are sent again.
        :see: `wx.WakeUpIdle`
        """
        return __core.wxPyApp_WakeUpIdle(*args, **kwargs)

    def IsMainLoopRunning(*args, **kwargs):
        """
        IsMainLoopRunning() -> bool

        Returns True if we're running the main loop, i.e. if the events can
        currently be dispatched.
        """
        return __core.wxPyApp_IsMainLoopRunning(*args, **kwargs)

    IsMainLoopRunning = staticmethod(IsMainLoopRunning)
    def MainLoop(*args, **kwargs):
        """
        MainLoop(self) -> int

        Execute the main GUI loop, the function doesn't normally return until
        all top level windows have been closed and destroyed.
        """
        return __core.wxPyApp_MainLoop(*args, **kwargs)

    def Exit(*args, **kwargs):
        """
        Exit(self)

        Exit the main loop thus terminating the application.
        :see: `wx.Exit`
        """
        return __core.wxPyApp_Exit(*args, **kwargs)

    def ExitMainLoop(*args, **kwargs):
        """
        ExitMainLoop(self)

        Exit the main GUI loop during the next iteration of the main
        loop, (i.e. it does not stop the program immediately!)
        """
        return __core.wxPyApp_ExitMainLoop(*args, **kwargs)

    def Pending(*args, **kwargs):
        """
        Pending(self) -> bool

        Returns True if there are unprocessed events in the event queue.
        """
        return __core.wxPyApp_Pending(*args, **kwargs)

    def Dispatch(*args, **kwargs):
        """
        Dispatch(self) -> bool

        Process the first event in the event queue (blocks until an event
        appears if there are none currently)
        """
        return __core.wxPyApp_Dispatch(*args, **kwargs)

    def ProcessIdle(*args, **kwargs):
        """
        ProcessIdle(self) -> bool

        Called from the MainLoop when the application becomes idle (there are
        no pending events) and sends a `wx.IdleEvent` to all interested
        parties.  Returns True if more idle events are needed, False if not.
        """
        return __core.wxPyApp_ProcessIdle(*args, **kwargs)

    def SendIdleEvents(*args, **kwargs):
        """
        SendIdleEvents(self, wxWindow win, wxIdleEvent event) -> bool

        Send idle event to window and all subwindows.  Returns True if more
        idle time is requested.
        """
        return __core.wxPyApp_SendIdleEvents(*args, **kwargs)

    def IsActive(*args, **kwargs):
        """
        IsActive(self) -> bool

        Return True if our app has focus.
        """
        return __core.wxPyApp_IsActive(*args, **kwargs)

    def SetTopWindow(*args, **kwargs):
        """
        SetTopWindow(self, wxWindow win)

        Set the *main* top level window
        """
        return __core.wxPyApp_SetTopWindow(*args, **kwargs)

    def GetTopWindow(*args, **kwargs):
        """
        GetTopWindow(self) -> wxWindow

        Return the *main* top level window (if it hadn't been set previously
        with SetTopWindow(), will return just some top level window and, if
        there not any, will return None)
        """
        return __core.wxPyApp_GetTopWindow(*args, **kwargs)

    def SetExitOnFrameDelete(*args, **kwargs):
        """
        SetExitOnFrameDelete(self, bool flag)

        Control the exit behaviour: by default, the program will exit the main
        loop (and so, usually, terminate) when the last top-level program
        window is deleted.  Beware that if you disable this behaviour (with
        SetExitOnFrameDelete(False)), you'll have to call ExitMainLoop()
        explicitly from somewhere.
        """
        return __core.wxPyApp_SetExitOnFrameDelete(*args, **kwargs)

    def GetExitOnFrameDelete(*args, **kwargs):
        """
        GetExitOnFrameDelete(self) -> bool

        Get the current exit behaviour setting.
        """
        return __core.wxPyApp_GetExitOnFrameDelete(*args, **kwargs)

    def SetUseBestVisual(*args, **kwargs):
        """
        SetUseBestVisual(self, bool flag)

        Set whether the app should try to use the best available visual on
        systems where more than one is available, (Sun, SGI, XFree86 4, etc.)
        """
        return __core.wxPyApp_SetUseBestVisual(*args, **kwargs)

    def GetUseBestVisual(*args, **kwargs):
        """
        GetUseBestVisual(self) -> bool

        Get current UseBestVisual setting.
        """
        return __core.wxPyApp_GetUseBestVisual(*args, **kwargs)

    def SetPrintMode(*args, **kwargs):
        """SetPrintMode(self, int mode)"""
        return __core.wxPyApp_SetPrintMode(*args, **kwargs)

    def GetPrintMode(*args, **kwargs):
        """GetPrintMode(self) -> int"""
        return __core.wxPyApp_GetPrintMode(*args, **kwargs)

    def SetAssertMode(*args, **kwargs):
        """
        SetAssertMode(self, int mode)

        Set the OnAssert behaviour for debug and hybrid builds.
        """
        return __core.wxPyApp_SetAssertMode(*args, **kwargs)

    def GetAssertMode(*args, **kwargs):
        """
        GetAssertMode(self) -> int

        Get the current OnAssert behaviour setting.
        """
        return __core.wxPyApp_GetAssertMode(*args, **kwargs)

    def GetMacSupportPCMenuShortcuts(*args, **kwargs):
        """GetMacSupportPCMenuShortcuts() -> bool"""
        return __core.wxPyApp_GetMacSupportPCMenuShortcuts(*args, **kwargs)

    GetMacSupportPCMenuShortcuts = staticmethod(GetMacSupportPCMenuShortcuts)
    def GetMacAboutMenuItemId(*args, **kwargs):
        """GetMacAboutMenuItemId() -> long"""
        return __core.wxPyApp_GetMacAboutMenuItemId(*args, **kwargs)

    GetMacAboutMenuItemId = staticmethod(GetMacAboutMenuItemId)
    def GetMacPreferencesMenuItemId(*args, **kwargs):
        """GetMacPreferencesMenuItemId() -> long"""
        return __core.wxPyApp_GetMacPreferencesMenuItemId(*args, **kwargs)

    GetMacPreferencesMenuItemId = staticmethod(GetMacPreferencesMenuItemId)
    def GetMacExitMenuItemId(*args, **kwargs):
        """GetMacExitMenuItemId() -> long"""
        return __core.wxPyApp_GetMacExitMenuItemId(*args, **kwargs)

    GetMacExitMenuItemId = staticmethod(GetMacExitMenuItemId)
    def GetMacHelpMenuTitleName(*args, **kwargs):
        """GetMacHelpMenuTitleName() -> wxString"""
        return __core.wxPyApp_GetMacHelpMenuTitleName(*args, **kwargs)

    GetMacHelpMenuTitleName = staticmethod(GetMacHelpMenuTitleName)
    def SetMacSupportPCMenuShortcuts(*args, **kwargs):
        """SetMacSupportPCMenuShortcuts(bool val)"""
        return __core.wxPyApp_SetMacSupportPCMenuShortcuts(*args, **kwargs)

    SetMacSupportPCMenuShortcuts = staticmethod(SetMacSupportPCMenuShortcuts)
    def SetMacAboutMenuItemId(*args, **kwargs):
        """SetMacAboutMenuItemId(long val)"""
        return __core.wxPyApp_SetMacAboutMenuItemId(*args, **kwargs)

    SetMacAboutMenuItemId = staticmethod(SetMacAboutMenuItemId)
    def SetMacPreferencesMenuItemId(*args, **kwargs):
        """SetMacPreferencesMenuItemId(long val)"""
        return __core.wxPyApp_SetMacPreferencesMenuItemId(*args, **kwargs)

    SetMacPreferencesMenuItemId = staticmethod(SetMacPreferencesMenuItemId)
    def SetMacExitMenuItemId(*args, **kwargs):
        """SetMacExitMenuItemId(long val)"""
        return __core.wxPyApp_SetMacExitMenuItemId(*args, **kwargs)

    SetMacExitMenuItemId = staticmethod(SetMacExitMenuItemId)
    def SetMacHelpMenuTitleName(*args, **kwargs):
        """SetMacHelpMenuTitleName(wxString val)"""
        return __core.wxPyApp_SetMacHelpMenuTitleName(*args, **kwargs)

    SetMacHelpMenuTitleName = staticmethod(SetMacHelpMenuTitleName)
    def _BootstrapApp(*args, **kwargs):
        """
        _BootstrapApp(self)

        For internal use only
        """
        return __core.wxPyApp__BootstrapApp(*args, **kwargs)

    def GetComCtl32Version(*args, **kwargs):
        """
        GetComCtl32Version() -> int

        Returns 400, 470, 471, etc. for comctl32.dll 4.00, 4.70, 4.71 or 0 if
        it wasn't found at all.  Raises an exception on non-Windows platforms.
        """
        return __core.wxPyApp_GetComCtl32Version(*args, **kwargs)

    GetComCtl32Version = staticmethod(GetComCtl32Version)

class wxPyAppPtr(wxPyApp):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPyApp
__core.wxPyApp_swigregister(wxPyAppPtr)

def wxPyApp_IsMainLoopRunning(*args, **kwargs):
    """
    wxPyApp_IsMainLoopRunning() -> bool

    Returns True if we're running the main loop, i.e. if the events can
    currently be dispatched.
    """
    return __core.wxPyApp_IsMainLoopRunning(*args, **kwargs)

def wxPyApp_GetMacSupportPCMenuShortcuts(*args, **kwargs):
    """wxPyApp_GetMacSupportPCMenuShortcuts() -> bool"""
    return __core.wxPyApp_GetMacSupportPCMenuShortcuts(*args, **kwargs)

def wxPyApp_GetMacAboutMenuItemId(*args, **kwargs):
    """wxPyApp_GetMacAboutMenuItemId() -> long"""
    return __core.wxPyApp_GetMacAboutMenuItemId(*args, **kwargs)

def wxPyApp_GetMacPreferencesMenuItemId(*args, **kwargs):
    """wxPyApp_GetMacPreferencesMenuItemId() -> long"""
    return __core.wxPyApp_GetMacPreferencesMenuItemId(*args, **kwargs)

def wxPyApp_GetMacExitMenuItemId(*args, **kwargs):
    """wxPyApp_GetMacExitMenuItemId() -> long"""
    return __core.wxPyApp_GetMacExitMenuItemId(*args, **kwargs)

def wxPyApp_GetMacHelpMenuTitleName(*args, **kwargs):
    """wxPyApp_GetMacHelpMenuTitleName() -> wxString"""
    return __core.wxPyApp_GetMacHelpMenuTitleName(*args, **kwargs)

def wxPyApp_SetMacSupportPCMenuShortcuts(*args, **kwargs):
    """wxPyApp_SetMacSupportPCMenuShortcuts(bool val)"""
    return __core.wxPyApp_SetMacSupportPCMenuShortcuts(*args, **kwargs)

def wxPyApp_SetMacAboutMenuItemId(*args, **kwargs):
    """wxPyApp_SetMacAboutMenuItemId(long val)"""
    return __core.wxPyApp_SetMacAboutMenuItemId(*args, **kwargs)

def wxPyApp_SetMacPreferencesMenuItemId(*args, **kwargs):
    """wxPyApp_SetMacPreferencesMenuItemId(long val)"""
    return __core.wxPyApp_SetMacPreferencesMenuItemId(*args, **kwargs)

def wxPyApp_SetMacExitMenuItemId(*args, **kwargs):
    """wxPyApp_SetMacExitMenuItemId(long val)"""
    return __core.wxPyApp_SetMacExitMenuItemId(*args, **kwargs)

def wxPyApp_SetMacHelpMenuTitleName(*args, **kwargs):
    """wxPyApp_SetMacHelpMenuTitleName(wxString val)"""
    return __core.wxPyApp_SetMacHelpMenuTitleName(*args, **kwargs)

def wxPyApp_GetComCtl32Version(*args, **kwargs):
    """
    wxPyApp_GetComCtl32Version() -> int

    Returns 400, 470, 471, etc. for comctl32.dll 4.00, 4.70, 4.71 or 0 if
    it wasn't found at all.  Raises an exception on non-Windows platforms.
    """
    return __core.wxPyApp_GetComCtl32Version(*args, **kwargs)

#---------------------------------------------------------------------------


def wxExit(*args, **kwargs):
    """
    wxExit()

    Force an exit of the application.  Convenience for wx.GetApp().Exit()
    """
    return __core.wxExit(*args, **kwargs)

def wxYield(*args, **kwargs):
    """
    wxYield() -> bool

    Yield to other apps/messages.  Convenience for wx.GetApp().Yield()
    """
    return __core.wxYield(*args, **kwargs)

def wxYieldIfNeeded(*args, **kwargs):
    """
    wxYieldIfNeeded() -> bool

    Yield to other apps/messages.  Convenience for wx.GetApp().Yield(True)
    """
    return __core.wxYieldIfNeeded(*args, **kwargs)

def wxSafeYield(*args, **kwargs):
    """
    wxSafeYield(wxWindow win=None, bool onlyIfNeeded=False) -> bool

    This function is similar to `wx.Yield`, except that it disables the
    user input to all program windows before calling `wx.Yield` and
    re-enables it again afterwards. If ``win`` is not None, this window
    will remain enabled, allowing the implementation of some limited user
    interaction.

    :Returns: the result of the call to `wx.Yield`.
    """
    return __core.wxSafeYield(*args, **kwargs)

def wxWakeUpIdle(*args, **kwargs):
    """
    wxWakeUpIdle()

    Cause the message queue to become empty again, so idle events will be
    sent.
    """
    return __core.wxWakeUpIdle(*args, **kwargs)

def wxPostEvent(*args, **kwargs):
    """
    wxPostEvent(wxEvtHandler dest, wxEvent event)

    Send an event to a window or other wx.EvtHandler to be processed
    later.
    """
    return __core.wxPostEvent(*args, **kwargs)

def wxApp_CleanUp(*args, **kwargs):
    """
    wxApp_CleanUp()

    For internal use only, it is used to cleanup after wxWidgets when
    Python shuts down.
    """
    return __core.wxApp_CleanUp(*args, **kwargs)

def GetApp(*args, **kwargs):
    """
    GetApp() -> wxPyApp

    Return a reference to the current wx.App object.
    """
    return __core.GetApp(*args, **kwargs)

def wxSetDefaultPyEncoding(*args, **kwargs):
    """
    SetDefaultPyEncoding(string encoding)

    Sets the encoding that wxPython will use when it needs to convert a
    Python string or unicode object to or from a wxString.

    The default encoding is the value of ``locale.getdefaultlocale()[1]``
    but please be aware that the default encoding within the same locale
    may be slightly different on different platforms.  For example, please
    see http://www.alanwood.net/demos/charsetdiffs.html for differences
    between the common latin/roman encodings.
    """
    return __core.wxSetDefaultPyEncoding(*args, **kwargs)

def wxGetDefaultPyEncoding(*args, **kwargs):
    """
    GetDefaultPyEncoding() -> string

    Gets the current encoding that wxPython will use when it needs to
    convert a Python string or unicode object to or from a wxString.
    """
    return __core.wxGetDefaultPyEncoding(*args, **kwargs)
#----------------------------------------------------------------------

class PyOnDemandOutputWindow:
    """
    A class that can be used for redirecting Python's stdout and
    stderr streams.  It will do nothing until something is wrriten to
    the stream at which point it will create a Frame with a text area
    and write the text there.
    """
    def __init__(self, title = "wxPython: stdout/stderr"):
        self.frame  = None
        self.title  = title
        self.pos    = wx.DefaultPosition
        self.size   = (450, 300)
        self.parent = None

    def SetParent(self, parent):
        """Set the window to be used as the popup Frame's parent."""
        self.parent = parent


    def CreateOutputWindow(self, st):
        self.frame = wx.Frame(self.parent, -1, self.title, self.pos, self.size,
                              style=wx.DEFAULT_FRAME_STYLE)
        self.text  = wx.TextCtrl(self.frame, -1, "",
                                 style=wx.TE_MULTILINE|wx.TE_READONLY)
        self.text.AppendText(st)
        self.frame.Show(True)
        EVT_CLOSE(self.frame, self.OnCloseWindow)
        

    def OnCloseWindow(self, event):
        if self.frame is not None:
            self.frame.Destroy()
        self.frame = None
        self.text  = None


    # These methods provide the file-like output behaviour.
    def write(self, text):
        """
        Create the output window if needed and write the string to it.
        If not called in the context of the gui thread then uses
        CallAfter to do the work there.
        """        
        if self.frame is None:
            if not wx.Thread_IsMain():
                wx.CallAfter(self.CreateOutputWindow, text)
            else:
                self.CreateOutputWindow(text)
        else:
            if not wx.Thread_IsMain():
                wx.CallAfter(self.text.AppendText, text)
            else:
                self.text.AppendText(text)


    def close(self):
        if self.frame is not None:
            wx.CallAfter(self.frame.Close)


    def flush(self):
        pass
    


#----------------------------------------------------------------------

_defRedirect = (wx.Platform == '__WXMSW__' or wx.Platform == '__WXMAC__')

class App(wx.PyApp):
    """
    The ``wx.App`` class represents the application and is used to:

      * bootstrap the wxPython system and initialize the underlying
        gui toolkit
      * set and get application-wide properties
      * implement the windowing system main message or event loop,
        and to dispatch events to window instances
      * etc.

    Every application must have a ``wx.App`` instance, and all
    creation of UI objects should be delayed until after the
    ``wx.App`` object has been created in order to ensure that the gui
    platform and wxWidgets have been fully initialized.

    Normally you would derive from this class and implement an
    ``OnInit`` method that creates a frame and then calls
    ``self.SetTopWindow(frame)``.

    :see: `wx.PySimpleApp` for a simpler app class that can be used
           directly.
    """
    
    outputWindowClass = PyOnDemandOutputWindow

    def __init__(self, redirect=_defRedirect, filename=None,
                 useBestVisual=False, clearSigInt=True):
        """
        Construct a ``wx.App`` object.  

        :param redirect: Should ``sys.stdout`` and ``sys.stderr`` be
            redirected?  Defaults to True on Windows and Mac, False
            otherwise.  If `filename` is None then output will be
            redirected to a window that pops up as needed.  (You can
            control what kind of window is created for the output by
            resetting the class variable ``outputWindowClass`` to a
            class of your choosing.)

        :param filename: The name of a file to redirect output to, if
            redirect is True.

        :param useBestVisual: Should the app try to use the best
            available visual provided by the system (only relevant on
            systems that have more than one visual.)  This parameter
            must be used instead of calling `SetUseBestVisual` later
            on because it must be set before the underlying GUI
            toolkit is initialized.

        :param clearSigInt: Should SIGINT be cleared?  This allows the
            app to terminate upon a Ctrl-C in the console like other
            GUI apps will.

        :note: You should override OnInit to do applicaition
            initialization to ensure that the system, toolkit and
            wxWidgets are fully initialized.
        """
        wx.PyApp.__init__(self)

        if wx.Platform == "__WXMAC__":
            try:
                import MacOS
                if not MacOS.WMAvailable():
                    print """\
This program needs access to the screen. Please run with 'pythonw',
not 'python', and only when you are logged in on the main display of
your Mac."""
                    _sys.exit(1)
            except SystemExit:
                raise
            except:
                pass

        # This has to be done before OnInit
        self.SetUseBestVisual(useBestVisual)

        # Set the default handler for SIGINT.  This fixes a problem
        # where if Ctrl-C is pressed in the console that started this
        # app then it will not appear to do anything, (not even send
        # KeyboardInterrupt???)  but will later segfault on exit.  By
        # setting the default handler then the app will exit, as
        # expected (depending on platform.)
        if clearSigInt:
            try:
                import signal
                signal.signal(signal.SIGINT, signal.SIG_DFL)
            except:
                pass

        # Save and redirect the stdio to a window?
        self.stdioWin = None
        self.saveStdio = (_sys.stdout, _sys.stderr)
        if redirect:
            self.RedirectStdio(filename)

        # Use Python's install prefix as the default  
        wx.StandardPaths.Get().SetInstallPrefix(_sys.prefix)

        # This finishes the initialization of wxWindows and then calls
        # the OnInit that should be present in the derived class
        self._BootstrapApp()


    def __del__(self):
        try:
            self.RestoreStdio()  # Just in case the MainLoop was overridden
        except:
            pass


    def SetTopWindow(self, frame):
        """Set the \"main\" top level window"""
        if self.stdioWin:
            self.stdioWin.SetParent(frame)
        wx.PyApp.SetTopWindow(self, frame)


    def MainLoop(self):
        """Execute the main GUI event loop"""
        wx.PyApp.MainLoop(self)
        self.RestoreStdio()


    def RedirectStdio(self, filename=None):
        """Redirect sys.stdout and sys.stderr to a file or a popup window."""
        if filename:
            _sys.stdout = _sys.stderr = open(filename, 'a')
        else:
            self.stdioWin = self.outputWindowClass()
            _sys.stdout = _sys.stderr = self.stdioWin


    def RestoreStdio(self):
        _sys.stdout, _sys.stderr = self.saveStdio


    def SetOutputWindowAttributes(self, title=None, pos=None, size=None):
        """
        Set the title, position and/or size of the output window if
        the stdio has been redirected.  This should be called before
        any output would cause the output window to be created.
        """
        if self.stdioWin:
            if title is not None:
                self.stdioWin.title = title
            if pos is not None:
                self.stdioWin.pos = pos
            if size is not None:
                self.stdioWin.size = size
            



# change from wx.PyApp_XX to wx.App_XX
App_GetMacSupportPCMenuShortcuts = _core_.PyApp_GetMacSupportPCMenuShortcuts
App_GetMacAboutMenuItemId        = _core_.PyApp_GetMacAboutMenuItemId
App_GetMacPreferencesMenuItemId  = _core_.PyApp_GetMacPreferencesMenuItemId
App_GetMacExitMenuItemId         = _core_.PyApp_GetMacExitMenuItemId
App_GetMacHelpMenuTitleName      = _core_.PyApp_GetMacHelpMenuTitleName
App_SetMacSupportPCMenuShortcuts = _core_.PyApp_SetMacSupportPCMenuShortcuts
App_SetMacAboutMenuItemId        = _core_.PyApp_SetMacAboutMenuItemId
App_SetMacPreferencesMenuItemId  = _core_.PyApp_SetMacPreferencesMenuItemId
App_SetMacExitMenuItemId         = _core_.PyApp_SetMacExitMenuItemId
App_SetMacHelpMenuTitleName      = _core_.PyApp_SetMacHelpMenuTitleName
App_GetComCtl32Version           = _core_.PyApp_GetComCtl32Version

#----------------------------------------------------------------------------

class PySimpleApp(wx.App):
    """
    A simple application class.  You can just create one of these and
    then then make your top level windows later, and not have to worry
    about OnInit.  For example::

        app = wx.PySimpleApp()
        frame = wx.Frame(None, title='Hello World')
        frame.Show()
        app.MainLoop()

    :see: `wx.App` 
    """

    def __init__(self, redirect=False, filename=None,
                 useBestVisual=False, clearSigInt=True):
        """
        :see: `wx.App.__init__`
        """
        wx.App.__init__(self, redirect, filename, useBestVisual, clearSigInt)
        
    def OnInit(self):
        return True



# Is anybody using this one?
class PyWidgetTester(wx.App):
    def __init__(self, size = (250, 100)):
        self.size = size
        wx.App.__init__(self, 0)

    def OnInit(self):
        self.frame = wx.Frame(None, -1, "Widget Tester", pos=(0,0), size=self.size)
        self.SetTopWindow(self.frame)
        return True

    def SetWidget(self, widgetClass, *args, **kwargs):
        w = widgetClass(self.frame, *args, **kwargs)
        self.frame.Show(True)

#----------------------------------------------------------------------------
# DO NOT hold any other references to this object.  This is how we
# know when to cleanup system resources that wxWidgets is holding.  When
# the sys module is unloaded, the refcount on sys.__wxPythonCleanup
# goes to zero and it calls the wx.App_CleanUp function.

class __wxPyCleanup:
    def __init__(self):
        self.cleanup = _core_.App_CleanUp
    def __del__(self):
        self.cleanup()

_sys.__wxPythonCleanup = __wxPyCleanup()

## # another possible solution, but it gets called too early...
## import atexit
## atexit.register(_core_.wxApp_CleanUp)


#----------------------------------------------------------------------------

#---------------------------------------------------------------------------

class wxEventLoop(object):
    """Proxy of C++ wxEventLoop class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEventLoop instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxEventLoop"""
        newobj = __core.new_wxEventLoop(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxEventLoop):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Run(*args, **kwargs):
        """Run(self) -> int"""
        return __core.wxEventLoop_Run(*args, **kwargs)

    def Exit(*args, **kwargs):
        """Exit(self, int rc=0)"""
        return __core.wxEventLoop_Exit(*args, **kwargs)

    def Pending(*args, **kwargs):
        """Pending(self) -> bool"""
        return __core.wxEventLoop_Pending(*args, **kwargs)

    def Dispatch(*args, **kwargs):
        """Dispatch(self) -> bool"""
        return __core.wxEventLoop_Dispatch(*args, **kwargs)

    def IsRunning(*args, **kwargs):
        """IsRunning(self) -> bool"""
        return __core.wxEventLoop_IsRunning(*args, **kwargs)

    def GetActive(*args, **kwargs):
        """GetActive() -> wxEventLoop"""
        return __core.wxEventLoop_GetActive(*args, **kwargs)

    GetActive = staticmethod(GetActive)
    def SetActive(*args, **kwargs):
        """SetActive(wxEventLoop loop)"""
        return __core.wxEventLoop_SetActive(*args, **kwargs)

    SetActive = staticmethod(SetActive)

class wxEventLoopPtr(wxEventLoop):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxEventLoop
__core.wxEventLoop_swigregister(wxEventLoopPtr)

def wxEventLoop_GetActive(*args, **kwargs):
    """wxEventLoop_GetActive() -> wxEventLoop"""
    return __core.wxEventLoop_GetActive(*args, **kwargs)

def wxEventLoop_SetActive(*args, **kwargs):
    """wxEventLoop_SetActive(wxEventLoop loop)"""
    return __core.wxEventLoop_SetActive(*args, **kwargs)

#---------------------------------------------------------------------------

class wxAcceleratorEntry(object):
    """
    A class used to define items in an `wx.AcceleratorTable`.  wxPython
    programs can choose to use wx.AcceleratorEntry objects, but using a
    list of 3-tuple of integers (flags, keyCode, cmdID) usually works just
    as well.  See `__init__` for  of the tuple values.

    :see: `wx.AcceleratorTable`
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxAcceleratorEntry instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int flags=0, int keyCode=0, int cmdID=0) -> wxAcceleratorEntry

        Construct a wx.AcceleratorEntry.
        """
        newobj = __core.new_wxAcceleratorEntry(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxAcceleratorEntry):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Set(*args, **kwargs):
        """
        Set(self, int flags, int keyCode, int cmd)

        (Re)set the attributes of a wx.AcceleratorEntry.
        :see `__init__`
        """
        return __core.wxAcceleratorEntry_Set(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """
        GetFlags(self) -> int

        Get the AcceleratorEntry's flags.
        """
        return __core.wxAcceleratorEntry_GetFlags(*args, **kwargs)

    def GetKeyCode(*args, **kwargs):
        """
        GetKeyCode(self) -> int

        Get the AcceleratorEntry's keycode.
        """
        return __core.wxAcceleratorEntry_GetKeyCode(*args, **kwargs)

    def GetCommand(*args, **kwargs):
        """
        GetCommand(self) -> int

        Get the AcceleratorEntry's command ID.
        """
        return __core.wxAcceleratorEntry_GetCommand(*args, **kwargs)


class wxAcceleratorEntryPtr(wxAcceleratorEntry):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxAcceleratorEntry
__core.wxAcceleratorEntry_swigregister(wxAcceleratorEntryPtr)

class wxAcceleratorTable(wxObject):
    """
    An accelerator table allows the application to specify a table of
    keyboard shortcuts for menus or other commands. On Windows, menu or
    button commands are supported; on GTK, only menu commands are
    supported.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxAcceleratorTable instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(entries) -> AcceleratorTable

        Construct an AcceleratorTable from a list of `wx.AcceleratorEntry`
        items or or of 3-tuples (flags, keyCode, cmdID)

        :see: `wx.AcceleratorEntry`
        """
        newobj = __core.new_wxAcceleratorTable(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxAcceleratorTable):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return __core.wxAcceleratorTable_Ok(*args, **kwargs)


class wxAcceleratorTablePtr(wxAcceleratorTable):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxAcceleratorTable
__core.wxAcceleratorTable_swigregister(wxAcceleratorTablePtr)


def wxGetAccelFromString(*args, **kwargs):
    """wxGetAccelFromString(wxString label) -> wxAcceleratorEntry"""
    return __core.wxGetAccelFromString(*args, **kwargs)
#---------------------------------------------------------------------------

class wxVisualAttributes(object):
    """struct containing all the visual attributes of a control"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxVisualAttributes instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> wxVisualAttributes

        struct containing all the visual attributes of a control
        """
        newobj = __core.new_wxVisualAttributes(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=__core.delete_wxVisualAttributes):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    font = property(__core.wxVisualAttributes_font_get, __core.wxVisualAttributes_font_set)
    colFg = property(__core.wxVisualAttributes_colFg_get, __core.wxVisualAttributes_colFg_set)
    colBg = property(__core.wxVisualAttributes_colBg_get, __core.wxVisualAttributes_colBg_set)

class wxVisualAttributesPtr(wxVisualAttributes):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxVisualAttributes
__core.wxVisualAttributes_swigregister(wxVisualAttributesPtr)
wxNullAcceleratorTable = cvar.wxNullAcceleratorTable
PanelNameStr = cvar.PanelNameStr

wxWINDOW_VARIANT_NORMAL = __core.wxWINDOW_VARIANT_NORMAL
wxWINDOW_VARIANT_SMALL = __core.wxWINDOW_VARIANT_SMALL
wxWINDOW_VARIANT_MINI = __core.wxWINDOW_VARIANT_MINI
wxWINDOW_VARIANT_LARGE = __core.wxWINDOW_VARIANT_LARGE
wxWINDOW_VARIANT_MAX = __core.wxWINDOW_VARIANT_MAX
class wxWindow(wxEvtHandler):
    """
    wx.Window is the base class for all windows and represents any visible
    object on the screen. All controls, top level windows and so on are
    wx.Windows. Sizers and device contexts are not however, as they don't
    appear on screen themselves.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxWindow parent, int id=-1, wxPoint pos=wxDefaultPosition, 
            wxSize size=wxDefaultSize, long style=0, 
            wxString name=PanelNameStr) -> wxWindow

        Construct and show a generic Window.
        """
        newobj = __core.new_wxWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, wxWindow parent, int id=-1, wxPoint pos=wxDefaultPosition, 
            wxSize size=wxDefaultSize, long style=0, 
            wxString name=PanelNameStr) -> bool

        Create the GUI part of the Window for 2-phase creation mode.
        """
        return __core.wxWindow_Create(*args, **kwargs)

    def Close(*args, **kwargs):
        """
        Close(self, bool force=False) -> bool

        This function simply generates a EVT_CLOSE event whose handler usually
        tries to close the window. It doesn't close the window itself,
        however.  If force is False (the default) then the window's close
        handler will be allowed to veto the destruction of the window.
        """
        return __core.wxWindow_Close(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """
        Destroy(self) -> bool

        Destroys the window safely.  Frames and dialogs are not destroyed
        immediately when this function is called -- they are added to a list
        of windows to be deleted on idle time, when all the window's events
        have been processed. This prevents problems with events being sent to
        non-existent windows.

        Returns True if the window has either been successfully deleted, or it
        has been added to the list of windows pending real deletion.
        """
        return __core.wxWindow_Destroy(*args, **kwargs)

    def DestroyChildren(*args, **kwargs):
        """
        DestroyChildren(self) -> bool

        Destroys all children of a window. Called automatically by the
        destructor.
        """
        return __core.wxWindow_DestroyChildren(*args, **kwargs)

    def IsBeingDeleted(*args, **kwargs):
        """
        IsBeingDeleted(self) -> bool

        Is the window in the process of being deleted?
        """
        return __core.wxWindow_IsBeingDeleted(*args, **kwargs)

    def SetTitle(*args, **kwargs):
        """
        SetTitle(self, wxString title)

        Sets the window's title. Applicable only to frames and dialogs.
        """
        return __core.wxWindow_SetTitle(*args, **kwargs)

    def GetTitle(*args, **kwargs):
        """
        GetTitle(self) -> wxString

        Gets the window's title. Applicable only to frames and dialogs.
        """
        return __core.wxWindow_GetTitle(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """
        SetLabel(self, wxString label)

        Set the text which the window shows in its label if applicable.
        """
        return __core.wxWindow_SetLabel(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """
        GetLabel(self) -> wxString

        Generic way of getting a label from any window, for identification
        purposes.  The interpretation of this function differs from class to
        class. For frames and dialogs, the value returned is the title. For
        buttons or static text controls, it is the button text. This function
        can be useful for meta-programs such as testing tools or special-needs
        access programs)which need to identify windows by name.
        """
        return __core.wxWindow_GetLabel(*args, **kwargs)

    def SetName(*args, **kwargs):
        """
        SetName(self, wxString name)

        Sets the window's name.  The window name is used for ressource setting
        in X, it is not the same as the window title/label
        """
        return __core.wxWindow_SetName(*args, **kwargs)

    def GetName(*args, **kwargs):
        """
        GetName(self) -> wxString

        Returns the windows name.  This name is not guaranteed to be unique;
        it is up to the programmer to supply an appropriate name in the window
        constructor or via wx.Window.SetName.
        """
        return __core.wxWindow_GetName(*args, **kwargs)

    def SetWindowVariant(*args, **kwargs):
        """
        SetWindowVariant(self, int variant)

        Sets the variant of the window/font size to use for this window, if
        the platform supports variants, for example, wxMac.
        """
        return __core.wxWindow_SetWindowVariant(*args, **kwargs)

    def GetWindowVariant(*args, **kwargs):
        """GetWindowVariant(self) -> int"""
        return __core.wxWindow_GetWindowVariant(*args, **kwargs)

    def SetId(*args, **kwargs):
        """
        SetId(self, int winid)

        Sets the identifier of the window.  Each window has an integer
        identifier. If the application has not provided one, an identifier
        will be generated. Normally, the identifier should be provided on
        creation and should not be modified subsequently.
        """
        return __core.wxWindow_SetId(*args, **kwargs)

    def GetId(*args, **kwargs):
        """
        GetId(self) -> int

        Returns the identifier of the window.  Each window has an integer
        identifier. If the application has not provided one (or the default Id
        -1 is used) then an unique identifier with a negative value will be
        generated.
        """
        return __core.wxWindow_GetId(*args, **kwargs)

    def NewControlId(*args, **kwargs):
        """
        NewControlId() -> int

        Generate a control id for the controls which were not given one.
        """
        return __core.wxWindow_NewControlId(*args, **kwargs)

    NewControlId = staticmethod(NewControlId)
    def NextControlId(*args, **kwargs):
        """
        NextControlId(int winid) -> int

        Get the id of the control following the one with the given
        autogenerated) id
        """
        return __core.wxWindow_NextControlId(*args, **kwargs)

    NextControlId = staticmethod(NextControlId)
    def PrevControlId(*args, **kwargs):
        """
        PrevControlId(int winid) -> int

        Get the id of the control preceding the one with the given
        autogenerated) id
        """
        return __core.wxWindow_PrevControlId(*args, **kwargs)

    PrevControlId = staticmethod(PrevControlId)
    def SetSize(*args, **kwargs):
        """
        SetSize(self, wxSize size)

        Sets the size of the window in pixels.
        """
        return __core.wxWindow_SetSize(*args, **kwargs)

    def SetDimensions(*args, **kwargs):
        """
        SetDimensions(self, int x, int y, int width, int height, int sizeFlags=wxSIZE_AUTO)

        Sets the position and size of the window in pixels.  The sizeFlags
        parameter indicates the interpretation of the other params if they are
        equal to -1.

            ========================  ======================================
            wx.SIZE_AUTO              A -1 indicates that a class-specific
                                      default should be used.
            wx.SIZE_USE_EXISTING      Axisting dimensions should be used if
                                      -1 values are supplied.
            wxSIZE_ALLOW_MINUS_ONE    Allow dimensions of -1 and less to be
                                      interpreted as real dimensions, not
                                      default values.
            ========================  ======================================

        """
        return __core.wxWindow_SetDimensions(*args, **kwargs)

    def SetRect(*args, **kwargs):
        """
        SetRect(self, wxRect rect, int sizeFlags=wxSIZE_AUTO)

        Sets the position and size of the window in pixels using a wx.Rect.
        """
        return __core.wxWindow_SetRect(*args, **kwargs)

    def SetSizeWH(*args, **kwargs):
        """
        SetSizeWH(self, int width, int height)

        Sets the size of the window in pixels.
        """
        return __core.wxWindow_SetSizeWH(*args, **kwargs)

    def Move(*args, **kwargs):
        """
        Move(self, wxPoint pt, int flags=wxSIZE_USE_EXISTING)

        Moves the window to the given position.
        """
        return __core.wxWindow_Move(*args, **kwargs)

    SetPosition = Move 
    def MoveXY(*args, **kwargs):
        """
        MoveXY(self, int x, int y, int flags=wxSIZE_USE_EXISTING)

        Moves the window to the given position.
        """
        return __core.wxWindow_MoveXY(*args, **kwargs)

    def SetBestFittingSize(*args, **kwargs):
        """
        SetBestFittingSize(self, wxSize size=wxDefaultSize)

        A 'Smart' SetSize that will fill in default size components with the
        window's *best size* values.  Also set's the minsize for use with sizers.
        """
        return __core.wxWindow_SetBestFittingSize(*args, **kwargs)

    def Raise(*args, **kwargs):
        """
        Raise(self)

        Raises the window to the top of the window hierarchy if it is a
        managed window (dialog or frame).
        """
        return __core.wxWindow_Raise(*args, **kwargs)

    def Lower(*args, **kwargs):
        """
        Lower(self)

        Lowers the window to the bottom of the window hierarchy if it is a
        managed window (dialog or frame).
        """
        return __core.wxWindow_Lower(*args, **kwargs)

    def SetClientSize(*args, **kwargs):
        """
        SetClientSize(self, wxSize size)

        This sets the size of the window client area in pixels. Using this
        function to size a window tends to be more device-independent than
        wx.Window.SetSize, since the application need not worry about what
        dimensions the border or title bar have when trying to fit the window
        around panel items, for example.
        """
        return __core.wxWindow_SetClientSize(*args, **kwargs)

    def SetClientSizeWH(*args, **kwargs):
        """
        SetClientSizeWH(self, int width, int height)

        This sets the size of the window client area in pixels. Using this
        function to size a window tends to be more device-independent than
        wx.Window.SetSize, since the application need not worry about what
        dimensions the border or title bar have when trying to fit the window
        around panel items, for example.
        """
        return __core.wxWindow_SetClientSizeWH(*args, **kwargs)

    def SetClientRect(*args, **kwargs):
        """
        SetClientRect(self, wxRect rect)

        This sets the size of the window client area in pixels. Using this
        function to size a window tends to be more device-independent than
        wx.Window.SetSize, since the application need not worry about what
        dimensions the border or title bar have when trying to fit the window
        around panel items, for example.
        """
        return __core.wxWindow_SetClientRect(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> wxPoint

        Get the window's position.
        """
        return __core.wxWindow_GetPosition(*args, **kwargs)

    def GetPositionTuple(*args, **kwargs):
        """
        GetPositionTuple() -> (x,y)

        Get the window's position.
        """
        return __core.wxWindow_GetPositionTuple(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> wxSize

        Get the window size.
        """
        return __core.wxWindow_GetSize(*args, **kwargs)

    def GetSizeTuple(*args, **kwargs):
        """
        GetSizeTuple() -> (width, height)

        Get the window size.
        """
        return __core.wxWindow_GetSizeTuple(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """
        GetRect(self) -> wxRect

        Returns the size and position of the window as a wx.Rect object.
        """
        return __core.wxWindow_GetRect(*args, **kwargs)

    def GetClientSize(*args, **kwargs):
        """
        GetClientSize(self) -> wxSize

        This gets the size of the window's 'client area' in pixels. The client
        area is the area which may be drawn on by the programmer, excluding
        title bar, border, scrollbars, etc.
        """
        return __core.wxWindow_GetClientSize(*args, **kwargs)

    def GetClientSizeTuple(*args, **kwargs):
        """
        GetClientSizeTuple() -> (width, height)

        This gets the size of the window's 'client area' in pixels. The client
        area is the area which may be drawn on by the programmer, excluding
        title bar, border, scrollbars, etc.
        """
        return __core.wxWindow_GetClientSizeTuple(*args, **kwargs)

    def GetClientAreaOrigin(*args, **kwargs):
        """
        GetClientAreaOrigin(self) -> wxPoint

        Get the origin of the client area of the window relative to the
        window's top left corner (the client area may be shifted because of
        the borders, scrollbars, other decorations...)
        """
        return __core.wxWindow_GetClientAreaOrigin(*args, **kwargs)

    def GetClientRect(*args, **kwargs):
        """
        GetClientRect(self) -> wxRect

        Get the client area position and size as a `wx.Rect` object.
        """
        return __core.wxWindow_GetClientRect(*args, **kwargs)

    def GetBestSize(*args, **kwargs):
        """
        GetBestSize(self) -> wxSize

        This function returns the best acceptable minimal size for the
        window, if applicable. For example, for a static text control, it will
        be the minimal size such that the control label is not truncated. For
        windows containing subwindows (suzh aswx.Panel), the size returned by
        this function will be the same as the size the window would have had
        after calling Fit.
        """
        return __core.wxWindow_GetBestSize(*args, **kwargs)

    def GetBestSizeTuple(*args, **kwargs):
        """
        GetBestSizeTuple() -> (width, height)

        This function returns the best acceptable minimal size for the
        window, if applicable. For example, for a static text control, it will
        be the minimal size such that the control label is not truncated. For
        windows containing subwindows (suzh aswx.Panel), the size returned by
        this function will be the same as the size the window would have had
        after calling Fit.
        """
        return __core.wxWindow_GetBestSizeTuple(*args, **kwargs)

    def InvalidateBestSize(*args, **kwargs):
        """
        InvalidateBestSize(self)

        Reset the cached best size value so it will be recalculated the next
        time it is needed.
        """
        return __core.wxWindow_InvalidateBestSize(*args, **kwargs)

    def GetBestFittingSize(*args, **kwargs):
        """
        GetBestFittingSize(self) -> wxSize

        This function will merge the window's best size into the window's
        minimum size, giving priority to the min size components, and returns
        the results.

        """
        return __core.wxWindow_GetBestFittingSize(*args, **kwargs)

    def GetAdjustedBestSize(*args, **kwargs):
        """
        GetAdjustedBestSize(self) -> wxSize

        This method is similar to GetBestSize, except in one
        thing. GetBestSize should return the minimum untruncated size of the
        window, while this method will return the largest of BestSize and any
        user specified minimum size. ie. it is the minimum size the window
        should currently be drawn at, not the minimal size it can possibly
        tolerate.
        """
        return __core.wxWindow_GetAdjustedBestSize(*args, **kwargs)

    def Center(*args, **kwargs):
        """
        Center(self, int direction=wxBOTH)

        Centers the window.  The parameter specifies the direction for
        cetering, and may be wx.HORIZONTAL, wx.VERTICAL or wx.BOTH. It may
        also include wx.CENTER_ON_SCREEN flag if you want to center the window
        on the entire screen and not on its parent window.  If it is a
        top-level window and has no parent then it will always be centered
        relative to the screen.
        """
        return __core.wxWindow_Center(*args, **kwargs)

    Centre = Center 
    def CenterOnScreen(*args, **kwargs):
        """
        CenterOnScreen(self, int dir=wxBOTH)

        Center on screen (only works for top level windows)
        """
        return __core.wxWindow_CenterOnScreen(*args, **kwargs)

    CentreOnScreen = CenterOnScreen 
    def CenterOnParent(*args, **kwargs):
        """
        CenterOnParent(self, int dir=wxBOTH)

        Center with respect to the the parent window
        """
        return __core.wxWindow_CenterOnParent(*args, **kwargs)

    CentreOnParent = CenterOnParent 
    def Fit(*args, **kwargs):
        """
        Fit(self)

        Sizes the window so that it fits around its subwindows. This function
        won't do anything if there are no subwindows and will only really work
        correctly if sizers are used for the subwindows layout. Also, if the
        window has exactly one subwindow it is better (faster and the result
        is more precise as Fit adds some margin to account for fuzziness of
        its calculations) to call window.SetClientSize(child.GetSize())
        instead of calling Fit.
        """
        return __core.wxWindow_Fit(*args, **kwargs)

    def FitInside(*args, **kwargs):
        """
        FitInside(self)

        Similar to Fit, but sizes the interior (virtual) size of a
        window. Mainly useful with scrolled windows to reset scrollbars after
        sizing changes that do not trigger a size event, and/or scrolled
        windows without an interior sizer. This function similarly won't do
        anything if there are no subwindows.
        """
        return __core.wxWindow_FitInside(*args, **kwargs)

    def SetSizeHints(*args, **kwargs):
        """
        SetSizeHints(self, int minW, int minH, int maxW=-1, int maxH=-1, int incW=-1, 
            int incH=-1)

        Allows specification of minimum and maximum window sizes, and window
        size increments. If a pair of values is not set (or set to -1), the
        default values will be used.  If this function is called, the user
        will not be able to size the window outside the given bounds (if it is
        a top-level window.)  Sizers will also inspect the minimum window size
        and will use that value if set when calculating layout.

        The resizing increments are only significant under Motif or Xt.
        """
        return __core.wxWindow_SetSizeHints(*args, **kwargs)

    def SetSizeHintsSz(*args, **kwargs):
        """
        SetSizeHintsSz(self, wxSize minSize, wxSize maxSize=wxDefaultSize, wxSize incSize=wxDefaultSize)

        Allows specification of minimum and maximum window sizes, and window
        size increments. If a pair of values is not set (or set to -1), the
        default values will be used.  If this function is called, the user
        will not be able to size the window outside the given bounds (if it is
        a top-level window.)  Sizers will also inspect the minimum window size
        and will use that value if set when calculating layout.

        The resizing increments are only significant under Motif or Xt.
        """
        return __core.wxWindow_SetSizeHintsSz(*args, **kwargs)

    def SetVirtualSizeHints(*args, **kwargs):
        """
        SetVirtualSizeHints(self, int minW, int minH, int maxW=-1, int maxH=-1)

        Allows specification of minimum and maximum virtual window sizes. If a
        pair of values is not set (or set to -1), the default values will be
        used.  If this function is called, the user will not be able to size
        the virtual area of the window outside the given bounds.
        """
        return __core.wxWindow_SetVirtualSizeHints(*args, **kwargs)

    def SetVirtualSizeHintsSz(*args, **kwargs):
        """
        SetVirtualSizeHintsSz(self, wxSize minSize, wxSize maxSize=wxDefaultSize)

        Allows specification of minimum and maximum virtual window sizes. If a
        pair of values is not set (or set to -1), the default values will be
        used.  If this function is called, the user will not be able to size
        the virtual area of the window outside the given bounds.
        """
        return __core.wxWindow_SetVirtualSizeHintsSz(*args, **kwargs)

    def GetMaxSize(*args, **kwargs):
        """GetMaxSize(self) -> wxSize"""
        return __core.wxWindow_GetMaxSize(*args, **kwargs)

    def GetMinSize(*args, **kwargs):
        """GetMinSize(self) -> wxSize"""
        return __core.wxWindow_GetMinSize(*args, **kwargs)

    def SetMinSize(*args, **kwargs):
        """
        SetMinSize(self, wxSize minSize)

        A more convenient method than `SetSizeHints` for setting just the
        min size.
        """
        return __core.wxWindow_SetMinSize(*args, **kwargs)

    def SetMaxSize(*args, **kwargs):
        """
        SetMaxSize(self, wxSize maxSize)

        A more convenient method than `SetSizeHints` for setting just the
        max size.
        """
        return __core.wxWindow_SetMaxSize(*args, **kwargs)

    def GetMinWidth(*args, **kwargs):
        """GetMinWidth(self) -> int"""
        return __core.wxWindow_GetMinWidth(*args, **kwargs)

    def GetMinHeight(*args, **kwargs):
        """GetMinHeight(self) -> int"""
        return __core.wxWindow_GetMinHeight(*args, **kwargs)

    def GetMaxWidth(*args, **kwargs):
        """GetMaxWidth(self) -> int"""
        return __core.wxWindow_GetMaxWidth(*args, **kwargs)

    def GetMaxHeight(*args, **kwargs):
        """GetMaxHeight(self) -> int"""
        return __core.wxWindow_GetMaxHeight(*args, **kwargs)

    def SetVirtualSize(*args, **kwargs):
        """
        SetVirtualSize(self, wxSize size)

        Set the the virtual size of a window in pixels.  For most windows this
        is just the client area of the window, but for some like scrolled
        windows it is more or less independent of the screen window size.
        """
        return __core.wxWindow_SetVirtualSize(*args, **kwargs)

    def SetVirtualSizeWH(*args, **kwargs):
        """
        SetVirtualSizeWH(self, int w, int h)

        Set the the virtual size of a window in pixels.  For most windows this
        is just the client area of the window, but for some like scrolled
        windows it is more or less independent of the screen window size.
        """
        return __core.wxWindow_SetVirtualSizeWH(*args, **kwargs)

    def GetVirtualSize(*args, **kwargs):
        """
        GetVirtualSize(self) -> wxSize

        Get the the virtual size of the window in pixels.  For most windows
        this is just the client area of the window, but for some like scrolled
        windows it is more or less independent of the screen window size.
        """
        return __core.wxWindow_GetVirtualSize(*args, **kwargs)

    def GetVirtualSizeTuple(*args, **kwargs):
        """
        GetVirtualSizeTuple() -> (width, height)

        Get the the virtual size of the window in pixels.  For most windows
        this is just the client area of the window, but for some like scrolled
        windows it is more or less independent of the screen window size.
        """
        return __core.wxWindow_GetVirtualSizeTuple(*args, **kwargs)

    def GetBestVirtualSize(*args, **kwargs):
        """
        GetBestVirtualSize(self) -> wxSize

        Return the largest of ClientSize and BestSize (as determined by a
        sizer, interior children, or other means)
        """
        return __core.wxWindow_GetBestVirtualSize(*args, **kwargs)

    def Show(*args, **kwargs):
        """
        Show(self, bool show=True) -> bool

        Shows or hides the window. You may need to call Raise for a top level
        window if you want to bring it to top, although this is not needed if
        Show is called immediately after the frame creation.  Returns True if
        the window has been shown or hidden or False if nothing was done
        because it already was in the requested state.
        """
        return __core.wxWindow_Show(*args, **kwargs)

    def Hide(*args, **kwargs):
        """
        Hide(self) -> bool

        Equivalent to calling Show(False).
        """
        return __core.wxWindow_Hide(*args, **kwargs)

    def Enable(*args, **kwargs):
        """
        Enable(self, bool enable=True) -> bool

        Enable or disable the window for user input. Note that when a parent
        window is disabled, all of its children are disabled as well and they
        are reenabled again when the parent is.  Returns true if the window
        has been enabled or disabled, false if nothing was done, i.e. if the
        window had already been in the specified state.
        """
        return __core.wxWindow_Enable(*args, **kwargs)

    def Disable(*args, **kwargs):
        """
        Disable(self) -> bool

        Disables the window, same as Enable(false).
        """
        return __core.wxWindow_Disable(*args, **kwargs)

    def IsShown(*args, **kwargs):
        """
        IsShown(self) -> bool

        Returns true if the window is shown, false if it has been hidden.
        """
        return __core.wxWindow_IsShown(*args, **kwargs)

    def IsEnabled(*args, **kwargs):
        """
        IsEnabled(self) -> bool

        Returns true if the window is enabled for input, false otherwise.
        """
        return __core.wxWindow_IsEnabled(*args, **kwargs)

    def SetWindowStyleFlag(*args, **kwargs):
        """
        SetWindowStyleFlag(self, long style)

        Sets the style of the window. Please note that some styles cannot be
        changed after the window creation and that Refresh() might need to be
        called after changing the others for the change to take place
        immediately.
        """
        return __core.wxWindow_SetWindowStyleFlag(*args, **kwargs)

    def GetWindowStyleFlag(*args, **kwargs):
        """
        GetWindowStyleFlag(self) -> long

        Gets the window style that was passed to the constructor or Create
        method.
        """
        return __core.wxWindow_GetWindowStyleFlag(*args, **kwargs)

    SetWindowStyle = SetWindowStyleFlag; GetWindowStyle = GetWindowStyleFlag 
    def HasFlag(*args, **kwargs):
        """
        HasFlag(self, int flag) -> bool

        Test if the given style is set for this window.
        """
        return __core.wxWindow_HasFlag(*args, **kwargs)

    def IsRetained(*args, **kwargs):
        """
        IsRetained(self) -> bool

        Returns true if the window is retained, false otherwise.  Retained
        windows are only available on X platforms.
        """
        return __core.wxWindow_IsRetained(*args, **kwargs)

    def SetExtraStyle(*args, **kwargs):
        """
        SetExtraStyle(self, long exStyle)

        Sets the extra style bits for the window.  Extra styles are the less
        often used style bits which can't be set with the constructor or with
        SetWindowStyleFlag()
        """
        return __core.wxWindow_SetExtraStyle(*args, **kwargs)

    def GetExtraStyle(*args, **kwargs):
        """
        GetExtraStyle(self) -> long

        Returns the extra style bits for the window.
        """
        return __core.wxWindow_GetExtraStyle(*args, **kwargs)

    def MakeModal(*args, **kwargs):
        """
        MakeModal(self, bool modal=True)

        Disables all other windows in the application so that the user can
        only interact with this window.  Passing False will reverse this
        effect.
        """
        return __core.wxWindow_MakeModal(*args, **kwargs)

    def SetThemeEnabled(*args, **kwargs):
        """
        SetThemeEnabled(self, bool enableTheme)

        This function tells a window if it should use the system's "theme"
         code to draw the windows' background instead if its own background
         drawing code. This will only have an effect on platforms that support
         the notion of themes in user defined windows. One such platform is
         GTK+ where windows can have (very colourful) backgrounds defined by a
         user's selected theme.

        Dialogs, notebook pages and the status bar have this flag set to true
        by default so that the default look and feel is simulated best.
        """
        return __core.wxWindow_SetThemeEnabled(*args, **kwargs)

    def GetThemeEnabled(*args, **kwargs):
        """
        GetThemeEnabled(self) -> bool

        Return the themeEnabled flag.
        """
        return __core.wxWindow_GetThemeEnabled(*args, **kwargs)

    def SetFocus(*args, **kwargs):
        """
        SetFocus(self)

        Set's the focus to this window, allowing it to receive keyboard input.
        """
        return __core.wxWindow_SetFocus(*args, **kwargs)

    def SetFocusFromKbd(*args, **kwargs):
        """
        SetFocusFromKbd(self)

        Set focus to this window as the result of a keyboard action.  Normally
        only called internally.
        """
        return __core.wxWindow_SetFocusFromKbd(*args, **kwargs)

    def FindFocus(*args, **kwargs):
        """
        FindFocus() -> wxWindow

        Returns the window or control that currently has the keyboard focus,
        or None.
        """
        return __core.wxWindow_FindFocus(*args, **kwargs)

    FindFocus = staticmethod(FindFocus)
    def AcceptsFocus(*args, **kwargs):
        """
        AcceptsFocus(self) -> bool

        Can this window have focus?
        """
        return __core.wxWindow_AcceptsFocus(*args, **kwargs)

    def AcceptsFocusFromKeyboard(*args, **kwargs):
        """
        AcceptsFocusFromKeyboard(self) -> bool

        Can this window be given focus by keyboard navigation? if not, the
        only way to give it focus (provided it accepts it at all) is to click
        it.
        """
        return __core.wxWindow_AcceptsFocusFromKeyboard(*args, **kwargs)

    def GetDefaultItem(*args, **kwargs):
        """
        GetDefaultItem(self) -> wxWindow

        Get the default child of this parent, i.e. the one which is activated
        by pressing <Enter> such as the OK button on a wx.Dialog.
        """
        return __core.wxWindow_GetDefaultItem(*args, **kwargs)

    def SetDefaultItem(*args, **kwargs):
        """
        SetDefaultItem(self, wxWindow child) -> wxWindow

        Set this child as default, return the old default.
        """
        return __core.wxWindow_SetDefaultItem(*args, **kwargs)

    def SetTmpDefaultItem(*args, **kwargs):
        """
        SetTmpDefaultItem(self, wxWindow win)

        Set this child as temporary default
        """
        return __core.wxWindow_SetTmpDefaultItem(*args, **kwargs)

    def Navigate(*args, **kwargs):
        """
        Navigate(self, int flags=NavigationKeyEvent.IsForward) -> bool

        Does keyboard navigation from this window to another, by sending a
        `wx.NavigationKeyEvent`.
        """
        return __core.wxWindow_Navigate(*args, **kwargs)

    def MoveAfterInTabOrder(*args, **kwargs):
        """
        MoveAfterInTabOrder(self, wxWindow win)

        Moves this window in the tab navigation order after the specified
        sibling window.  This means that when the user presses the TAB key on
        that other window, the focus switches to this window.

        The default tab order is the same as creation order.  This function
        and `MoveBeforeInTabOrder` allow to change it after creating all the
        windows.

        """
        return __core.wxWindow_MoveAfterInTabOrder(*args, **kwargs)

    def MoveBeforeInTabOrder(*args, **kwargs):
        """
        MoveBeforeInTabOrder(self, wxWindow win)

        Same as `MoveAfterInTabOrder` except that it inserts this window just
        before win instead of putting it right after it.
        """
        return __core.wxWindow_MoveBeforeInTabOrder(*args, **kwargs)

    def GetChildren(*args, **kwargs):
        """
        GetChildren(self) -> PyObject

        Returns a list of the window's children.  NOTE: Currently this is a
        copy of the child window list maintained by the window, so the return
        value of this function is only valid as long as the window's children
        do not change.
        """
        return __core.wxWindow_GetChildren(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """
        GetParent(self) -> wxWindow

        Returns the parent window of this window, or None if there isn't one.
        """
        return __core.wxWindow_GetParent(*args, **kwargs)

    def GetGrandParent(*args, **kwargs):
        """
        GetGrandParent(self) -> wxWindow

        Returns the parent of the parent of this window, or None if there
        isn't one.
        """
        return __core.wxWindow_GetGrandParent(*args, **kwargs)

    def IsTopLevel(*args, **kwargs):
        """
        IsTopLevel(self) -> bool

        Returns true if the given window is a top-level one. Currently all
        frames and dialogs are always considered to be top-level windows (even
        if they have a parent window).
        """
        return __core.wxWindow_IsTopLevel(*args, **kwargs)

    def Reparent(*args, **kwargs):
        """
        Reparent(self, wxWindow newParent) -> bool

        Reparents the window, i.e the window will be removed from its current
        parent window (e.g. a non-standard toolbar in a wxFrame) and then
        re-inserted into another. Available on Windows and GTK.  Returns True
        if the parent was changed, False otherwise (error or newParent ==
        oldParent)
        """
        return __core.wxWindow_Reparent(*args, **kwargs)

    def AddChild(*args, **kwargs):
        """
        AddChild(self, wxWindow child)

        Adds a child window. This is called automatically by window creation
        functions so should not be required by the application programmer.
        """
        return __core.wxWindow_AddChild(*args, **kwargs)

    def RemoveChild(*args, **kwargs):
        """
        RemoveChild(self, wxWindow child)

        Removes a child window. This is called automatically by window
        deletion functions so should not be required by the application
        programmer.
        """
        return __core.wxWindow_RemoveChild(*args, **kwargs)

    def FindWindowById(*args, **kwargs):
        """
        FindWindowById(self, long winid) -> wxWindow

        Find a chld of this window by window ID
        """
        return __core.wxWindow_FindWindowById(*args, **kwargs)

    def FindWindowByName(*args, **kwargs):
        """
        FindWindowByName(self, wxString name) -> wxWindow

        Find a child of this window by name
        """
        return __core.wxWindow_FindWindowByName(*args, **kwargs)

    def GetEventHandler(*args, **kwargs):
        """
        GetEventHandler(self) -> wxEvtHandler

        Returns the event handler for this window. By default, the window is
        its own event handler.
        """
        return __core.wxWindow_GetEventHandler(*args, **kwargs)

    def SetEventHandler(*args, **kwargs):
        """
        SetEventHandler(self, wxEvtHandler handler)

        Sets the event handler for this window.  An event handler is an object
        that is capable of processing the events sent to a window. By default,
        the window is its own event handler, but an application may wish to
        substitute another, for example to allow central implementation of
        event-handling for a variety of different window classes.

        It is usually better to use `wx.Window.PushEventHandler` since this sets
        up a chain of event handlers, where an event not handled by one event
        handler is handed to the next one in the chain.
        """
        return __core.wxWindow_SetEventHandler(*args, **kwargs)

    def PushEventHandler(*args, **kwargs):
        """
        PushEventHandler(self, wxEvtHandler handler)

        Pushes this event handler onto the event handler stack for the window.
        An event handler is an object that is capable of processing the events
        sent to a window. By default, the window is its own event handler, but
        an application may wish to substitute another, for example to allow
        central implementation of event-handling for a variety of different
        window classes.

        wx.Window.PushEventHandler allows an application to set up a chain of
        event handlers, where an event not handled by one event handler is
        handed to the next one in the chain. Use `wx.Window.PopEventHandler` to
        remove the event handler.
        """
        return __core.wxWindow_PushEventHandler(*args, **kwargs)

    def PopEventHandler(*args, **kwargs):
        """
        PopEventHandler(self, bool deleteHandler=False) -> wxEvtHandler

        Removes and returns the top-most event handler on the event handler
        stack.  If deleteHandler is True then the wx.EvtHandler object will be
        destroyed after it is popped.
        """
        return __core.wxWindow_PopEventHandler(*args, **kwargs)

    def RemoveEventHandler(*args, **kwargs):
        """
        RemoveEventHandler(self, wxEvtHandler handler) -> bool

        Find the given handler in the event handler chain and remove (but not
        delete) it from the event handler chain, return True if it was found
        and False otherwise (this also results in an assert failure so this
        function should only be called when the handler is supposed to be
        there.)
        """
        return __core.wxWindow_RemoveEventHandler(*args, **kwargs)

    def SetValidator(*args, **kwargs):
        """
        SetValidator(self, wxValidator validator)

        Deletes the current validator (if any) and sets the window validator,
        having called wx.Validator.Clone to create a new validator of this
        type.
        """
        return __core.wxWindow_SetValidator(*args, **kwargs)

    def GetValidator(*args, **kwargs):
        """
        GetValidator(self) -> wxValidator

        Returns a pointer to the current validator for the window, or None if
        there is none.
        """
        return __core.wxWindow_GetValidator(*args, **kwargs)

    def Validate(*args, **kwargs):
        """
        Validate(self) -> bool

        Validates the current values of the child controls using their
        validators.  If the window has wx.WS_EX_VALIDATE_RECURSIVELY extra
        style flag set, the method will also call Validate() of all child
        windows.  Returns false if any of the validations failed.
        """
        return __core.wxWindow_Validate(*args, **kwargs)

    def TransferDataToWindow(*args, **kwargs):
        """
        TransferDataToWindow(self) -> bool

        Transfers values to child controls from data areas specified by their
        validators.  If the window has wx.WS_EX_VALIDATE_RECURSIVELY extra
        style flag set, the method will also call TransferDataToWindow() of
        all child windows.
        """
        return __core.wxWindow_TransferDataToWindow(*args, **kwargs)

    def TransferDataFromWindow(*args, **kwargs):
        """
        TransferDataFromWindow(self) -> bool

        Transfers values from child controls to data areas specified by their
        validators. Returns false if a transfer failed.  If the window has
        wx.WS_EX_VALIDATE_RECURSIVELY extra style flag set, the method will
        also call TransferDataFromWindow() of all child windows.
        """
        return __core.wxWindow_TransferDataFromWindow(*args, **kwargs)

    def InitDialog(*args, **kwargs):
        """
        InitDialog(self)

        Sends an EVT_INIT_DIALOG event, whose handler usually transfers data
        to the dialog via validators.
        """
        return __core.wxWindow_InitDialog(*args, **kwargs)

    def SetAcceleratorTable(*args, **kwargs):
        """
        SetAcceleratorTable(self, wxAcceleratorTable accel)

        Sets the accelerator table for this window.
        """
        return __core.wxWindow_SetAcceleratorTable(*args, **kwargs)

    def GetAcceleratorTable(*args, **kwargs):
        """
        GetAcceleratorTable(self) -> wxAcceleratorTable

        Gets the accelerator table for this window.
        """
        return __core.wxWindow_GetAcceleratorTable(*args, **kwargs)

    def RegisterHotKey(*args, **kwargs):
        """
        RegisterHotKey(self, int hotkeyId, int modifiers, int keycode) -> bool

        Registers a system wide hotkey. Every time the user presses the hotkey
        registered here, this window will receive a hotkey event. It will
        receive the event even if the application is in the background and
        does not have the input focus because the user is working with some
        other application.  To bind an event handler function to this hotkey
        use EVT_HOTKEY with an id equal to hotkeyId.  Returns True if the
        hotkey was registered successfully.
        """
        return __core.wxWindow_RegisterHotKey(*args, **kwargs)

    def UnregisterHotKey(*args, **kwargs):
        """
        UnregisterHotKey(self, int hotkeyId) -> bool

        Unregisters a system wide hotkey.
        """
        return __core.wxWindow_UnregisterHotKey(*args, **kwargs)

    def ConvertDialogPointToPixels(*args, **kwargs):
        """
        ConvertDialogPointToPixels(self, wxPoint pt) -> wxPoint

        Converts a point or size from dialog units to pixels.  Dialog units
        are used for maintaining a dialog's proportions even if the font
        changes. For the x dimension, the dialog units are multiplied by the
        average character width and then divided by 4. For the y dimension,
        the dialog units are multiplied by the average character height and
        then divided by 8.
        """
        return __core.wxWindow_ConvertDialogPointToPixels(*args, **kwargs)

    def ConvertDialogSizeToPixels(*args, **kwargs):
        """
        ConvertDialogSizeToPixels(self, wxSize sz) -> wxSize

        Converts a point or size from dialog units to pixels.  Dialog units
        are used for maintaining a dialog's proportions even if the font
        changes. For the x dimension, the dialog units are multiplied by the
        average character width and then divided by 4. For the y dimension,
        the dialog units are multiplied by the average character height and
        then divided by 8.
        """
        return __core.wxWindow_ConvertDialogSizeToPixels(*args, **kwargs)

    def DLG_PNT(*args, **kwargs):
        """
        DLG_PNT(self, wxPoint pt) -> wxPoint

        Converts a point or size from dialog units to pixels.  Dialog units
        are used for maintaining a dialog's proportions even if the font
        changes. For the x dimension, the dialog units are multiplied by the
        average character width and then divided by 4. For the y dimension,
        the dialog units are multiplied by the average character height and
        then divided by 8.
        """
        return __core.wxWindow_DLG_PNT(*args, **kwargs)

    def DLG_SZE(*args, **kwargs):
        """
        DLG_SZE(self, wxSize sz) -> wxSize

        Converts a point or size from dialog units to pixels.  Dialog units
        are used for maintaining a dialog's proportions even if the font
        changes. For the x dimension, the dialog units are multiplied by the
        average character width and then divided by 4. For the y dimension,
        the dialog units are multiplied by the average character height and
        then divided by 8.
        """
        return __core.wxWindow_DLG_SZE(*args, **kwargs)

    def ConvertPixelPointToDialog(*args, **kwargs):
        """ConvertPixelPointToDialog(self, wxPoint pt) -> wxPoint"""
        return __core.wxWindow_ConvertPixelPointToDialog(*args, **kwargs)

    def ConvertPixelSizeToDialog(*args, **kwargs):
        """ConvertPixelSizeToDialog(self, wxSize sz) -> wxSize"""
        return __core.wxWindow_ConvertPixelSizeToDialog(*args, **kwargs)

    def WarpPointer(*args, **kwargs):
        """
        WarpPointer(self, int x, int y)

        Moves the pointer to the given position on the window.

        NOTE: This function is not supported under Mac because Apple Human
        Interface Guidelines forbid moving the mouse cursor programmatically.
        """
        return __core.wxWindow_WarpPointer(*args, **kwargs)

    def CaptureMouse(*args, **kwargs):
        """
        CaptureMouse(self)

        Directs all mouse input to this window. Call wx.Window.ReleaseMouse to
        release the capture.

        Note that wxWindows maintains the stack of windows having captured the
        mouse and when the mouse is released the capture returns to the window
        which had had captured it previously and it is only really released if
        there were no previous window. In particular, this means that you must
        release the mouse as many times as you capture it.
        """
        return __core.wxWindow_CaptureMouse(*args, **kwargs)

    def ReleaseMouse(*args, **kwargs):
        """
        ReleaseMouse(self)

        Releases mouse input captured with wx.Window.CaptureMouse.
        """
        return __core.wxWindow_ReleaseMouse(*args, **kwargs)

    def GetCapture(*args, **kwargs):
        """
        GetCapture() -> wxWindow

        Returns the window which currently captures the mouse or None
        """
        return __core.wxWindow_GetCapture(*args, **kwargs)

    GetCapture = staticmethod(GetCapture)
    def HasCapture(*args, **kwargs):
        """
        HasCapture(self) -> bool

        Returns true if this window has the current mouse capture.
        """
        return __core.wxWindow_HasCapture(*args, **kwargs)

    def Refresh(*args, **kwargs):
        """
        Refresh(self, bool eraseBackground=True, wxRect rect=None)

        Mark the specified rectangle (or the whole window) as "dirty" so it
        will be repainted.  Causes an EVT_PAINT event to be generated and sent
        to the window.
        """
        return __core.wxWindow_Refresh(*args, **kwargs)

    def RefreshRect(*args, **kwargs):
        """
        RefreshRect(self, wxRect rect, bool eraseBackground=True)

        Redraws the contents of the given rectangle: the area inside it will
        be repainted.  This is the same as Refresh but has a nicer syntax.
        """
        return __core.wxWindow_RefreshRect(*args, **kwargs)

    def Update(*args, **kwargs):
        """
        Update(self)

        Calling this method immediately repaints the invalidated area of the
        window instead of waiting for the EVT_PAINT event to happen, (normally
        this would usually only happen when the flow of control returns to the
        event loop.)  Notice that this function doesn't refresh the window and
        does nothing if the window has been already repainted.  Use Refresh
        first if you want to immediately redraw the window (or some portion of
        it) unconditionally.
        """
        return __core.wxWindow_Update(*args, **kwargs)

    def ClearBackground(*args, **kwargs):
        """
        ClearBackground(self)

        Clears the window by filling it with the current background
        colour. Does not cause an erase background event to be generated.
        """
        return __core.wxWindow_ClearBackground(*args, **kwargs)

    def Freeze(*args, **kwargs):
        """
        Freeze(self)

        Freezes the window or, in other words, prevents any updates from
        taking place on screen, the window is not redrawn at all. Thaw must be
        called to reenable window redrawing.  Calls to Freeze/Thaw may be
        nested, with the actual Thaw being delayed until all the nesting has
        been undone.

        This method is useful for visual appearance optimization (for example,
        it is a good idea to use it before inserting large amount of text into
        a wxTextCtrl under wxGTK) but is not implemented on all platforms nor
        for all controls so it is mostly just a hint to wxWindows and not a
        mandatory directive.
        """
        return __core.wxWindow_Freeze(*args, **kwargs)

    def Thaw(*args, **kwargs):
        """
        Thaw(self)

        Reenables window updating after a previous call to Freeze.  Calls to
        Freeze/Thaw may be nested, so Thaw must be called the same number of
        times that Freeze was before the window will be updated.
        """
        return __core.wxWindow_Thaw(*args, **kwargs)

    def PrepareDC(*args, **kwargs):
        """
        PrepareDC(self, wxDC dc)

        Call this function to prepare the device context for drawing a
        scrolled image. It sets the device origin according to the current
        scroll position.
        """
        return __core.wxWindow_PrepareDC(*args, **kwargs)

    def GetUpdateRegion(*args, **kwargs):
        """
        GetUpdateRegion(self) -> wxRegion

        Returns the region specifying which parts of the window have been
        damaged. Should only be called within an EVT_PAINT handler.
        """
        return __core.wxWindow_GetUpdateRegion(*args, **kwargs)

    def GetUpdateClientRect(*args, **kwargs):
        """
        GetUpdateClientRect(self) -> wxRect

        Get the update rectangle region bounding box in client coords.
        """
        return __core.wxWindow_GetUpdateClientRect(*args, **kwargs)

    def IsExposed(*args, **kwargs):
        """
        IsExposed(self, int x, int y, int w=1, int h=1) -> bool

        Returns true if the given point or rectangle area has been exposed
        since the last repaint. Call this in an paint event handler to
        optimize redrawing by only redrawing those areas, which have been
        exposed.
        """
        return __core.wxWindow_IsExposed(*args, **kwargs)

    def IsExposedPoint(*args, **kwargs):
        """
        IsExposedPoint(self, wxPoint pt) -> bool

        Returns true if the given point or rectangle area has been exposed
        since the last repaint. Call this in an paint event handler to
        optimize redrawing by only redrawing those areas, which have been
        exposed.
        """
        return __core.wxWindow_IsExposedPoint(*args, **kwargs)

    def IsExposedRect(*args, **kwargs):
        """
        IsExposedRect(self, wxRect rect) -> bool

        Returns true if the given point or rectangle area has been exposed
        since the last repaint. Call this in an paint event handler to
        optimize redrawing by only redrawing those areas, which have been
        exposed.
        """
        return __core.wxWindow_IsExposedRect(*args, **kwargs)

    def GetDefaultAttributes(*args, **kwargs):
        """
        GetDefaultAttributes(self) -> wxVisualAttributes

        Get the default attributes for an instance of this class.  This is
        useful if you want to use the same font or colour in your own control
        as in a standard control -- which is a much better idea than hard
        coding specific colours or fonts which might look completely out of
        place on the user's system, especially if it uses themes.
        """
        return __core.wxWindow_GetDefaultAttributes(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=wxWINDOW_VARIANT_NORMAL) -> wxVisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return __core.wxWindow_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    def SetBackgroundColour(*args, **kwargs):
        """
        SetBackgroundColour(self, wxColour colour) -> bool

        Sets the background colour of the window.  Returns True if the colour
        was changed.  The background colour is usually painted by the default
        EVT_ERASE_BACKGROUND event handler function under Windows and
        automatically under GTK.  Using `wx.NullColour` will reset the window
        to the default background colour.

        Note that setting the background colour may not cause an immediate
        refresh, so you may wish to call `ClearBackground` or `Refresh` after
        calling this function.

        Using this function will disable attempts to use themes for this
        window, if the system supports them.  Use with care since usually the
        themes represent the appearance chosen by the user to be used for all
        applications on the system.
        """
        return __core.wxWindow_SetBackgroundColour(*args, **kwargs)

    def SetOwnBackgroundColour(*args, **kwargs):
        """SetOwnBackgroundColour(self, wxColour colour)"""
        return __core.wxWindow_SetOwnBackgroundColour(*args, **kwargs)

    def SetForegroundColour(*args, **kwargs):
        """
        SetForegroundColour(self, wxColour colour) -> bool

        Sets the foreground colour of the window.  Returns True is the colour
        was changed.  The interpretation of foreground colour is dependent on
        the window class; it may be the text colour or other colour, or it may
        not be used at all.
        """
        return __core.wxWindow_SetForegroundColour(*args, **kwargs)

    def SetOwnForegroundColour(*args, **kwargs):
        """SetOwnForegroundColour(self, wxColour colour)"""
        return __core.wxWindow_SetOwnForegroundColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """
        GetBackgroundColour(self) -> wxColour

        Returns the background colour of the window.
        """
        return __core.wxWindow_GetBackgroundColour(*args, **kwargs)

    def GetForegroundColour(*args, **kwargs):
        """
        GetForegroundColour(self) -> wxColour

        Returns the foreground colour of the window.  The interpretation of
        foreground colour is dependent on the window class; it may be the text
        colour or other colour, or it may not be used at all.
        """
        return __core.wxWindow_GetForegroundColour(*args, **kwargs)

    def InheritsBackgroundColour(*args, **kwargs):
        """InheritsBackgroundColour(self) -> bool"""
        return __core.wxWindow_InheritsBackgroundColour(*args, **kwargs)

    def UseBgCol(*args, **kwargs):
        """UseBgCol(self) -> bool"""
        return __core.wxWindow_UseBgCol(*args, **kwargs)

    def SetBackgroundStyle(*args, **kwargs):
        """
        SetBackgroundStyle(self, int style) -> bool

        Returns the background style of the window. The background style
        indicates how the background of the window is drawn.

            ======================  ========================================
            wx.BG_STYLE_SYSTEM      The background colour or pattern should
                                    be determined by the system
            wx.BG_STYLE_COLOUR      The background should be a solid colour
            wx.BG_STYLE_CUSTOM      The background will be implemented by the
                                    application.
            ======================  ========================================

        On GTK+, use of wx.BG_STYLE_CUSTOM allows the flicker-free drawing of
        a custom background, such as a tiled bitmap. Currently the style has
        no effect on other platforms.

        :see: `GetBackgroundStyle`, `SetBackgroundColour`
        """
        return __core.wxWindow_SetBackgroundStyle(*args, **kwargs)

    def GetBackgroundStyle(*args, **kwargs):
        """
        GetBackgroundStyle(self) -> int

        Returns the background style of the window.

        :see: `SetBackgroundStyle`
        """
        return __core.wxWindow_GetBackgroundStyle(*args, **kwargs)

    def HasTransparentBackground(*args, **kwargs):
        """
        HasTransparentBackground(self) -> bool

        Returns True if this window's background is transparent (as, for
        example, for `wx.StaticText`) and should show the parent window's
        background.

        This method is mostly used internally by the library itself and you
        normally shouldn't have to call it. You may, however, have to override
        it in your custom control classes to ensure that background is painted
        correctly.
        """
        return __core.wxWindow_HasTransparentBackground(*args, **kwargs)

    def SetCursor(*args, **kwargs):
        """
        SetCursor(self, wxCursor cursor) -> bool

        Sets the window's cursor. Notice that the window cursor also sets it
        for the children of the window implicitly.

        The cursor may be wx.NullCursor in which case the window cursor will
        be reset back to default.
        """
        return __core.wxWindow_SetCursor(*args, **kwargs)

    def GetCursor(*args, **kwargs):
        """
        GetCursor(self) -> wxCursor

        Return the cursor associated with this window.
        """
        return __core.wxWindow_GetCursor(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """
        SetFont(self, wxFont font) -> bool

        Sets the font for this window.
        """
        return __core.wxWindow_SetFont(*args, **kwargs)

    def SetOwnFont(*args, **kwargs):
        """SetOwnFont(self, wxFont font)"""
        return __core.wxWindow_SetOwnFont(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """
        GetFont(self) -> wxFont

        Returns the default font used for this window.
        """
        return __core.wxWindow_GetFont(*args, **kwargs)

    def SetCaret(*args, **kwargs):
        """
        SetCaret(self, wxCaret caret)

        Sets the caret associated with the window.
        """
        return __core.wxWindow_SetCaret(*args, **kwargs)

    def GetCaret(*args, **kwargs):
        """
        GetCaret(self) -> wxCaret

        Returns the caret associated with the window.
        """
        return __core.wxWindow_GetCaret(*args, **kwargs)

    def GetCharHeight(*args, **kwargs):
        """
        GetCharHeight(self) -> int

        Get the (average) character size for the current font.
        """
        return __core.wxWindow_GetCharHeight(*args, **kwargs)

    def GetCharWidth(*args, **kwargs):
        """
        GetCharWidth(self) -> int

        Get the (average) character size for the current font.
        """
        return __core.wxWindow_GetCharWidth(*args, **kwargs)

    def GetTextExtent(*args, **kwargs):
        """
        GetTextExtent(String string) -> (width, height)

        Get the width and height of the text using the current font.
        """
        return __core.wxWindow_GetTextExtent(*args, **kwargs)

    def GetFullTextExtent(*args, **kwargs):
        """
        GetFullTextExtent(String string, Font font=None) ->
           (width, height, descent, externalLeading)

        Get the width, height, decent and leading of the text using the
        current or specified font.
        """
        return __core.wxWindow_GetFullTextExtent(*args, **kwargs)

    def ClientToScreenXY(*args, **kwargs):
        """
        ClientToScreenXY(int x, int y) -> (x,y)

        Converts to screen coordinates from coordinates relative to this window.
        """
        return __core.wxWindow_ClientToScreenXY(*args, **kwargs)

    def ScreenToClientXY(*args, **kwargs):
        """
        ScreenToClientXY(int x, int y) -> (x,y)

        Converts from screen to client window coordinates.
        """
        return __core.wxWindow_ScreenToClientXY(*args, **kwargs)

    def ClientToScreen(*args, **kwargs):
        """
        ClientToScreen(self, wxPoint pt) -> wxPoint

        Converts to screen coordinates from coordinates relative to this window.
        """
        return __core.wxWindow_ClientToScreen(*args, **kwargs)

    def ScreenToClient(*args, **kwargs):
        """
        ScreenToClient(self, wxPoint pt) -> wxPoint

        Converts from screen to client window coordinates.
        """
        return __core.wxWindow_ScreenToClient(*args, **kwargs)

    def HitTestXY(*args, **kwargs):
        """
        HitTestXY(self, int x, int y) -> int

        Test where the given (in client coords) point lies
        """
        return __core.wxWindow_HitTestXY(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(self, wxPoint pt) -> int

        Test where the given (in client coords) point lies
        """
        return __core.wxWindow_HitTest(*args, **kwargs)

    def GetBorder(*args):
        """
        GetBorder(self, long flags) -> int
        GetBorder(self) -> int

        Get border for the flags of this window
        """
        return __core.wxWindow_GetBorder(*args)

    def UpdateWindowUI(*args, **kwargs):
        """
        UpdateWindowUI(self, long flags=wxUPDATE_UI_NONE)

        This function sends EVT_UPDATE_UI events to the window. The particular
        implementation depends on the window; for example a wx.ToolBar will
        send an update UI event for each toolbar button, and a wx.Frame will
        send an update UI event for each menubar menu item. You can call this
        function from your application to ensure that your UI is up-to-date at
        a particular point in time (as far as your EVT_UPDATE_UI handlers are
        concerned). This may be necessary if you have called
        wx.UpdateUIEvent.SetMode or wx.UpdateUIEvent.SetUpdateInterval to
        limit the overhead that wxWindows incurs by sending update UI events
        in idle time.
        """
        return __core.wxWindow_UpdateWindowUI(*args, **kwargs)

    def PopupMenuXY(*args, **kwargs):
        """
        PopupMenuXY(self, wxMenu menu, int x=-1, int y=-1) -> bool

        Pops up the given menu at the specified coordinates, relative to this window,
        and returns control when the user has dismissed the menu. If a menu item is
        selected, the corresponding menu event is generated and will be processed as
        usual.  If the default position is given then the current position of the
        mouse cursor will be used.
        """
        return __core.wxWindow_PopupMenuXY(*args, **kwargs)

    def PopupMenu(*args, **kwargs):
        """
        PopupMenu(self, wxMenu menu, wxPoint pos=wxDefaultPosition) -> bool

        Pops up the given menu at the specified coordinates, relative to this window,
        and returns control when the user has dismissed the menu. If a menu item is
        selected, the corresponding menu event is generated and will be processed as
        usual.  If the default position is given then the current position of the
        mouse cursor will be used.
        """
        return __core.wxWindow_PopupMenu(*args, **kwargs)

    def GetHandle(*args, **kwargs):
        """
        GetHandle(self) -> long

        Returns the platform-specific handle (as a long integer) of the
        physical window.  Currently on wxMac it returns the handle of the
        toplevel parent of the window.
        """
        return __core.wxWindow_GetHandle(*args, **kwargs)

    def AssociateHandle(*args, **kwargs):
        """
        AssociateHandle(self, long handle)

        Associate the window with a new native handle
        """
        return __core.wxWindow_AssociateHandle(*args, **kwargs)

    def DissociateHandle(*args, **kwargs):
        """
        DissociateHandle(self)

        Dissociate the current native handle from the window
        """
        return __core.wxWindow_DissociateHandle(*args, **kwargs)

    def HasScrollbar(*args, **kwargs):
        """
        HasScrollbar(self, int orient) -> bool

        Does the window have the scrollbar for this orientation?
        """
        return __core.wxWindow_HasScrollbar(*args, **kwargs)

    def SetScrollbar(*args, **kwargs):
        """
        SetScrollbar(self, int orientation, int position, int thumbSize, int range, 
            bool refresh=True)

        Sets the scrollbar properties of a built-in scrollbar.
        """
        return __core.wxWindow_SetScrollbar(*args, **kwargs)

    def SetScrollPos(*args, **kwargs):
        """
        SetScrollPos(self, int orientation, int pos, bool refresh=True)

        Sets the position of one of the built-in scrollbars.
        """
        return __core.wxWindow_SetScrollPos(*args, **kwargs)

    def GetScrollPos(*args, **kwargs):
        """
        GetScrollPos(self, int orientation) -> int

        Returns the built-in scrollbar position.
        """
        return __core.wxWindow_GetScrollPos(*args, **kwargs)

    def GetScrollThumb(*args, **kwargs):
        """
        GetScrollThumb(self, int orientation) -> int

        Returns the built-in scrollbar thumb size.
        """
        return __core.wxWindow_GetScrollThumb(*args, **kwargs)

    def GetScrollRange(*args, **kwargs):
        """
        GetScrollRange(self, int orientation) -> int

        Returns the built-in scrollbar range.
        """
        return __core.wxWindow_GetScrollRange(*args, **kwargs)

    def ScrollWindow(*args, **kwargs):
        """
        ScrollWindow(self, int dx, int dy, wxRect rect=None)

        Physically scrolls the pixels in the window and move child windows
        accordingly.  Use this function to optimise your scrolling
        implementations, to minimise the area that must be redrawn. Note that
        it is rarely required to call this function from a user program.
        """
        return __core.wxWindow_ScrollWindow(*args, **kwargs)

    def ScrollLines(*args, **kwargs):
        """
        ScrollLines(self, int lines) -> bool

        If the platform and window class supports it, scrolls the window by
        the given number of lines down, if lines is positive, or up if lines
        is negative.  Returns True if the window was scrolled, False if it was
        already on top/bottom and nothing was done.
        """
        return __core.wxWindow_ScrollLines(*args, **kwargs)

    def ScrollPages(*args, **kwargs):
        """
        ScrollPages(self, int pages) -> bool

        If the platform and window class supports it, scrolls the window by
        the given number of pages down, if pages is positive, or up if pages
        is negative.  Returns True if the window was scrolled, False if it was
        already on top/bottom and nothing was done.
        """
        return __core.wxWindow_ScrollPages(*args, **kwargs)

    def LineUp(*args, **kwargs):
        """
        LineUp(self) -> bool

        This is just a wrapper for ScrollLines(-1).
        """
        return __core.wxWindow_LineUp(*args, **kwargs)

    def LineDown(*args, **kwargs):
        """
        LineDown(self) -> bool

        This is just a wrapper for ScrollLines(1).
        """
        return __core.wxWindow_LineDown(*args, **kwargs)

    def PageUp(*args, **kwargs):
        """
        PageUp(self) -> bool

        This is just a wrapper for ScrollPages(-1).
        """
        return __core.wxWindow_PageUp(*args, **kwargs)

    def PageDown(*args, **kwargs):
        """
        PageDown(self) -> bool

        This is just a wrapper for ScrollPages(1).
        """
        return __core.wxWindow_PageDown(*args, **kwargs)

    def SetHelpText(*args, **kwargs):
        """
        SetHelpText(self, wxString text)

        Sets the help text to be used as context-sensitive help for this
        window.  Note that the text is actually stored by the current
        wxHelpProvider implementation, and not in the window object itself.
        """
        return __core.wxWindow_SetHelpText(*args, **kwargs)

    def SetHelpTextForId(*args, **kwargs):
        """
        SetHelpTextForId(self, wxString text)

        Associate this help text with all windows with the same id as this
        one.
        """
        return __core.wxWindow_SetHelpTextForId(*args, **kwargs)

    def GetHelpText(*args, **kwargs):
        """
        GetHelpText(self) -> wxString

        Gets the help text to be used as context-sensitive help for this
        window.  Note that the text is actually stored by the current
        wxHelpProvider implementation, and not in the window object itself.
        """
        return __core.wxWindow_GetHelpText(*args, **kwargs)

    def SetToolTipString(*args, **kwargs):
        """
        SetToolTipString(self, wxString tip)

        Attach a tooltip to the window.
        """
        return __core.wxWindow_SetToolTipString(*args, **kwargs)

    def SetToolTip(*args, **kwargs):
        """
        SetToolTip(self, wxToolTip tip)

        Attach a tooltip to the window.
        """
        return __core.wxWindow_SetToolTip(*args, **kwargs)

    def GetToolTip(*args, **kwargs):
        """
        GetToolTip(self) -> wxToolTip

        get the associated tooltip or None if none
        """
        return __core.wxWindow_GetToolTip(*args, **kwargs)

    def SetDropTarget(*args, **kwargs):
        """
        SetDropTarget(self, wxPyDropTarget dropTarget)

        Associates a drop target with this window.  If the window already has
        a drop target, it is deleted.
        """
        return __core.wxWindow_SetDropTarget(*args, **kwargs)

    def GetDropTarget(*args, **kwargs):
        """
        GetDropTarget(self) -> wxPyDropTarget

        Returns the associated drop target, which may be None.
        """
        return __core.wxWindow_GetDropTarget(*args, **kwargs)

    def SetConstraints(*args, **kwargs):
        """
        SetConstraints(self, wxLayoutConstraints constraints)

        Sets the window to have the given layout constraints. If an existing
        layout constraints object is already owned by the window, it will be
        deleted.  Pass None to disassociate and delete the window's current
        constraints.

        You must call SetAutoLayout to tell a window to use the constraints
        automatically in its default EVT_SIZE handler; otherwise, you must
        handle EVT_SIZE yourself and call Layout() explicitly. When setting
        both a wx.LayoutConstraints and a wx.Sizer, only the sizer will have
        effect.
        """
        return __core.wxWindow_SetConstraints(*args, **kwargs)

    def GetConstraints(*args, **kwargs):
        """
        GetConstraints(self) -> wxLayoutConstraints

        Returns a pointer to the window's layout constraints, or None if there
        are none.
        """
        return __core.wxWindow_GetConstraints(*args, **kwargs)

    def SetAutoLayout(*args, **kwargs):
        """
        SetAutoLayout(self, bool autoLayout)

        Determines whether the Layout function will be called automatically
        when the window is resized.  It is called implicitly by SetSizer but
        if you use SetConstraints you should call it manually or otherwise the
        window layout won't be correctly updated when its size changes.
        """
        return __core.wxWindow_SetAutoLayout(*args, **kwargs)

    def GetAutoLayout(*args, **kwargs):
        """
        GetAutoLayout(self) -> bool

        Returns the current autoLayout setting
        """
        return __core.wxWindow_GetAutoLayout(*args, **kwargs)

    def Layout(*args, **kwargs):
        """
        Layout(self) -> bool

        Invokes the constraint-based layout algorithm or the sizer-based
        algorithm for this window.  See SetAutoLayout: when auto layout is on,
        this function gets called automatically by the default EVT_SIZE
        handler when the window is resized.
        """
        return __core.wxWindow_Layout(*args, **kwargs)

    def SetSizer(*args, **kwargs):
        """
        SetSizer(self, wxSizer sizer, bool deleteOld=True)

        Sets the window to have the given layout sizer. The window will then
        own the object, and will take care of its deletion. If an existing
        layout sizer object is already owned by the window, it will be deleted
        if the deleteOld parameter is true. Note that this function will also
        call SetAutoLayout implicitly with a True parameter if the sizer is
        non-None, and False otherwise.
        """
        return __core.wxWindow_SetSizer(*args, **kwargs)

    def SetSizerAndFit(*args, **kwargs):
        """
        SetSizerAndFit(self, wxSizer sizer, bool deleteOld=True)

        The same as SetSizer, except it also sets the size hints for the
        window based on the sizer's minimum size.
        """
        return __core.wxWindow_SetSizerAndFit(*args, **kwargs)

    def GetSizer(*args, **kwargs):
        """
        GetSizer(self) -> wxSizer

        Return the sizer associated with the window by a previous call to
        SetSizer or None if there isn't one.
        """
        return __core.wxWindow_GetSizer(*args, **kwargs)

    def SetContainingSizer(*args, **kwargs):
        """
        SetContainingSizer(self, wxSizer sizer)

        This normally does not need to be called by application code. It is
        called internally when a window is added to a sizer, and is used so
        the window can remove itself from the sizer when it is destroyed.
        """
        return __core.wxWindow_SetContainingSizer(*args, **kwargs)

    def GetContainingSizer(*args, **kwargs):
        """
        GetContainingSizer(self) -> wxSizer

        Return the sizer that this window is a member of, if any, otherwise None.
        """
        return __core.wxWindow_GetContainingSizer(*args, **kwargs)

    def InheritAttributes(*args, **kwargs):
        """
        InheritAttributes(self)

        This function is (or should be, in case of custom controls) called
        during window creation to intelligently set up the window visual
        attributes, that is the font and the foreground and background
        colours.

        By 'intelligently' the following is meant: by default, all windows use
        their own default attributes. However if some of the parent's
        attributes are explicitly changed (that is, using SetFont and not
        SetOwnFont) and if the corresponding attribute hadn't been
        explicitly set for this window itself, then this window takes the same
        value as used by the parent. In addition, if the window overrides
        ShouldInheritColours to return false, the colours will not be changed
        no matter what and only the font might.

        This rather complicated logic is necessary in order to accommodate the
        different usage scenarios. The most common one is when all default
        attributes are used and in this case, nothing should be inherited as
        in modern GUIs different controls use different fonts (and colours)
        than their siblings so they can't inherit the same value from the
        parent. However it was also deemed desirable to allow to simply change
        the attributes of all children at once by just changing the font or
        colour of their common parent, hence in this case we do inherit the
        parents attributes.

        """
        return __core.wxWindow_InheritAttributes(*args, **kwargs)

    def ShouldInheritColours(*args, **kwargs):
        """
        ShouldInheritColours(self) -> bool

        Return true from here to allow the colours of this window to be
        changed by InheritAttributes, returning false forbids inheriting them
        from the parent window.

        The base class version returns false, but this method is overridden in
        wxControl where it returns true.
        """
        return __core.wxWindow_ShouldInheritColours(*args, **kwargs)

    def PostCreate(self, pre):
        """
        Phase 3 of the 2-phase create <wink!>
        Call this method after precreating the window with the 2-phase create method.
        """
        self.this = pre.this
        self.thisown = pre.thisown
        pre.thisown = 0
        if hasattr(self, '_setOORInfo'):
            self._setOORInfo(self)
        if hasattr(self, '_setCallbackInfo'):
            self._setCallbackInfo(self, self.__class__)


class wxWindowPtr(wxWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxWindow
__core.wxWindow_swigregister(wxWindowPtr)

def PreWindow(*args, **kwargs):
    """
    PreWindow() -> wxWindow

    Precreate a Window for 2-phase creation.
    """
    val = __core.new_PreWindow(*args, **kwargs)
    val.thisown = 1
    return val

def wxWindow_NewControlId(*args, **kwargs):
    """
    wxWindow_NewControlId() -> int

    Generate a control id for the controls which were not given one.
    """
    return __core.wxWindow_NewControlId(*args, **kwargs)

def wxWindow_NextControlId(*args, **kwargs):
    """
    wxWindow_NextControlId(int winid) -> int

    Get the id of the control following the one with the given
    autogenerated) id
    """
    return __core.wxWindow_NextControlId(*args, **kwargs)

def wxWindow_PrevControlId(*args, **kwargs):
    """
    wxWindow_PrevControlId(int winid) -> int

    Get the id of the control preceding the one with the given
    autogenerated) id
    """
    return __core.wxWindow_PrevControlId(*args, **kwargs)

def wxWindow_FindFocus(*args, **kwargs):
    """
    wxWindow_FindFocus() -> wxWindow

    Returns the window or control that currently has the keyboard focus,
    or None.
    """
    return __core.wxWindow_FindFocus(*args, **kwargs)

def wxWindow_GetCapture(*args, **kwargs):
    """
    wxWindow_GetCapture() -> wxWindow

    Returns the window which currently captures the mouse or None
    """
    return __core.wxWindow_GetCapture(*args, **kwargs)

def wxWindow_GetClassDefaultAttributes(*args, **kwargs):
    """
    wxWindow_GetClassDefaultAttributes(int variant=wxWINDOW_VARIANT_NORMAL) -> wxVisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
    return __core.wxWindow_GetClassDefaultAttributes(*args, **kwargs)

def DLG_PNT(win, point_or_x, y=None):
    """
    Convenience function for converting a Point or (x,y) in
    dialog units to pixel units.
    """
    if y is None:
        return win.ConvertDialogPointToPixels(point_or_x)
    else:
        return win.ConvertDialogPointToPixels(wx.Point(point_or_x, y))

def DLG_SZE(win, size_width, height=None):
    """
    Convenience function for converting a Size or (w,h) in
    dialog units to pixel units.
    """
    if height is None:
        return win.ConvertDialogSizeToPixels(size_width)
    else:
        return win.ConvertDialogSizeToPixels(wx.Size(size_width, height))


def wxFindWindowById(*args, **kwargs):
    """
    wxFindWindowById(long id, wxWindow parent=None) -> wxWindow

    Find the first window in the application with the given id. If parent
    is None, the search will start from all top-level frames and dialog
    boxes; if non-None, the search will be limited to the given window
    hierarchy. The search is recursive in both cases.
    """
    return __core.wxFindWindowById(*args, **kwargs)

def wxFindWindowByName(*args, **kwargs):
    """
    wxFindWindowByName(wxString name, wxWindow parent=None) -> wxWindow

    Find a window by its name (as given in a window constructor or Create
    function call). If parent is None, the search will start from all
    top-level frames and dialog boxes; if non-None, the search will be
    limited to the given window hierarchy. The search is recursive in both
    cases.

    If no window with such name is found, wx.FindWindowByLabel is called.
    """
    return __core.wxFindWindowByName(*args, **kwargs)

def wxFindWindowByLabel(*args, **kwargs):
    """
    wxFindWindowByLabel(wxString label, wxWindow parent=None) -> wxWindow

    Find a window by its label. Depending on the type of window, the label
    may be a window title or panel item label. If parent is None, the
    search will start from all top-level frames and dialog boxes; if
    non-None, the search will be limited to the given window
    hierarchy. The search is recursive in both cases.
    """
    return __core.wxFindWindowByLabel(*args, **kwargs)

def wxWindow_FromHWND(*args, **kwargs):
    """wxWindow_FromHWND(wxWindow parent, unsigned long _hWnd) -> wxWindow"""
    return __core.wxWindow_FromHWND(*args, **kwargs)
#---------------------------------------------------------------------------

class wxValidator(wxEvtHandler):
    """Proxy of C++ wxValidator class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxValidator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxValidator"""
        newobj = __core.new_wxValidator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Clone(*args, **kwargs):
        """Clone(self) -> wxValidator"""
        return __core.wxValidator_Clone(*args, **kwargs)

    def Validate(*args, **kwargs):
        """Validate(self, wxWindow parent) -> bool"""
        return __core.wxValidator_Validate(*args, **kwargs)

    def TransferToWindow(*args, **kwargs):
        """TransferToWindow(self) -> bool"""
        return __core.wxValidator_TransferToWindow(*args, **kwargs)

    def TransferFromWindow(*args, **kwargs):
        """TransferFromWindow(self) -> bool"""
        return __core.wxValidator_TransferFromWindow(*args, **kwargs)

    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> wxWindow"""
        return __core.wxValidator_GetWindow(*args, **kwargs)

    def SetWindow(*args, **kwargs):
        """SetWindow(self, wxWindow window)"""
        return __core.wxValidator_SetWindow(*args, **kwargs)

    def IsSilent(*args, **kwargs):
        """IsSilent() -> bool"""
        return __core.wxValidator_IsSilent(*args, **kwargs)

    IsSilent = staticmethod(IsSilent)
    def SetBellOnError(*args, **kwargs):
        """SetBellOnError(int doIt=True)"""
        return __core.wxValidator_SetBellOnError(*args, **kwargs)

    SetBellOnError = staticmethod(SetBellOnError)

class wxValidatorPtr(wxValidator):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxValidator
__core.wxValidator_swigregister(wxValidatorPtr)

def wxValidator_IsSilent(*args, **kwargs):
    """wxValidator_IsSilent() -> bool"""
    return __core.wxValidator_IsSilent(*args, **kwargs)

def wxValidator_SetBellOnError(*args, **kwargs):
    """wxValidator_SetBellOnError(int doIt=True)"""
    return __core.wxValidator_SetBellOnError(*args, **kwargs)

class wxPyValidator(wxValidator):
    """Proxy of C++ wxPyValidator class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyValidator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxPyValidator"""
        newobj = __core.new_wxPyValidator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        
        self._setCallbackInfo(self, PyValidator, 1)
        self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class, int incref=True)"""
        return __core.wxPyValidator__setCallbackInfo(*args, **kwargs)


class wxPyValidatorPtr(wxPyValidator):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPyValidator
__core.wxPyValidator_swigregister(wxPyValidatorPtr)

#---------------------------------------------------------------------------

class wxMenu(wxEvtHandler):
    """Proxy of C++ wxMenu class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMenu instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxString title=EmptyString, long style=0) -> wxMenu"""
        newobj = __core.new_wxMenu(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Append(*args, **kwargs):
        """Append(self, int id, wxString text, wxString help=EmptyString, int kind=wxITEM_NORMAL) -> wxMenuItem"""
        return __core.wxMenu_Append(*args, **kwargs)

    def AppendSeparator(*args, **kwargs):
        """AppendSeparator(self) -> wxMenuItem"""
        return __core.wxMenu_AppendSeparator(*args, **kwargs)

    def AppendCheckItem(*args, **kwargs):
        """AppendCheckItem(self, int id, wxString text, wxString help=EmptyString) -> wxMenuItem"""
        return __core.wxMenu_AppendCheckItem(*args, **kwargs)

    def AppendRadioItem(*args, **kwargs):
        """AppendRadioItem(self, int id, wxString text, wxString help=EmptyString) -> wxMenuItem"""
        return __core.wxMenu_AppendRadioItem(*args, **kwargs)

    def AppendMenu(*args, **kwargs):
        """AppendMenu(self, int id, wxString text, wxMenu submenu, wxString help=EmptyString) -> wxMenuItem"""
        return __core.wxMenu_AppendMenu(*args, **kwargs)

    def AppendItem(*args, **kwargs):
        """AppendItem(self, wxMenuItem item) -> wxMenuItem"""
        return __core.wxMenu_AppendItem(*args, **kwargs)

    def Break(*args, **kwargs):
        """Break(self)"""
        return __core.wxMenu_Break(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """InsertItem(self, size_t pos, wxMenuItem item) -> wxMenuItem"""
        return __core.wxMenu_InsertItem(*args, **kwargs)

    def Insert(*args, **kwargs):
        """
        Insert(self, size_t pos, int id, wxString text, wxString help=EmptyString, 
            int kind=wxITEM_NORMAL) -> wxMenuItem
        """
        return __core.wxMenu_Insert(*args, **kwargs)

    def InsertSeparator(*args, **kwargs):
        """InsertSeparator(self, size_t pos) -> wxMenuItem"""
        return __core.wxMenu_InsertSeparator(*args, **kwargs)

    def InsertCheckItem(*args, **kwargs):
        """InsertCheckItem(self, size_t pos, int id, wxString text, wxString help=EmptyString) -> wxMenuItem"""
        return __core.wxMenu_InsertCheckItem(*args, **kwargs)

    def InsertRadioItem(*args, **kwargs):
        """InsertRadioItem(self, size_t pos, int id, wxString text, wxString help=EmptyString) -> wxMenuItem"""
        return __core.wxMenu_InsertRadioItem(*args, **kwargs)

    def InsertMenu(*args, **kwargs):
        """
        InsertMenu(self, size_t pos, int id, wxString text, wxMenu submenu, 
            wxString help=EmptyString) -> wxMenuItem
        """
        return __core.wxMenu_InsertMenu(*args, **kwargs)

    def PrependItem(*args, **kwargs):
        """PrependItem(self, wxMenuItem item) -> wxMenuItem"""
        return __core.wxMenu_PrependItem(*args, **kwargs)

    def Prepend(*args, **kwargs):
        """Prepend(self, int id, wxString text, wxString help=EmptyString, int kind=wxITEM_NORMAL) -> wxMenuItem"""
        return __core.wxMenu_Prepend(*args, **kwargs)

    def PrependSeparator(*args, **kwargs):
        """PrependSeparator(self) -> wxMenuItem"""
        return __core.wxMenu_PrependSeparator(*args, **kwargs)

    def PrependCheckItem(*args, **kwargs):
        """PrependCheckItem(self, int id, wxString text, wxString help=EmptyString) -> wxMenuItem"""
        return __core.wxMenu_PrependCheckItem(*args, **kwargs)

    def PrependRadioItem(*args, **kwargs):
        """PrependRadioItem(self, int id, wxString text, wxString help=EmptyString) -> wxMenuItem"""
        return __core.wxMenu_PrependRadioItem(*args, **kwargs)

    def PrependMenu(*args, **kwargs):
        """PrependMenu(self, int id, wxString text, wxMenu submenu, wxString help=EmptyString) -> wxMenuItem"""
        return __core.wxMenu_PrependMenu(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(self, int id) -> wxMenuItem"""
        return __core.wxMenu_Remove(*args, **kwargs)

    def RemoveItem(*args, **kwargs):
        """RemoveItem(self, wxMenuItem item) -> wxMenuItem"""
        return __core.wxMenu_RemoveItem(*args, **kwargs)

    def Delete(*args, **kwargs):
        """Delete(self, int id) -> bool"""
        return __core.wxMenu_Delete(*args, **kwargs)

    def DeleteItem(*args, **kwargs):
        """DeleteItem(self, wxMenuItem item) -> bool"""
        return __core.wxMenu_DeleteItem(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """
        Destroy(self)

        Deletes the C++ object this Python object is a proxy for.
        """
        return __core.wxMenu_Destroy(*args, **kwargs)

    def DestroyId(*args, **kwargs):
        """
        DestroyId(self, int id) -> bool

        Deletes the C++ object this Python object is a proxy for.
        """
        return __core.wxMenu_DestroyId(*args, **kwargs)

    def DestroyItem(*args, **kwargs):
        """
        DestroyItem(self, wxMenuItem item) -> bool

        Deletes the C++ object this Python object is a proxy for.
        """
        return __core.wxMenu_DestroyItem(*args, **kwargs)

    def GetMenuItemCount(*args, **kwargs):
        """GetMenuItemCount(self) -> size_t"""
        return __core.wxMenu_GetMenuItemCount(*args, **kwargs)

    def GetMenuItems(*args, **kwargs):
        """GetMenuItems(self) -> PyObject"""
        return __core.wxMenu_GetMenuItems(*args, **kwargs)

    def FindItem(*args, **kwargs):
        """FindItem(self, wxString item) -> int"""
        return __core.wxMenu_FindItem(*args, **kwargs)

    def FindItemById(*args, **kwargs):
        """FindItemById(self, int id) -> wxMenuItem"""
        return __core.wxMenu_FindItemById(*args, **kwargs)

    def FindItemByPosition(*args, **kwargs):
        """FindItemByPosition(self, size_t position) -> wxMenuItem"""
        return __core.wxMenu_FindItemByPosition(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(self, int id, bool enable)"""
        return __core.wxMenu_Enable(*args, **kwargs)

    def IsEnabled(*args, **kwargs):
        """IsEnabled(self, int id) -> bool"""
        return __core.wxMenu_IsEnabled(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(self, int id, bool check)"""
        return __core.wxMenu_Check(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked(self, int id) -> bool"""
        return __core.wxMenu_IsChecked(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """SetLabel(self, int id, wxString label)"""
        return __core.wxMenu_SetLabel(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel(self, int id) -> wxString"""
        return __core.wxMenu_GetLabel(*args, **kwargs)

    def SetHelpString(*args, **kwargs):
        """SetHelpString(self, int id, wxString helpString)"""
        return __core.wxMenu_SetHelpString(*args, **kwargs)

    def GetHelpString(*args, **kwargs):
        """GetHelpString(self, int id) -> wxString"""
        return __core.wxMenu_GetHelpString(*args, **kwargs)

    def SetTitle(*args, **kwargs):
        """SetTitle(self, wxString title)"""
        return __core.wxMenu_SetTitle(*args, **kwargs)

    def GetTitle(*args, **kwargs):
        """GetTitle(self) -> wxString"""
        return __core.wxMenu_GetTitle(*args, **kwargs)

    def SetEventHandler(*args, **kwargs):
        """SetEventHandler(self, wxEvtHandler handler)"""
        return __core.wxMenu_SetEventHandler(*args, **kwargs)

    def GetEventHandler(*args, **kwargs):
        """GetEventHandler(self) -> wxEvtHandler"""
        return __core.wxMenu_GetEventHandler(*args, **kwargs)

    def SetInvokingWindow(*args, **kwargs):
        """SetInvokingWindow(self, wxWindow win)"""
        return __core.wxMenu_SetInvokingWindow(*args, **kwargs)

    def GetInvokingWindow(*args, **kwargs):
        """GetInvokingWindow(self) -> wxWindow"""
        return __core.wxMenu_GetInvokingWindow(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self) -> long"""
        return __core.wxMenu_GetStyle(*args, **kwargs)

    def UpdateUI(*args, **kwargs):
        """UpdateUI(self, wxEvtHandler source=None)"""
        return __core.wxMenu_UpdateUI(*args, **kwargs)

    def GetMenuBar(*args, **kwargs):
        """GetMenuBar(self) -> wxMenuBar"""
        return __core.wxMenu_GetMenuBar(*args, **kwargs)

    def Attach(*args, **kwargs):
        """Attach(self, wxMenuBarBase menubar)"""
        return __core.wxMenu_Attach(*args, **kwargs)

    def Detach(*args, **kwargs):
        """Detach(self)"""
        return __core.wxMenu_Detach(*args, **kwargs)

    def IsAttached(*args, **kwargs):
        """IsAttached(self) -> bool"""
        return __core.wxMenu_IsAttached(*args, **kwargs)

    def SetParent(*args, **kwargs):
        """SetParent(self, wxMenu parent)"""
        return __core.wxMenu_SetParent(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent(self) -> wxMenu"""
        return __core.wxMenu_GetParent(*args, **kwargs)


class wxMenuPtr(wxMenu):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxMenu
__core.wxMenu_swigregister(wxMenuPtr)
wxDefaultValidator = cvar.wxDefaultValidator

#---------------------------------------------------------------------------

class wxMenuBar(wxWindow):
    """Proxy of C++ wxMenuBar class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMenuBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, long style=0) -> wxMenuBar"""
        newobj = __core.new_wxMenuBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Append(*args, **kwargs):
        """Append(self, wxMenu menu, wxString title) -> bool"""
        return __core.wxMenuBar_Append(*args, **kwargs)

    def Insert(*args, **kwargs):
        """Insert(self, size_t pos, wxMenu menu, wxString title) -> bool"""
        return __core.wxMenuBar_Insert(*args, **kwargs)

    def GetMenuCount(*args, **kwargs):
        """GetMenuCount(self) -> size_t"""
        return __core.wxMenuBar_GetMenuCount(*args, **kwargs)

    def GetMenu(*args, **kwargs):
        """GetMenu(self, size_t pos) -> wxMenu"""
        return __core.wxMenuBar_GetMenu(*args, **kwargs)

    def Replace(*args, **kwargs):
        """Replace(self, size_t pos, wxMenu menu, wxString title) -> wxMenu"""
        return __core.wxMenuBar_Replace(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(self, size_t pos) -> wxMenu"""
        return __core.wxMenuBar_Remove(*args, **kwargs)

    def EnableTop(*args, **kwargs):
        """EnableTop(self, size_t pos, bool enable)"""
        return __core.wxMenuBar_EnableTop(*args, **kwargs)

    def IsEnabledTop(*args, **kwargs):
        """IsEnabledTop(self, size_t pos) -> bool"""
        return __core.wxMenuBar_IsEnabledTop(*args, **kwargs)

    def SetLabelTop(*args, **kwargs):
        """SetLabelTop(self, size_t pos, wxString label)"""
        return __core.wxMenuBar_SetLabelTop(*args, **kwargs)

    def GetLabelTop(*args, **kwargs):
        """GetLabelTop(self, size_t pos) -> wxString"""
        return __core.wxMenuBar_GetLabelTop(*args, **kwargs)

    def FindMenuItem(*args, **kwargs):
        """FindMenuItem(self, wxString menu, wxString item) -> int"""
        return __core.wxMenuBar_FindMenuItem(*args, **kwargs)

    def FindItemById(*args, **kwargs):
        """FindItemById(self, int id) -> wxMenuItem"""
        return __core.wxMenuBar_FindItemById(*args, **kwargs)

    def FindMenu(*args, **kwargs):
        """FindMenu(self, wxString title) -> int"""
        return __core.wxMenuBar_FindMenu(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(self, int id, bool enable)"""
        return __core.wxMenuBar_Enable(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(self, int id, bool check)"""
        return __core.wxMenuBar_Check(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked(self, int id) -> bool"""
        return __core.wxMenuBar_IsChecked(*args, **kwargs)

    def IsEnabled(*args, **kwargs):
        """IsEnabled(self, int id) -> bool"""
        return __core.wxMenuBar_IsEnabled(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """SetLabel(self, int id, wxString label)"""
        return __core.wxMenuBar_SetLabel(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel(self, int id) -> wxString"""
        return __core.wxMenuBar_GetLabel(*args, **kwargs)

    def SetHelpString(*args, **kwargs):
        """SetHelpString(self, int id, wxString helpString)"""
        return __core.wxMenuBar_SetHelpString(*args, **kwargs)

    def GetHelpString(*args, **kwargs):
        """GetHelpString(self, int id) -> wxString"""
        return __core.wxMenuBar_GetHelpString(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame(self) -> wxFrame"""
        return __core.wxMenuBar_GetFrame(*args, **kwargs)

    def IsAttached(*args, **kwargs):
        """IsAttached(self) -> bool"""
        return __core.wxMenuBar_IsAttached(*args, **kwargs)

    def Attach(*args, **kwargs):
        """Attach(self, wxFrame frame)"""
        return __core.wxMenuBar_Attach(*args, **kwargs)

    def Detach(*args, **kwargs):
        """Detach(self)"""
        return __core.wxMenuBar_Detach(*args, **kwargs)


class wxMenuBarPtr(wxMenuBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxMenuBar
__core.wxMenuBar_swigregister(wxMenuBarPtr)

#---------------------------------------------------------------------------

class wxMenuItem(wxObject):
    """Proxy of C++ wxMenuItem class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMenuItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxMenu parentMenu=None, int id=wxID_ANY, wxString text=EmptyString, 
            wxString help=EmptyString, int kind=wxITEM_NORMAL, 
            wxMenu subMenu=None) -> wxMenuItem
        """
        newobj = __core.new_wxMenuItem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetMenu(*args, **kwargs):
        """GetMenu(self) -> wxMenu"""
        return __core.wxMenuItem_GetMenu(*args, **kwargs)

    def SetMenu(*args, **kwargs):
        """SetMenu(self, wxMenu menu)"""
        return __core.wxMenuItem_SetMenu(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(self, int id)"""
        return __core.wxMenuItem_SetId(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> int"""
        return __core.wxMenuItem_GetId(*args, **kwargs)

    def IsSeparator(*args, **kwargs):
        """IsSeparator(self) -> bool"""
        return __core.wxMenuItem_IsSeparator(*args, **kwargs)

    def SetText(*args, **kwargs):
        """SetText(self, wxString str)"""
        return __core.wxMenuItem_SetText(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel(self) -> wxString"""
        return __core.wxMenuItem_GetLabel(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText(self) -> wxString"""
        return __core.wxMenuItem_GetText(*args, **kwargs)

    def GetLabelFromText(*args, **kwargs):
        """GetLabelFromText(wxString text) -> wxString"""
        return __core.wxMenuItem_GetLabelFromText(*args, **kwargs)

    GetLabelFromText = staticmethod(GetLabelFromText)
    def GetKind(*args, **kwargs):
        """GetKind(self) -> int"""
        return __core.wxMenuItem_GetKind(*args, **kwargs)

    def SetKind(*args, **kwargs):
        """SetKind(self, int kind)"""
        return __core.wxMenuItem_SetKind(*args, **kwargs)

    def SetCheckable(*args, **kwargs):
        """SetCheckable(self, bool checkable)"""
        return __core.wxMenuItem_SetCheckable(*args, **kwargs)

    def IsCheckable(*args, **kwargs):
        """IsCheckable(self) -> bool"""
        return __core.wxMenuItem_IsCheckable(*args, **kwargs)

    def IsSubMenu(*args, **kwargs):
        """IsSubMenu(self) -> bool"""
        return __core.wxMenuItem_IsSubMenu(*args, **kwargs)

    def SetSubMenu(*args, **kwargs):
        """SetSubMenu(self, wxMenu menu)"""
        return __core.wxMenuItem_SetSubMenu(*args, **kwargs)

    def GetSubMenu(*args, **kwargs):
        """GetSubMenu(self) -> wxMenu"""
        return __core.wxMenuItem_GetSubMenu(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(self, bool enable=True)"""
        return __core.wxMenuItem_Enable(*args, **kwargs)

    def IsEnabled(*args, **kwargs):
        """IsEnabled(self) -> bool"""
        return __core.wxMenuItem_IsEnabled(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(self, bool check=True)"""
        return __core.wxMenuItem_Check(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked(self) -> bool"""
        return __core.wxMenuItem_IsChecked(*args, **kwargs)

    def Toggle(*args, **kwargs):
        """Toggle(self)"""
        return __core.wxMenuItem_Toggle(*args, **kwargs)

    def SetHelp(*args, **kwargs):
        """SetHelp(self, wxString str)"""
        return __core.wxMenuItem_SetHelp(*args, **kwargs)

    def GetHelp(*args, **kwargs):
        """GetHelp(self) -> wxString"""
        return __core.wxMenuItem_GetHelp(*args, **kwargs)

    def GetAccel(*args, **kwargs):
        """GetAccel(self) -> wxAcceleratorEntry"""
        return __core.wxMenuItem_GetAccel(*args, **kwargs)

    def SetAccel(*args, **kwargs):
        """SetAccel(self, wxAcceleratorEntry accel)"""
        return __core.wxMenuItem_SetAccel(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(self, wxFont font)"""
        return __core.wxMenuItem_SetFont(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(self) -> wxFont"""
        return __core.wxMenuItem_GetFont(*args, **kwargs)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(self, wxColour colText)"""
        return __core.wxMenuItem_SetTextColour(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour(self) -> wxColour"""
        return __core.wxMenuItem_GetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(self, wxColour colBack)"""
        return __core.wxMenuItem_SetBackgroundColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self) -> wxColour"""
        return __core.wxMenuItem_GetBackgroundColour(*args, **kwargs)

    def SetBitmaps(*args, **kwargs):
        """SetBitmaps(self, wxBitmap bmpChecked, wxBitmap bmpUnchecked=wxNullBitmap)"""
        return __core.wxMenuItem_SetBitmaps(*args, **kwargs)

    def SetDisabledBitmap(*args, **kwargs):
        """SetDisabledBitmap(self, wxBitmap bmpDisabled)"""
        return __core.wxMenuItem_SetDisabledBitmap(*args, **kwargs)

    def GetDisabledBitmap(*args, **kwargs):
        """GetDisabledBitmap(self) -> wxBitmap"""
        return __core.wxMenuItem_GetDisabledBitmap(*args, **kwargs)

    def SetMarginWidth(*args, **kwargs):
        """SetMarginWidth(self, int nWidth)"""
        return __core.wxMenuItem_SetMarginWidth(*args, **kwargs)

    def GetMarginWidth(*args, **kwargs):
        """GetMarginWidth(self) -> int"""
        return __core.wxMenuItem_GetMarginWidth(*args, **kwargs)

    def GetDefaultMarginWidth(*args, **kwargs):
        """GetDefaultMarginWidth() -> int"""
        return __core.wxMenuItem_GetDefaultMarginWidth(*args, **kwargs)

    GetDefaultMarginWidth = staticmethod(GetDefaultMarginWidth)
    def IsOwnerDrawn(*args, **kwargs):
        """IsOwnerDrawn(self) -> bool"""
        return __core.wxMenuItem_IsOwnerDrawn(*args, **kwargs)

    def SetOwnerDrawn(*args, **kwargs):
        """SetOwnerDrawn(self, bool ownerDrawn=True)"""
        return __core.wxMenuItem_SetOwnerDrawn(*args, **kwargs)

    def ResetOwnerDrawn(*args, **kwargs):
        """ResetOwnerDrawn(self)"""
        return __core.wxMenuItem_ResetOwnerDrawn(*args, **kwargs)

    def SetBitmap(*args, **kwargs):
        """SetBitmap(self, wxBitmap bitmap)"""
        return __core.wxMenuItem_SetBitmap(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap(self) -> wxBitmap"""
        return __core.wxMenuItem_GetBitmap(*args, **kwargs)


class wxMenuItemPtr(wxMenuItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxMenuItem
__core.wxMenuItem_swigregister(wxMenuItemPtr)

def wxMenuItem_GetLabelFromText(*args, **kwargs):
    """wxMenuItem_GetLabelFromText(wxString text) -> wxString"""
    return __core.wxMenuItem_GetLabelFromText(*args, **kwargs)

def wxMenuItem_GetDefaultMarginWidth(*args, **kwargs):
    """wxMenuItem_GetDefaultMarginWidth() -> int"""
    return __core.wxMenuItem_GetDefaultMarginWidth(*args, **kwargs)

#---------------------------------------------------------------------------

class wxControl(wxWindow):
    """
    This is the base class for a control or 'widget'.

    A control is generally a small window which processes user input
    and/or displays one or more item of data.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxControl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxWindow parent, int id=-1, wxPoint pos=wxDefaultPosition, 
            wxSize size=wxDefaultSize, long style=0, 
            wxValidator validator=wxDefaultValidator, 
            wxString name=ControlNameStr) -> wxControl

        Create a Control.  Normally you should only call this from a subclass'
        __init__ as a plain old wx.Control is not very useful.
        """
        newobj = __core.new_wxControl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, wxWindow parent, int id=-1, wxPoint pos=wxDefaultPosition, 
            wxSize size=wxDefaultSize, long style=0, 
            wxValidator validator=wxDefaultValidator, 
            wxString name=ControlNameStr) -> bool

        Do the 2nd phase and create the GUI control.
        """
        return __core.wxControl_Create(*args, **kwargs)

    def Command(*args, **kwargs):
        """
        Command(self, wxCommandEvent event)

        Simulates the effect of the user issuing a command to the item.

        :see: `wx.CommandEvent`

        """
        return __core.wxControl_Command(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """
        GetLabel(self) -> wxString

        Return a control's text.
        """
        return __core.wxControl_GetLabel(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """
        SetLabel(self, wxString label)

        Sets the item's text.
        """
        return __core.wxControl_SetLabel(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=wxWINDOW_VARIANT_NORMAL) -> wxVisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return __core.wxControl_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)

class wxControlPtr(wxControl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxControl
__core.wxControl_swigregister(wxControlPtr)
ControlNameStr = cvar.ControlNameStr

def PreControl(*args, **kwargs):
    """
    PreControl() -> wxControl

    Precreate a Control control for 2-phase creation
    """
    val = __core.new_PreControl(*args, **kwargs)
    val.thisown = 1
    return val

def wxControl_GetClassDefaultAttributes(*args, **kwargs):
    """
    wxControl_GetClassDefaultAttributes(int variant=wxWINDOW_VARIANT_NORMAL) -> wxVisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
    return __core.wxControl_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class wxItemContainer(object):
    """
    The wx.ItemContainer class defines an interface which is implemented
    by all controls which have string subitems, each of which may be
    selected, such as `wx.ListBox`, `wx.CheckListBox`, `wx.Choice` as well
    as `wx.ComboBox` which implements an extended interface deriving from
    this one.

    It defines the methods for accessing the control's items and although
    each of the derived classes implements them differently, they still
    all conform to the same interface.

    The items in a wx.ItemContainer have (non empty) string labels and,
    optionally, client data associated with them.

    """
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxItemContainer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def Append(*args, **kwargs):
        """
        Append(self, wxString item, PyObject clientData=None) -> int

        Adds the item to the control, associating the given data with the item
        if not None.  The return value is the index of the newly added item
        which may be different from the last one if the control is sorted (e.g.
        has wx.LB_SORT or wx.CB_SORT style).
        """
        return __core.wxItemContainer_Append(*args, **kwargs)

    def AppendItems(*args, **kwargs):
        """
        AppendItems(self, List strings)

        Apend several items at once to the control.  Notice that calling this
        method may be much faster than appending the items one by one if you
        need to add a lot of items.
        """
        return __core.wxItemContainer_AppendItems(*args, **kwargs)

    def Insert(*args, **kwargs):
        """
        Insert(self, wxString item, int pos, PyObject clientData=None) -> int

        Insert an item into the control before the item at the ``pos`` index,
        optionally associating some data object with the item.
        """
        return __core.wxItemContainer_Insert(*args, **kwargs)

    def Clear(*args, **kwargs):
        """
        Clear(self)

        Removes all items from the control.
        """
        return __core.wxItemContainer_Clear(*args, **kwargs)

    def Delete(*args, **kwargs):
        """
        Delete(self, int n)

        Deletes the item at the zero-based index 'n' from the control. Note
        that it is an error (signalled by a `wx.PyAssertionError` exception if
        enabled) to remove an item with the index negative or greater or equal
        than the number of items in the control.
        """
        return __core.wxItemContainer_Delete(*args, **kwargs)

    def GetClientData(*args, **kwargs):
        """
        GetClientData(self, int n) -> PyObject

        Returns the client data associated with the given item, (if any.)
        """
        return __core.wxItemContainer_GetClientData(*args, **kwargs)

    def SetClientData(*args, **kwargs):
        """
        SetClientData(self, int n, PyObject clientData)

        Associate the given client data with the item at position n.
        """
        return __core.wxItemContainer_SetClientData(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """
        GetCount(self) -> int

        Returns the number of items in the control.
        """
        return __core.wxItemContainer_GetCount(*args, **kwargs)

    def IsEmpty(*args, **kwargs):
        """
        IsEmpty(self) -> bool

        Returns True if the control is empty or False if it has some items.
        """
        return __core.wxItemContainer_IsEmpty(*args, **kwargs)

    def GetString(*args, **kwargs):
        """
        GetString(self, int n) -> wxString

        Returns the label of the item with the given index.
        """
        return __core.wxItemContainer_GetString(*args, **kwargs)

    def GetStrings(*args, **kwargs):
        """GetStrings(self) -> wxArrayString"""
        return __core.wxItemContainer_GetStrings(*args, **kwargs)

    def SetString(*args, **kwargs):
        """
        SetString(self, int n, wxString s)

        Sets the label for the given item.
        """
        return __core.wxItemContainer_SetString(*args, **kwargs)

    def FindString(*args, **kwargs):
        """
        FindString(self, wxString s) -> int

        Finds an item whose label matches the given string.  Returns the
        zero-based position of the item, or ``wx.NOT_FOUND`` if the string was not
        found.
        """
        return __core.wxItemContainer_FindString(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """
        SetSelection(self, int n)

        Sets the item at index 'n' to be the selected item.
        """
        return __core.wxItemContainer_SetSelection(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """
        GetSelection(self) -> int

        Returns the index of the selected item or ``wx.NOT_FOUND`` if no item
        is selected.
        """
        return __core.wxItemContainer_GetSelection(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """SetStringSelection(self, wxString s) -> bool"""
        return __core.wxItemContainer_SetStringSelection(*args, **kwargs)

    def GetStringSelection(*args, **kwargs):
        """
        GetStringSelection(self) -> wxString

        Returns the label of the selected item or an empty string if no item
        is selected.
        """
        return __core.wxItemContainer_GetStringSelection(*args, **kwargs)

    def Select(*args, **kwargs):
        """
        Select(self, int n)

        This is the same as `SetSelection` and exists only because it is
        slightly more natural for controls which support multiple selection.
        """
        return __core.wxItemContainer_Select(*args, **kwargs)


class wxItemContainerPtr(wxItemContainer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxItemContainer
__core.wxItemContainer_swigregister(wxItemContainerPtr)

#---------------------------------------------------------------------------

class wxControlWithItems(wxControl,wxItemContainer):
    """
    wx.ControlWithItems combines the ``wx.ItemContainer`` class with the
    wx.Control class, and is used for the base class of various controls
    that have items.
    """
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxControlWithItems instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class wxControlWithItemsPtr(wxControlWithItems):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxControlWithItems
__core.wxControlWithItems_swigregister(wxControlWithItemsPtr)

#---------------------------------------------------------------------------

class wxSizerItem(wxObject):
    """
    The wx.SizerItem class is used to track the position, size and other
    attributes of each item managed by a `wx.Sizer`. In normal usage user
    code should never need to deal directly with a wx.SizerItem, but
    custom classes derived from `wx.PySizer` will probably need to use the
    collection of wx.SizerItems held by wx.Sizer when calculating layout.

    :see: `wx.Sizer`, `wx.GBSizerItem`
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSizerItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> wxSizerItem

        Constructs an empty wx.SizerItem.  Either a window, sizer or spacer
        size will need to be set before this item can be used in a Sizer.

        You will probably never need to create a wx.SizerItem directly as they
        are created automatically when the sizer's Add, Insert or Prepend
        methods are called.

        :see: `wx.SizerItemSpacer`, `wx.SizerItemWindow`, `wx.SizerItemSizer`
        """
        newobj = __core.new_wxSizerItem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def DeleteWindows(*args, **kwargs):
        """
        DeleteWindows(self)

        Destroy the window or the windows in a subsizer, depending on the type
        of item.
        """
        return __core.wxSizerItem_DeleteWindows(*args, **kwargs)

    def DetachSizer(*args, **kwargs):
        """
        DetachSizer(self)

        Enable deleting the SizerItem without destroying the contained sizer.
        """
        return __core.wxSizerItem_DetachSizer(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> wxSize

        Get the current size of the item, as set in the last Layout.
        """
        return __core.wxSizerItem_GetSize(*args, **kwargs)

    def CalcMin(*args, **kwargs):
        """
        CalcMin(self) -> wxSize

        Calculates the minimum desired size for the item, including any space
        needed by borders.
        """
        return __core.wxSizerItem_CalcMin(*args, **kwargs)

    def SetDimension(*args, **kwargs):
        """
        SetDimension(self, wxPoint pos, wxSize size)

        Set the position and size of the space allocated for this item by the
        sizer, and adjust the position and size of the item (window or
        subsizer) to be within that space taking alignment and borders into
        account.
        """
        return __core.wxSizerItem_SetDimension(*args, **kwargs)

    def GetMinSize(*args, **kwargs):
        """
        GetMinSize(self) -> wxSize

        Get the minimum size needed for the item.
        """
        return __core.wxSizerItem_GetMinSize(*args, **kwargs)

    def GetMinSizeWithBorder(*args, **kwargs):
        """
        GetMinSizeWithBorder(self) -> wxSize

        Get the minimum size needed for the item with space for the borders
        added, if needed.
        """
        return __core.wxSizerItem_GetMinSizeWithBorder(*args, **kwargs)

    def SetInitSize(*args, **kwargs):
        """SetInitSize(self, int x, int y)"""
        return __core.wxSizerItem_SetInitSize(*args, **kwargs)

    def SetRatioWH(*args, **kwargs):
        """
        SetRatioWH(self, int width, int height)

        Set the ratio item attribute.
        """
        return __core.wxSizerItem_SetRatioWH(*args, **kwargs)

    def SetRatioSize(*args, **kwargs):
        """
        SetRatioSize(self, wxSize size)

        Set the ratio item attribute.
        """
        return __core.wxSizerItem_SetRatioSize(*args, **kwargs)

    def SetRatio(*args, **kwargs):
        """
        SetRatio(self, float ratio)

        Set the ratio item attribute.
        """
        return __core.wxSizerItem_SetRatio(*args, **kwargs)

    def GetRatio(*args, **kwargs):
        """
        GetRatio(self) -> float

        Set the ratio item attribute.
        """
        return __core.wxSizerItem_GetRatio(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """
        GetRect(self) -> wxRect

        Returns the rectangle that the sizer item should occupy
        """
        return __core.wxSizerItem_GetRect(*args, **kwargs)

    def IsWindow(*args, **kwargs):
        """
        IsWindow(self) -> bool

        Is this sizer item a window?
        """
        return __core.wxSizerItem_IsWindow(*args, **kwargs)

    def IsSizer(*args, **kwargs):
        """
        IsSizer(self) -> bool

        Is this sizer item a subsizer?
        """
        return __core.wxSizerItem_IsSizer(*args, **kwargs)

    def IsSpacer(*args, **kwargs):
        """
        IsSpacer(self) -> bool

        Is this sizer item a spacer?
        """
        return __core.wxSizerItem_IsSpacer(*args, **kwargs)

    def SetProportion(*args, **kwargs):
        """
        SetProportion(self, int proportion)

        Set the proportion value for this item.
        """
        return __core.wxSizerItem_SetProportion(*args, **kwargs)

    def GetProportion(*args, **kwargs):
        """
        GetProportion(self) -> int

        Get the proportion value for this item.
        """
        return __core.wxSizerItem_GetProportion(*args, **kwargs)

    SetOption = wx._deprecated(SetProportion, "Please use `SetProportion` instead.") 
    GetOption = wx._deprecated(GetProportion, "Please use `GetProportion` instead.") 
    def SetFlag(*args, **kwargs):
        """
        SetFlag(self, int flag)

        Set the flag value for this item.
        """
        return __core.wxSizerItem_SetFlag(*args, **kwargs)

    def GetFlag(*args, **kwargs):
        """
        GetFlag(self) -> int

        Get the flag value for this item.
        """
        return __core.wxSizerItem_GetFlag(*args, **kwargs)

    def SetBorder(*args, **kwargs):
        """
        SetBorder(self, int border)

        Set the border value for this item.
        """
        return __core.wxSizerItem_SetBorder(*args, **kwargs)

    def GetBorder(*args, **kwargs):
        """
        GetBorder(self) -> int

        Get the border value for this item.
        """
        return __core.wxSizerItem_GetBorder(*args, **kwargs)

    def GetWindow(*args, **kwargs):
        """
        GetWindow(self) -> wxWindow

        Get the window (if any) that is managed by this sizer item.
        """
        return __core.wxSizerItem_GetWindow(*args, **kwargs)

    def SetWindow(*args, **kwargs):
        """
        SetWindow(self, wxWindow window)

        Set the window to be managed by this sizer item.
        """
        return __core.wxSizerItem_SetWindow(*args, **kwargs)

    def GetSizer(*args, **kwargs):
        """
        GetSizer(self) -> wxSizer

        Get the subsizer (if any) that is managed by this sizer item.
        """
        return __core.wxSizerItem_GetSizer(*args, **kwargs)

    def SetSizer(*args, **kwargs):
        """
        SetSizer(self, wxSizer sizer)

        Set the subsizer to be managed by this sizer item.
        """
        return __core.wxSizerItem_SetSizer(*args, **kwargs)

    def GetSpacer(*args, **kwargs):
        """
        GetSpacer(self) -> wxSize

        Get the size of the spacer managed by this sizer item.
        """
        return __core.wxSizerItem_GetSpacer(*args, **kwargs)

    def SetSpacer(*args, **kwargs):
        """
        SetSpacer(self, wxSize size)

        Set the size of the spacer to be managed by this sizer item.
        """
        return __core.wxSizerItem_SetSpacer(*args, **kwargs)

    def Show(*args, **kwargs):
        """
        Show(self, bool show)

        Set the show item attribute, which sizers use to determine if the item
        is to be made part of the layout or not. If the item is tracking a
        window then it is shown or hidden as needed.
        """
        return __core.wxSizerItem_Show(*args, **kwargs)

    def IsShown(*args, **kwargs):
        """
        IsShown(self) -> bool

        Is the item to be shown in the layout?
        """
        return __core.wxSizerItem_IsShown(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> wxPoint

        Returns the current position of the item, as set in the last Layout.
        """
        return __core.wxSizerItem_GetPosition(*args, **kwargs)

    def GetUserData(*args, **kwargs):
        """
        GetUserData(self) -> PyObject

        Returns the userData associated with this sizer item, or None if there
        isn't any.
        """
        return __core.wxSizerItem_GetUserData(*args, **kwargs)


class wxSizerItemPtr(wxSizerItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxSizerItem
__core.wxSizerItem_swigregister(wxSizerItemPtr)

def SizerItemWindow(*args, **kwargs):
    """
    SizerItemWindow(wxWindow window, int proportion, int flag, int border, 
        PyObject userData=None) -> wxSizerItem

    Constructs a `wx.SizerItem` for tracking a window.
    """
    val = __core.new_SizerItemWindow(*args, **kwargs)
    val.thisown = 1
    return val

def SizerItemSpacer(*args, **kwargs):
    """
    SizerItemSpacer(int width, int height, int proportion, int flag, int border, 
        PyObject userData=None) -> wxSizerItem

    Constructs a `wx.SizerItem` for tracking a spacer.
    """
    val = __core.new_SizerItemSpacer(*args, **kwargs)
    val.thisown = 1
    return val

def SizerItemSizer(*args, **kwargs):
    """
    SizerItemSizer(wxSizer sizer, int proportion, int flag, int border, 
        PyObject userData=None) -> wxSizerItem

    Constructs a `wx.SizerItem` for tracking a subsizer
    """
    val = __core.new_SizerItemSizer(*args, **kwargs)
    val.thisown = 1
    return val

class wxSizer(wxObject):
    """
    wx.Sizer is the abstract base class used for laying out subwindows in
    a window.  You cannot use wx.Sizer directly; instead, you will have to
    use one of the sizer classes derived from it such as `wx.BoxSizer`,
    `wx.StaticBoxSizer`, `wx.NotebookSizer`, `wx.GridSizer`,  `wx.FlexGridSizer`
    and `wx.GridBagSizer`.

    The concept implemented by sizers in wxWidgets is closely related to
    layout tools in other GUI toolkits, such as Java's AWT, the GTK
    toolkit or the Qt toolkit. It is based upon the idea of the individual
    subwindows reporting their minimal required size and their ability to
    get stretched if the size of the parent window has changed. This will
    most often mean that the programmer does not set the original size of
    a dialog in the beginning, rather the dialog will assigned a sizer and
    this sizer will be queried about the recommended size. The sizer in
    turn will query its children, which can be normal windows or contorls,
    empty space or other sizers, so that a hierarchy of sizers can be
    constructed. Note that wxSizer does not derive from wxWindow and thus
    do not interfere with tab ordering and requires very little resources
    compared to a real window on screen.

    What makes sizers so well fitted for use in wxWidgets is the fact that
    every control reports its own minimal size and the algorithm can
    handle differences in font sizes or different window (dialog item)
    sizes on different platforms without problems. If for example the
    standard font as well as the overall design of Mac widgets requires
    more space than on Windows, then the initial size of a dialog using a
    sizer will automatically be bigger on Mac than on Windows.
    """
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def _setOORInfo(*args, **kwargs):
        """_setOORInfo(self, PyObject _self)"""
        return __core.wxSizer__setOORInfo(*args, **kwargs)

    def Add(*args, **kwargs):
        """
        Add(self, item, int proportion=0, int flag=0, int border=0,
            PyObject userData=None) -> wx.SizerItem

        Appends a child item to the sizer.
        """
        return __core.wxSizer_Add(*args, **kwargs)

    def Insert(*args, **kwargs):
        """
        Insert(self, int before, item, int proportion=0, int flag=0, int border=0,
            PyObject userData=None) -> wx.SizerItem

        Inserts a new item into the list of items managed by this sizer before
        the item at index *before*.  See `Add` for a description of the parameters.
        """
        return __core.wxSizer_Insert(*args, **kwargs)

    def Prepend(*args, **kwargs):
        """
        Prepend(self, item, int proportion=0, int flag=0, int border=0,
            PyObject userData=None) -> wx.SizerItem

        Adds a new item to the begining of the list of sizer items managed by
        this sizer.  See `Add` for a description of the parameters.
        """
        return __core.wxSizer_Prepend(*args, **kwargs)

    def Remove(*args, **kwargs):
        """
        Remove(self, item) -> bool

        Removes an item from the sizer and destroys it.  This method does not
        cause any layout or resizing to take place, call `Layout` to update
        the layout on screen after removing a child from the sizer.  The
        *item* parameter can be either a window, a sizer, or the zero-based
        index of an item to remove.  Returns True if the child item was found
        and removed.
        """
        return __core.wxSizer_Remove(*args, **kwargs)

    def Detach(*args, **kwargs):
        """
        Detach(self, item) -> bool

        Detaches an item from the sizer without destroying it.  This method
        does not cause any layout or resizing to take place, call `Layout` to
        do so.  The *item* parameter can be either a window, a sizer, or the
        zero-based index of the item to be detached.  Returns True if the child item
        was found and detached.
        """
        return __core.wxSizer_Detach(*args, **kwargs)

    def GetItem(*args, **kwargs):
        """
        GetItem(self, item) -> wx.SizerItem

        Returns the `wx.SizerItem` which holds the *item* given.  The *item*
        parameter can be either a window, a sizer, or the zero-based index of
        the item to be detached.
        """
        return __core.wxSizer_GetItem(*args, **kwargs)

    def _SetItemMinSize(*args, **kwargs):
        """_SetItemMinSize(self, PyObject item, wxSize size)"""
        return __core.wxSizer__SetItemMinSize(*args, **kwargs)

    def SetItemMinSize(self, item, *args):
        """
        SetItemMinSize(self, item, Size size)

        Sets the minimum size that will be allocated for an item in the sizer.
        The *item* parameter can be either a window, a sizer, or the
        zero-based index of the item.  If a window or sizer is given then it
        will be searched for recursivly in subsizers if neccessary.
        """
        if len(args) == 2:
            # for backward compatibility accept separate width,height args too
            return self._SetItemMinSize(item, args)
        else:
            return self._SetItemMinSize(item, args[0])

    def AddItem(*args, **kwargs):
        """
        AddItem(self, SizerItem item)

        Adds a `wx.SizerItem` to the sizer.
        """
        return __core.wxSizer_AddItem(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """
        InsertItem(self, int index, SizerItem item)

        Inserts a `wx.SizerItem` to the sizer at the position given by *index*.
        """
        return __core.wxSizer_InsertItem(*args, **kwargs)

    def PrependItem(*args, **kwargs):
        """
        PrependItem(self, SizerItem item)

        Prepends a `wx.SizerItem` to the sizer.
        """
        return __core.wxSizer_PrependItem(*args, **kwargs)

    def AddMany(self, items):
        """
        AddMany is a convenience method for adding several items
        to a sizer at one time.  Simply pass it a list of tuples,
        where each tuple consists of the parameters that you
        would normally pass to the `Add` method.
        """
        for item in items:
            if type(item) != type(()) or (len(item) == 2 and type(item[0]) == type(1)):
                item = (item, )
            self.Add(*item)

    # for backwards compatibility only, please do not use in new code
    def AddWindow(self, *args, **kw):
        """Compatibility alias for `Add`."""
        return self.Add(*args, **kw)
    def AddSizer(self, *args, **kw):
        """Compatibility alias for `Add`."""
        return self.Add(*args, **kw)
    def AddSpacer(self, *args, **kw):
        """Compatibility alias for `Add`."""
        return self.Add(*args, **kw)

    def PrependWindow(self, *args, **kw):
        """Compatibility alias for `Prepend`."""
        return self.Prepend(*args, **kw)
    def PrependSizer(self, *args, **kw):
        """Compatibility alias for `Prepend`."""
        return self.Prepend(*args, **kw)
    def PrependSpacer(self, *args, **kw):
        """Compatibility alias for `Prepend`."""
        return self.Prepend(*args, **kw)

    def InsertWindow(self, *args, **kw):
        """Compatibility alias for `Insert`."""
        return self.Insert(*args, **kw)
    def InsertSizer(self, *args, **kw):
        """Compatibility alias for `Insert`."""
        return self.Insert(*args, **kw)
    def InsertSpacer(self, *args, **kw):
        """Compatibility alias for `Insert`."""
        return self.Insert(*args, **kw)

    def RemoveWindow(self, *args, **kw):
        """Compatibility alias for `Remove`."""
        return self.Remove(*args, **kw)
    def RemoveSizer(self, *args, **kw):
        """Compatibility alias for `Remove`."""
        return self.Remove(*args, **kw)
    def RemovePos(self, *args, **kw):
        """Compatibility alias for `Remove`."""
        return self.Remove(*args, **kw)


    def SetDimension(*args, **kwargs):
        """
        SetDimension(self, int x, int y, int width, int height)

        Call this to force the sizer to take the given dimension and thus
        force the items owned by the sizer to resize themselves according to
        the rules defined by the parameter in the `Add`, `Insert` or `Prepend`
        methods.
        """
        return __core.wxSizer_SetDimension(*args, **kwargs)

    def SetMinSize(*args, **kwargs):
        """
        SetMinSize(self, wxSize size)

        Call this to give the sizer a minimal size. Normally, the sizer will
        calculate its minimal size based purely on how much space its children
        need. After calling this method `GetMinSize` will return either the
        minimal size as requested by its children or the minimal size set
        here, depending on which is bigger.
        """
        return __core.wxSizer_SetMinSize(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> wxSize

        Returns the current size of the space managed by the sizer.
        """
        return __core.wxSizer_GetSize(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> wxPoint

        Returns the current position of the sizer's managed space.
        """
        return __core.wxSizer_GetPosition(*args, **kwargs)

    def GetMinSize(*args, **kwargs):
        """
        GetMinSize(self) -> wxSize

        Returns the minimal size of the sizer. This is either the combined
        minimal size of all the children and their borders or the minimal size
        set by SetMinSize, depending on which is bigger.
        """
        return __core.wxSizer_GetMinSize(*args, **kwargs)

    def GetSizeTuple(self):
        return self.GetSize().Get()
    def GetPositionTuple(self):
        return self.GetPosition().Get()
    def GetMinSizeTuple(self):
        return self.GetMinSize().Get()

    def RecalcSizes(*args, **kwargs):
        """
        RecalcSizes(self)

        Using the sizes calculated by `CalcMin` reposition and resize all the
        items managed by this sizer.  You should not need to call this directly as
        it is called by `Layout`.
        """
        return __core.wxSizer_RecalcSizes(*args, **kwargs)

    def CalcMin(*args, **kwargs):
        """
        CalcMin(self) -> wxSize

        This method is where the sizer will do the actual calculation of its
        children's minimal sizes.  You should not need to call this directly as
        it is called by `Layout`.
        """
        return __core.wxSizer_CalcMin(*args, **kwargs)

    def Layout(*args, **kwargs):
        """
        Layout(self)

        This method will force the recalculation and layout of the items
        controlled by the sizer using the current space allocated to the
        sizer.  Normally this is called automatically from the owning window's
        EVT_SIZE handler, but it is also useful to call it from user code when
        one of the items in a sizer change size, or items are added or
        removed.
        """
        return __core.wxSizer_Layout(*args, **kwargs)

    def Fit(*args, **kwargs):
        """
        Fit(self, wxWindow window) -> wxSize

        Tell the sizer to resize the *window* to match the sizer's minimal
        size. This is commonly done in the constructor of the window itself in
        order to set its initial size to match the needs of the children as
        determined by the sizer.  Returns the new size.

        For a top level window this is the total window size, not the client size.
        """
        return __core.wxSizer_Fit(*args, **kwargs)

    def FitInside(*args, **kwargs):
        """
        FitInside(self, wxWindow window)

        Tell the sizer to resize the *virtual size* of the *window* to match the
        sizer's minimal size. This will not alter the on screen size of the
        window, but may cause the addition/removal/alteration of scrollbars
        required to view the virtual area in windows which manage it.

        :see: `wx.ScrolledWindow.SetScrollbars`, `SetVirtualSizeHints`

        """
        return __core.wxSizer_FitInside(*args, **kwargs)

    def SetSizeHints(*args, **kwargs):
        """
        SetSizeHints(self, wxWindow window)

        Tell the sizer to set (and `Fit`) the minimal size of the *window* to
        match the sizer's minimal size. This is commonly done in the
        constructor of the window itself if the window is resizable (as are
        many dialogs under Unix and frames on probably all platforms) in order
        to prevent the window from being sized smaller than the minimal size
        required by the sizer.
        """
        return __core.wxSizer_SetSizeHints(*args, **kwargs)

    def SetVirtualSizeHints(*args, **kwargs):
        """
        SetVirtualSizeHints(self, wxWindow window)

        Tell the sizer to set the minimal size of the window virtual area to
        match the sizer's minimal size. For windows with managed scrollbars
        this will set them appropriately.

        :see: `wx.ScrolledWindow.SetScrollbars`

        """
        return __core.wxSizer_SetVirtualSizeHints(*args, **kwargs)

    def Clear(*args, **kwargs):
        """
        Clear(self, bool deleteWindows=False)

        Clear all items from the sizer, optionally destroying the window items
        as well.
        """
        return __core.wxSizer_Clear(*args, **kwargs)

    def DeleteWindows(*args, **kwargs):
        """
        DeleteWindows(self)

        Destroy all windows managed by the sizer.
        """
        return __core.wxSizer_DeleteWindows(*args, **kwargs)

    def GetChildren(*args, **kwargs):
        """
        GetChildren(self) -> list

        Returns a list of all the `wx.SizerItem` objects managed by the sizer.
        """
        return __core.wxSizer_GetChildren(*args, **kwargs)

    def Show(*args, **kwargs):
        """
        Show(self, item, bool show=True, bool recursive=false) -> bool

        Shows or hides an item managed by the sizer.  To make a sizer item
        disappear or reappear, use Show followed by `Layout`.  The *item*
        parameter can be either a window, a sizer, or the zero-based index of
        the item.  Use the recursive parameter to show or hide an item in a
        subsizer.  Returns True if the item was found.
        """
        return __core.wxSizer_Show(*args, **kwargs)

    def IsShown(*args, **kwargs):
        """
        IsShown(self, item)

        Determines if the item is currently shown. sizer.  To make a sizer
        item disappear or reappear, use Show followed by `Layout`.  The *item*
        parameter can be either a window, a sizer, or the zero-based index of
        the item.
        """
        return __core.wxSizer_IsShown(*args, **kwargs)

    def Hide(self, item, recursive=False):
        """
        A convenience method for `Show`(item, False, recursive).
        """
        return self.Show(item, False, recursive)

    def ShowItems(*args, **kwargs):
        """
        ShowItems(self, bool show)

        Recursively call `wx.SizerItem.Show` on all sizer items.
        """
        return __core.wxSizer_ShowItems(*args, **kwargs)


class wxSizerPtr(wxSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxSizer
__core.wxSizer_swigregister(wxSizerPtr)

class wxPySizer(wxSizer):
    """
    wx.PySizer is a special version of `wx.Sizer` that has been
    instrumented to allow the C++ virtual methods to be overloaded in
    Python derived classes.  You would derive from this class if you are
    wanting to implement a custom sizer in Python code.  Simply implement
    `CalcMin` and `RecalcSizes` in the derived class and you're all set.
    For example::

        class MySizer(wx.PySizer):
             def __init__(self):
                 wx.PySizer.__init__(self)

             def CalcMin(self):
                 for item in self.GetChildren():
                      # calculate the total minimum width and height needed
                      # by all items in the sizer according to this sizer's
                      # layout algorithm.
                      ...
                 return wx.Size(width, height)

              def RecalcSizes(self):
                  # find the space allotted to this sizer
                  pos = self.GetPosition()
                  size = self.GetSize()
                  for item in self.GetChildren():
                      # Recalculate (if necessary) the position and size of
                      # each item and then call item.SetDimension to do the
                      # actual positioning and sizing of the items within the
                      # space alloted to this sizer.
                      ...
                      item.SetDimension(itemPos, itemSize)


    When `Layout` is called it first calls `CalcMin` followed by
    `RecalcSizes` so you can optimize a bit by saving the results of
    `CalcMin` and reusing them in `RecalcSizes`.

    :see: `wx.SizerItem`, `wx.Sizer.GetChildren`


    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPySizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> wxPySizer

        Creates a wx.PySizer.  Must be called from the __init__ in the derived
        class.
        """
        newobj = __core.new_wxPySizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PySizer);self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return __core.wxPySizer__setCallbackInfo(*args, **kwargs)


class wxPySizerPtr(wxPySizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPySizer
__core.wxPySizer_swigregister(wxPySizerPtr)

#---------------------------------------------------------------------------

class wxBoxSizer(wxSizer):
    """
    The basic idea behind a box sizer is that windows will most often be
    laid out in rather simple basic geometry, typically in a row or a
    column or nested hierarchies of either.  A wx.BoxSizer will lay out
    its items in a simple row or column, depending on the orientation
    parameter passed to the constructor.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBoxSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int orient=wxHORIZONTAL) -> wxBoxSizer

        Constructor for a wx.BoxSizer. *orient* may be one of ``wx.VERTICAL``
        or ``wx.HORIZONTAL`` for creating either a column sizer or a row
        sizer.
        """
        newobj = __core.new_wxBoxSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetOrientation(*args, **kwargs):
        """
        GetOrientation(self) -> int

        Returns the current orientation of the sizer.
        """
        return __core.wxBoxSizer_GetOrientation(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """
        SetOrientation(self, int orient)

        Resets the orientation of the sizer.
        """
        return __core.wxBoxSizer_SetOrientation(*args, **kwargs)


class wxBoxSizerPtr(wxBoxSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxBoxSizer
__core.wxBoxSizer_swigregister(wxBoxSizerPtr)

#---------------------------------------------------------------------------

class wxStaticBoxSizer(wxBoxSizer):
    """
    wx.StaticBoxSizer derives from and functions identically to the
    `wx.BoxSizer` and adds a `wx.StaticBox` around the items that the sizer
    manages.  Note that this static box must be created separately and
    passed to the sizer constructor.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticBoxSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxStaticBox box, int orient=wxHORIZONTAL) -> wxStaticBoxSizer

        Constructor. It takes an associated static box and the orientation
        *orient* as parameters - orient can be either of ``wx.VERTICAL`` or
        ``wx.HORIZONTAL``.
        """
        newobj = __core.new_wxStaticBoxSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetStaticBox(*args, **kwargs):
        """
        GetStaticBox(self) -> wxStaticBox

        Returns the static box associated with this sizer.
        """
        return __core.wxStaticBoxSizer_GetStaticBox(*args, **kwargs)


class wxStaticBoxSizerPtr(wxStaticBoxSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxStaticBoxSizer
__core.wxStaticBoxSizer_swigregister(wxStaticBoxSizerPtr)

#---------------------------------------------------------------------------

class wxGridSizer(wxSizer):
    """
    A grid sizer is a sizer which lays out its children in a
    two-dimensional table with all cells having the same size.  In other
    words, the width of each cell within the grid is the width of the
    widest item added to the sizer and the height of each grid cell is the
    height of the tallest item.  An optional vertical and/or horizontal
    gap between items can also be specified (in pixels.)

    Items are placed in the cells of the grid in the order they are added,
    in row-major order.  In other words, the first row is filled first,
    then the second, and so on until all items have been added. (If
    neccessary, additional rows will be added as items are added.)  If you
    need to have greater control over the cells that items are placed in
    then use the `wx.GridBagSizer`.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int rows=1, int cols=0, int vgap=0, int hgap=0) -> wxGridSizer

        Constructor for a wx.GridSizer. *rows* and *cols* determine the number
        of columns and rows in the sizer - if either of the parameters is
        zero, it will be calculated to from the total number of children in
        the sizer, thus making the sizer grow dynamically. *vgap* and *hgap*
        define extra space between all children.
        """
        newobj = __core.new_wxGridSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def SetCols(*args, **kwargs):
        """
        SetCols(self, int cols)

        Sets the number of columns in the sizer.
        """
        return __core.wxGridSizer_SetCols(*args, **kwargs)

    def SetRows(*args, **kwargs):
        """
        SetRows(self, int rows)

        Sets the number of rows in the sizer.
        """
        return __core.wxGridSizer_SetRows(*args, **kwargs)

    def SetVGap(*args, **kwargs):
        """
        SetVGap(self, int gap)

        Sets the vertical gap (in pixels) between the cells in the sizer.
        """
        return __core.wxGridSizer_SetVGap(*args, **kwargs)

    def SetHGap(*args, **kwargs):
        """
        SetHGap(self, int gap)

        Sets the horizontal gap (in pixels) between cells in the sizer
        """
        return __core.wxGridSizer_SetHGap(*args, **kwargs)

    def GetCols(*args, **kwargs):
        """
        GetCols(self) -> int

        Returns the number of columns in the sizer.
        """
        return __core.wxGridSizer_GetCols(*args, **kwargs)

    def GetRows(*args, **kwargs):
        """
        GetRows(self) -> int

        Returns the number of rows in the sizer.
        """
        return __core.wxGridSizer_GetRows(*args, **kwargs)

    def GetVGap(*args, **kwargs):
        """
        GetVGap(self) -> int

        Returns the vertical gap (in pixels) between the cells in the sizer.
        """
        return __core.wxGridSizer_GetVGap(*args, **kwargs)

    def GetHGap(*args, **kwargs):
        """
        GetHGap(self) -> int

        Returns the horizontal gap (in pixels) between cells in the sizer.
        """
        return __core.wxGridSizer_GetHGap(*args, **kwargs)


class wxGridSizerPtr(wxGridSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxGridSizer
__core.wxGridSizer_swigregister(wxGridSizerPtr)

#---------------------------------------------------------------------------

wxFLEX_GROWMODE_NONE = __core.wxFLEX_GROWMODE_NONE
wxFLEX_GROWMODE_SPECIFIED = __core.wxFLEX_GROWMODE_SPECIFIED
wxFLEX_GROWMODE_ALL = __core.wxFLEX_GROWMODE_ALL
class wxFlexGridSizer(wxGridSizer):
    """
    A flex grid sizer is a sizer which lays out its children in a
    two-dimensional table with all table cells in one row having the same
    height and all cells in one column having the same width, but all
    rows or all columns are not necessarily the same height or width as in
    the `wx.GridSizer`.

    wx.FlexGridSizer can also size items equally in one direction but
    unequally ("flexibly") in the other. If the sizer is only flexible
    in one direction (this can be changed using `SetFlexibleDirection`), it
    needs to be decided how the sizer should grow in the other ("non
    flexible") direction in order to fill the available space. The
    `SetNonFlexibleGrowMode` method serves this purpose.


    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFlexGridSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int rows=1, int cols=0, int vgap=0, int hgap=0) -> wxFlexGridSizer

        Constructor for a wx.FlexGridSizer. *rows* and *cols* determine the
        number of columns and rows in the sizer - if either of the parameters
        is zero, it will be calculated to from the total number of children in
        the sizer, thus making the sizer grow dynamically. *vgap* and *hgap*
        define extra space between all children.
        """
        newobj = __core.new_wxFlexGridSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def AddGrowableRow(*args, **kwargs):
        """
        AddGrowableRow(self, size_t idx, int proportion=0)

        Specifies that row *idx* (starting from zero) should be grown if there
        is extra space available to the sizer.

        The *proportion* parameter has the same meaning as the stretch factor
        for the box sizers except that if all proportions are 0, then all
        columns are resized equally (instead of not being resized at all).
        """
        return __core.wxFlexGridSizer_AddGrowableRow(*args, **kwargs)

    def RemoveGrowableRow(*args, **kwargs):
        """
        RemoveGrowableRow(self, size_t idx)

        Specifies that row *idx* is no longer growable.
        """
        return __core.wxFlexGridSizer_RemoveGrowableRow(*args, **kwargs)

    def AddGrowableCol(*args, **kwargs):
        """
        AddGrowableCol(self, size_t idx, int proportion=0)

        Specifies that column *idx* (starting from zero) should be grown if
        there is extra space available to the sizer.

        The *proportion* parameter has the same meaning as the stretch factor
        for the box sizers except that if all proportions are 0, then all
        columns are resized equally (instead of not being resized at all).
        """
        return __core.wxFlexGridSizer_AddGrowableCol(*args, **kwargs)

    def RemoveGrowableCol(*args, **kwargs):
        """
        RemoveGrowableCol(self, size_t idx)

        Specifies that column *idx* is no longer growable.
        """
        return __core.wxFlexGridSizer_RemoveGrowableCol(*args, **kwargs)

    def SetFlexibleDirection(*args, **kwargs):
        """
        SetFlexibleDirection(self, int direction)

        Specifies whether the sizer should flexibly resize its columns, rows,
        or both. Argument *direction* can be one of the following values.  Any
        other value is ignored.

            ==============    =======================================
            wx.VERTICAL       Rows are flexibly sized.
            wx.HORIZONTAL     Columns are flexibly sized.
            wx.BOTH           Both rows and columns are flexibly sized
                              (this is the default value).
            ==============    =======================================

        Note that this method does not trigger relayout.

        """
        return __core.wxFlexGridSizer_SetFlexibleDirection(*args, **kwargs)

    def GetFlexibleDirection(*args, **kwargs):
        """
        GetFlexibleDirection(self) -> int

        Returns a value that specifies whether the sizer
        flexibly resizes its columns, rows, or both (default).

        :see: `SetFlexibleDirection`
        """
        return __core.wxFlexGridSizer_GetFlexibleDirection(*args, **kwargs)

    def SetNonFlexibleGrowMode(*args, **kwargs):
        """
        SetNonFlexibleGrowMode(self, int mode)

        Specifies how the sizer should grow in the non-flexible direction if
        there is one (so `SetFlexibleDirection` must have been called
        previously). Argument *mode* can be one of the following values:

            ==========================  =================================================
            wx.FLEX_GROWMODE_NONE       Sizer doesn't grow in the non flexible direction.
            wx.FLEX_GROWMODE_SPECIFIED  Sizer honors growable columns/rows set with
                                        `AddGrowableCol` and `AddGrowableRow`. In this
                                        case equal sizing applies to minimum sizes of
                                        columns or rows (this is the default value).
            wx.FLEX_GROWMODE_ALL        Sizer equally stretches all columns or rows in
                                        the non flexible direction, whether they are
                                        growable or not in the flexbile direction.
            ==========================  =================================================

        Note that this method does not trigger relayout.


        """
        return __core.wxFlexGridSizer_SetNonFlexibleGrowMode(*args, **kwargs)

    def GetNonFlexibleGrowMode(*args, **kwargs):
        """
        GetNonFlexibleGrowMode(self) -> int

        Returns the value that specifies how the sizer grows in the
        non-flexible direction if there is one.

        :see: `SetNonFlexibleGrowMode`
        """
        return __core.wxFlexGridSizer_GetNonFlexibleGrowMode(*args, **kwargs)

    def GetRowHeights(*args, **kwargs):
        """
        GetRowHeights(self) -> list

        Returns a list of integers representing the heights of each of the
        rows in the sizer.
        """
        return __core.wxFlexGridSizer_GetRowHeights(*args, **kwargs)

    def GetColWidths(*args, **kwargs):
        """
        GetColWidths(self) -> list

        Returns a list of integers representing the widths of each of the
        columns in the sizer.
        """
        return __core.wxFlexGridSizer_GetColWidths(*args, **kwargs)


class wxFlexGridSizerPtr(wxFlexGridSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxFlexGridSizer
__core.wxFlexGridSizer_swigregister(wxFlexGridSizerPtr)

class wxStdDialogButtonSizer(wxBoxSizer):
    """
    A special sizer that knows how to order and position standard buttons
    in order to conform to the current platform's standards.  You simply
    need to add each `wx.Button` to the sizer, and be sure to create the
    buttons using the standard ID's.  Then call `Realize` and the sizer
    will take care of the rest.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStdDialogButtonSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxStdDialogButtonSizer"""
        newobj = __core.new_wxStdDialogButtonSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def AddButton(*args, **kwargs):
        """
        AddButton(self, wxButton button)

        Use this to add the buttons to this sizer.  Do not use the `Add`
        method in the base class.
        """
        return __core.wxStdDialogButtonSizer_AddButton(*args, **kwargs)

    def Realize(*args, **kwargs):
        """
        Realize(self)

        This funciton needs to be called after all the buttons have been added
        to the sizer.  It will reorder them and position them in a platform
        specifc manner.
        """
        return __core.wxStdDialogButtonSizer_Realize(*args, **kwargs)

    def SetAffirmativeButton(*args, **kwargs):
        """SetAffirmativeButton(self, wxButton button)"""
        return __core.wxStdDialogButtonSizer_SetAffirmativeButton(*args, **kwargs)

    def SetNegativeButton(*args, **kwargs):
        """SetNegativeButton(self, wxButton button)"""
        return __core.wxStdDialogButtonSizer_SetNegativeButton(*args, **kwargs)

    def SetCancelButton(*args, **kwargs):
        """SetCancelButton(self, wxButton button)"""
        return __core.wxStdDialogButtonSizer_SetCancelButton(*args, **kwargs)

    def GetAffirmativeButton(*args, **kwargs):
        """GetAffirmativeButton(self) -> wxButton"""
        return __core.wxStdDialogButtonSizer_GetAffirmativeButton(*args, **kwargs)

    def GetApplyButton(*args, **kwargs):
        """GetApplyButton(self) -> wxButton"""
        return __core.wxStdDialogButtonSizer_GetApplyButton(*args, **kwargs)

    def GetNegativeButton(*args, **kwargs):
        """GetNegativeButton(self) -> wxButton"""
        return __core.wxStdDialogButtonSizer_GetNegativeButton(*args, **kwargs)

    def GetCancelButton(*args, **kwargs):
        """GetCancelButton(self) -> wxButton"""
        return __core.wxStdDialogButtonSizer_GetCancelButton(*args, **kwargs)

    def GetHelpButton(*args, **kwargs):
        """GetHelpButton(self) -> wxButton"""
        return __core.wxStdDialogButtonSizer_GetHelpButton(*args, **kwargs)


class wxStdDialogButtonSizerPtr(wxStdDialogButtonSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxStdDialogButtonSizer
__core.wxStdDialogButtonSizer_swigregister(wxStdDialogButtonSizerPtr)

#---------------------------------------------------------------------------

class wxGBPosition(object):
    """
    This class represents the position of an item in a virtual grid of
    rows and columns managed by a `wx.GridBagSizer`.  wxPython has
    typemaps that will automatically convert from a 2-element sequence of
    integers to a wx.GBPosition, so you can use the more pythonic
    representation of the position nearly transparently in Python code.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGBPosition instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int row=0, int col=0) -> wxGBPosition

        This class represents the position of an item in a virtual grid of
        rows and columns managed by a `wx.GridBagSizer`.  wxPython has
        typemaps that will automatically convert from a 2-element sequence of
        integers to a wx.GBPosition, so you can use the more pythonic
        representation of the position nearly transparently in Python code.
        """
        newobj = __core.new_wxGBPosition(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRow(*args, **kwargs):
        """GetRow(self) -> int"""
        return __core.wxGBPosition_GetRow(*args, **kwargs)

    def GetCol(*args, **kwargs):
        """GetCol(self) -> int"""
        return __core.wxGBPosition_GetCol(*args, **kwargs)

    def SetRow(*args, **kwargs):
        """SetRow(self, int row)"""
        return __core.wxGBPosition_SetRow(*args, **kwargs)

    def SetCol(*args, **kwargs):
        """SetCol(self, int col)"""
        return __core.wxGBPosition_SetCol(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, wxGBPosition other) -> bool"""
        return __core.wxGBPosition___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, wxGBPosition other) -> bool"""
        return __core.wxGBPosition___ne__(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(self, int row=0, int col=0)"""
        return __core.wxGBPosition_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """Get(self) -> PyObject"""
        return __core.wxGBPosition_Get(*args, **kwargs)

    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.GBPosition'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.SetRow(val)
        elif index == 1: self.SetCol(val)
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.GBPosition, self.Get())

    row = property(GetRow, SetRow)
    col = property(GetCol, SetCol)


class wxGBPositionPtr(wxGBPosition):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxGBPosition
__core.wxGBPosition_swigregister(wxGBPositionPtr)

class wxGBSpan(object):
    """
    This class is used to hold the row and column spanning attributes of
    items in a `wx.GridBagSizer`.  wxPython has typemaps that will
    automatically convert from a 2-element sequence of integers to a
    wx.GBSpan, so you can use the more pythonic representation of the span
    nearly transparently in Python code.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGBSpan instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int rowspan=1, int colspan=1) -> wxGBSpan

        Construct a new wxGBSpan, optionally setting the rowspan and
        colspan. The default is (1,1). (Meaning that the item occupies one
        cell in each direction.
        """
        newobj = __core.new_wxGBSpan(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRowspan(*args, **kwargs):
        """GetRowspan(self) -> int"""
        return __core.wxGBSpan_GetRowspan(*args, **kwargs)

    def GetColspan(*args, **kwargs):
        """GetColspan(self) -> int"""
        return __core.wxGBSpan_GetColspan(*args, **kwargs)

    def SetRowspan(*args, **kwargs):
        """SetRowspan(self, int rowspan)"""
        return __core.wxGBSpan_SetRowspan(*args, **kwargs)

    def SetColspan(*args, **kwargs):
        """SetColspan(self, int colspan)"""
        return __core.wxGBSpan_SetColspan(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, wxGBSpan other) -> bool"""
        return __core.wxGBSpan___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, wxGBSpan other) -> bool"""
        return __core.wxGBSpan___ne__(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(self, int rowspan=1, int colspan=1)"""
        return __core.wxGBSpan_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """Get(self) -> PyObject"""
        return __core.wxGBSpan_Get(*args, **kwargs)

    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.GBSpan'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.SetRowspan(val)
        elif index == 1: self.SetColspan(val)
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.GBSpan, self.Get())

    rowspan = property(GetRowspan, SetRowspan)
    colspan = property(GetColspan, SetColspan)


class wxGBSpanPtr(wxGBSpan):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxGBSpan
__core.wxGBSpan_swigregister(wxGBSpanPtr)

class wxGBSizerItem(wxSizerItem):
    """
    The wx.GBSizerItem class is used to track the additional data about
    items in a `wx.GridBagSizer` such as the item's position in the grid
    and how many rows or columns it spans.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGBSizerItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> wxGBSizerItem

        Constructs an empty wx.GBSizerItem.  Either a window, sizer or spacer
        size will need to be set, as well as a position and span before this
        item can be used in a Sizer.

        You will probably never need to create a wx.GBSizerItem directly as they
        are created automatically when the sizer's Add method is called.
        """
        newobj = __core.new_wxGBSizerItem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPos(*args, **kwargs):
        """
        GetPos(self) -> wxGBPosition

        Get the grid position of the item
        """
        return __core.wxGBSizerItem_GetPos(*args, **kwargs)

    def GetPosTuple(self): return self.GetPos().Get() 
    def GetSpan(*args, **kwargs):
        """
        GetSpan(self) -> wxGBSpan

        Get the row and column spanning of the item
        """
        return __core.wxGBSizerItem_GetSpan(*args, **kwargs)

    def GetSpanTuple(self): return self.GetSpan().Get() 
    def SetPos(*args, **kwargs):
        """
        SetPos(self, wxGBPosition pos) -> bool

        If the item is already a member of a sizer then first ensure that
        there is no other item that would intersect with this one at the new
        position, then set the new position.  Returns True if the change is
        successful and after the next Layout() the item will be moved.
        """
        return __core.wxGBSizerItem_SetPos(*args, **kwargs)

    def SetSpan(*args, **kwargs):
        """
        SetSpan(self, wxGBSpan span) -> bool

        If the item is already a member of a sizer then first ensure that
        there is no other item that would intersect with this one with its new
        spanning size, then set the new spanning.  Returns True if the change
        is successful and after the next Layout() the item will be resized.

        """
        return __core.wxGBSizerItem_SetSpan(*args, **kwargs)

    def Intersects(*args, **kwargs):
        """
        Intersects(self, wxGBSizerItem other) -> bool

        Returns True if this item and the other item instersect.
        """
        return __core.wxGBSizerItem_Intersects(*args, **kwargs)

    def IntersectsPos(*args, **kwargs):
        """
        IntersectsPos(self, wxGBPosition pos, wxGBSpan span) -> bool

        Returns True if the given pos/span would intersect with this item.
        """
        return __core.wxGBSizerItem_IntersectsPos(*args, **kwargs)

    def GetEndPos(*args, **kwargs):
        """
        GetEndPos(self) -> wxGBPosition

        Get the row and column of the endpoint of this item.
        """
        return __core.wxGBSizerItem_GetEndPos(*args, **kwargs)

    def GetGBSizer(*args, **kwargs):
        """
        GetGBSizer(self) -> wxGridBagSizer

        Get the sizer this item is a member of.
        """
        return __core.wxGBSizerItem_GetGBSizer(*args, **kwargs)

    def SetGBSizer(*args, **kwargs):
        """
        SetGBSizer(self, wxGridBagSizer sizer)

        Set the sizer this item is a member of.
        """
        return __core.wxGBSizerItem_SetGBSizer(*args, **kwargs)


class wxGBSizerItemPtr(wxGBSizerItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxGBSizerItem
__core.wxGBSizerItem_swigregister(wxGBSizerItemPtr)
wxDefaultSpan = cvar.wxDefaultSpan

def GBSizerItemWindow(*args, **kwargs):
    """
    GBSizerItemWindow(wxWindow window, wxGBPosition pos, wxGBSpan span, int flag, 
        int border, PyObject userData=None) -> wxGBSizerItem

    Construct a `wx.GBSizerItem` for a window.
    """
    val = __core.new_GBSizerItemWindow(*args, **kwargs)
    val.thisown = 1
    return val

def GBSizerItemSizer(*args, **kwargs):
    """
    GBSizerItemSizer(wxSizer sizer, wxGBPosition pos, wxGBSpan span, int flag, 
        int border, PyObject userData=None) -> wxGBSizerItem

    Construct a `wx.GBSizerItem` for a sizer
    """
    val = __core.new_GBSizerItemSizer(*args, **kwargs)
    val.thisown = 1
    return val

def GBSizerItemSpacer(*args, **kwargs):
    """
    GBSizerItemSpacer(int width, int height, wxGBPosition pos, wxGBSpan span, 
        int flag, int border, PyObject userData=None) -> wxGBSizerItem

    Construct a `wx.GBSizerItem` for a spacer.
    """
    val = __core.new_GBSizerItemSpacer(*args, **kwargs)
    val.thisown = 1
    return val

class wxGridBagSizer(wxFlexGridSizer):
    """
    A `wx.Sizer` that can lay out items in a virtual grid like a
    `wx.FlexGridSizer` but in this case explicit positioning of the items
    is allowed using `wx.GBPosition`, and items can optionally span more
    than one row and/or column using `wx.GBSpan`.  The total size of the
    virtual grid is determined by the largest row and column that items are
    positioned at, adjusted for spanning.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridBagSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int vgap=0, int hgap=0) -> wxGridBagSizer

        Constructor, with optional parameters to specify the gap between the
        rows and columns.
        """
        newobj = __core.new_wxGridBagSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Add(*args, **kwargs):
        """
        Add(self, item, GBPosition pos, GBSpan span=DefaultSpan, int flag=0,
        int border=0, userData=None) -> wx.GBSizerItem

        Adds an item to the sizer at the grid cell *pos*, optionally spanning
        more than one row or column as specified with *span*.  The remaining
        args behave similarly to `wx.Sizer.Add`.

        Returns True if the item was successfully placed at the given cell
        position, False if something was already there.

        """
        return __core.wxGridBagSizer_Add(*args, **kwargs)

    def AddItem(*args, **kwargs):
        """
        Add(self, GBSizerItem item) -> wx.GBSizerItem

        Add an item to the sizer using a `wx.GBSizerItem`.  Returns True if
        the item was successfully placed at its given cell position, False if
        something was already there.
        """
        return __core.wxGridBagSizer_AddItem(*args, **kwargs)

    def GetCellSize(*args, **kwargs):
        """
        GetCellSize(self, int row, int col) -> wxSize

        Get the size of the specified cell, including hgap and
        vgap.  Only valid after a Layout.
        """
        return __core.wxGridBagSizer_GetCellSize(*args, **kwargs)

    def GetEmptyCellSize(*args, **kwargs):
        """
        GetEmptyCellSize(self) -> wxSize

        Get the size used for cells in the grid with no item.
        """
        return __core.wxGridBagSizer_GetEmptyCellSize(*args, **kwargs)

    def SetEmptyCellSize(*args, **kwargs):
        """
        SetEmptyCellSize(self, wxSize sz)

        Set the size used for cells in the grid with no item.
        """
        return __core.wxGridBagSizer_SetEmptyCellSize(*args, **kwargs)

    def GetItemPosition(*args):
        """
        GetItemPosition(self, item) -> GBPosition

        Get the grid position of the specified *item* where *item* is either a
        window or subsizer that is a member of this sizer, or a zero-based
        index of an item.
        """
        return __core.wxGridBagSizer_GetItemPosition(*args)

    def SetItemPosition(*args):
        """
        SetItemPosition(self, item, GBPosition pos) -> bool

        Set the grid position of the specified *item* where *item* is either a
        window or subsizer that is a member of this sizer, or a zero-based
        index of an item.  Returns True on success.  If the move is not
        allowed (because an item is already there) then False is returned.

        """
        return __core.wxGridBagSizer_SetItemPosition(*args)

    def GetItemSpan(*args):
        """
        GetItemSpan(self, item) -> GBSpan

        Get the row/col spanning of the specified *item* where *item* is
        either a window or subsizer that is a member of this sizer, or a
        zero-based index of an item.
        """
        return __core.wxGridBagSizer_GetItemSpan(*args)

    def SetItemSpan(*args):
        """
        SetItemSpan(self, item, GBSpan span) -> bool

        Set the row/col spanning of the specified *item* where *item* is
        either a window or subsizer that is a member of this sizer, or a
        zero-based index of an item.  Returns True on success.  If the move is
        not allowed (because an item is already there) then False is returned.
        """
        return __core.wxGridBagSizer_SetItemSpan(*args)

    def FindItem(*args):
        """
        FindItem(self, item) -> GBSizerItem

        Find the sizer item for the given window or subsizer, returns None if
        not found. (non-recursive)
        """
        return __core.wxGridBagSizer_FindItem(*args)

    def FindItemAtPosition(*args, **kwargs):
        """
        FindItemAtPosition(self, wxGBPosition pos) -> wxGBSizerItem

        Return the sizer item for the given grid cell, or None if there is no
        item at that position. (non-recursive)
        """
        return __core.wxGridBagSizer_FindItemAtPosition(*args, **kwargs)

    def FindItemAtPoint(*args, **kwargs):
        """
        FindItemAtPoint(self, wxPoint pt) -> wxGBSizerItem

        Return the sizer item located at the point given in *pt*, or None if
        there is no item at that point. The (x,y) coordinates in pt correspond
        to the client coordinates of the window using the sizer for
        layout. (non-recursive)
        """
        return __core.wxGridBagSizer_FindItemAtPoint(*args, **kwargs)

    def CheckForIntersection(*args, **kwargs):
        """
        CheckForIntersection(self, wxGBSizerItem item, wxGBSizerItem excludeItem=None) -> bool

        Look at all items and see if any intersect (or would overlap) the
        given *item*.  Returns True if so, False if there would be no overlap.
        If an *excludeItem* is given then it will not be checked for
        intersection, for example it may be the item we are checking the
        position of.

        """
        return __core.wxGridBagSizer_CheckForIntersection(*args, **kwargs)

    def CheckForIntersectionPos(*args, **kwargs):
        """
        CheckForIntersectionPos(self, wxGBPosition pos, wxGBSpan span, wxGBSizerItem excludeItem=None) -> bool

        Look at all items and see if any intersect (or would overlap) the
        given position and span.  Returns True if so, False if there would be
        no overlap.  If an *excludeItem* is given then it will not be checked
        for intersection, for example it may be the item we are checking the
        position of.
        """
        return __core.wxGridBagSizer_CheckForIntersectionPos(*args, **kwargs)


class wxGridBagSizerPtr(wxGridBagSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxGridBagSizer
__core.wxGridBagSizer_swigregister(wxGridBagSizerPtr)

#---------------------------------------------------------------------------

wxLeft = __core.wxLeft
wxTop = __core.wxTop
wxRight = __core.wxRight
wxBottom = __core.wxBottom
wxWidth = __core.wxWidth
wxHeight = __core.wxHeight
wxCentre = __core.wxCentre
wxCenter = __core.wxCenter
wxCentreX = __core.wxCentreX
wxCentreY = __core.wxCentreY
wxUnconstrained = __core.wxUnconstrained
wxAsIs = __core.wxAsIs
wxPercentOf = __core.wxPercentOf
wxAbove = __core.wxAbove
wxBelow = __core.wxBelow
wxLeftOf = __core.wxLeftOf
wxRightOf = __core.wxRightOf
wxSameAs = __core.wxSameAs
wxAbsolute = __core.wxAbsolute
class wxIndividualLayoutConstraint(wxObject):
    """
    Objects of this class are stored in the `wx.LayoutConstraints` class as
    one of eight possible constraints that a window can be involved in.
    You will never need to create an instance of
    wx.IndividualLayoutConstraint, rather you should create a
    `wx.LayoutConstraints` instance and use the individual contstraints
    that it contains.
    """
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIndividualLayoutConstraint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def Set(*args, **kwargs):
        """
        Set(self, int rel, wxWindow otherW, int otherE, int val=0, int marg=wxLAYOUT_DEFAULT_MARGIN)

        Sets the properties of the constraint. Normally called by one of the
        convenience functions such as Above, RightOf, SameAs.
        """
        return __core.wxIndividualLayoutConstraint_Set(*args, **kwargs)

    def LeftOf(*args, **kwargs):
        """
        LeftOf(self, wxWindow sibling, int marg=0)

        Constrains this edge to be to the left of the given window, with an
        optional margin. Implicitly, this is relative to the left edge of the
        other window.
        """
        return __core.wxIndividualLayoutConstraint_LeftOf(*args, **kwargs)

    def RightOf(*args, **kwargs):
        """
        RightOf(self, wxWindow sibling, int marg=0)

        Constrains this edge to be to the right of the given window, with an
        optional margin. Implicitly, this is relative to the right edge of the
        other window.
        """
        return __core.wxIndividualLayoutConstraint_RightOf(*args, **kwargs)

    def Above(*args, **kwargs):
        """
        Above(self, wxWindow sibling, int marg=0)

        Constrains this edge to be above the given window, with an optional
        margin. Implicitly, this is relative to the top edge of the other
        window.
        """
        return __core.wxIndividualLayoutConstraint_Above(*args, **kwargs)

    def Below(*args, **kwargs):
        """
        Below(self, wxWindow sibling, int marg=0)

        Constrains this edge to be below the given window, with an optional
        margin. Implicitly, this is relative to the bottom edge of the other
        window.
        """
        return __core.wxIndividualLayoutConstraint_Below(*args, **kwargs)

    def SameAs(*args, **kwargs):
        """
        SameAs(self, wxWindow otherW, int edge, int marg=0)

        Constrains this edge or dimension to be to the same as the edge of the
        given window, with an optional margin.
        """
        return __core.wxIndividualLayoutConstraint_SameAs(*args, **kwargs)

    def PercentOf(*args, **kwargs):
        """
        PercentOf(self, wxWindow otherW, int wh, int per)

        Constrains this edge or dimension to be to a percentage of the given
        window, with an optional margin.
        """
        return __core.wxIndividualLayoutConstraint_PercentOf(*args, **kwargs)

    def Absolute(*args, **kwargs):
        """
        Absolute(self, int val)

        Constrains this edge or dimension to be the given absolute value.
        """
        return __core.wxIndividualLayoutConstraint_Absolute(*args, **kwargs)

    def Unconstrained(*args, **kwargs):
        """
        Unconstrained(self)

        Sets this edge or dimension to be unconstrained, that is, dependent on
        other edges and dimensions from which this value can be deduced.
        """
        return __core.wxIndividualLayoutConstraint_Unconstrained(*args, **kwargs)

    def AsIs(*args, **kwargs):
        """
        AsIs(self)

        Sets this edge or constraint to be whatever the window's value is at
        the moment. If either of the width and height constraints are *as is*,
        the window will not be resized, but moved instead. This is important
        when considering panel items which are intended to have a default
        size, such as a button, which may take its size from the size of the
        button label.
        """
        return __core.wxIndividualLayoutConstraint_AsIs(*args, **kwargs)

    def GetOtherWindow(*args, **kwargs):
        """GetOtherWindow(self) -> wxWindow"""
        return __core.wxIndividualLayoutConstraint_GetOtherWindow(*args, **kwargs)

    def GetMyEdge(*args, **kwargs):
        """GetMyEdge(self) -> int"""
        return __core.wxIndividualLayoutConstraint_GetMyEdge(*args, **kwargs)

    def SetEdge(*args, **kwargs):
        """SetEdge(self, int which)"""
        return __core.wxIndividualLayoutConstraint_SetEdge(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, int v)"""
        return __core.wxIndividualLayoutConstraint_SetValue(*args, **kwargs)

    def GetMargin(*args, **kwargs):
        """GetMargin(self) -> int"""
        return __core.wxIndividualLayoutConstraint_GetMargin(*args, **kwargs)

    def SetMargin(*args, **kwargs):
        """SetMargin(self, int m)"""
        return __core.wxIndividualLayoutConstraint_SetMargin(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> int"""
        return __core.wxIndividualLayoutConstraint_GetValue(*args, **kwargs)

    def GetPercent(*args, **kwargs):
        """GetPercent(self) -> int"""
        return __core.wxIndividualLayoutConstraint_GetPercent(*args, **kwargs)

    def GetOtherEdge(*args, **kwargs):
        """GetOtherEdge(self) -> int"""
        return __core.wxIndividualLayoutConstraint_GetOtherEdge(*args, **kwargs)

    def GetDone(*args, **kwargs):
        """GetDone(self) -> bool"""
        return __core.wxIndividualLayoutConstraint_GetDone(*args, **kwargs)

    def SetDone(*args, **kwargs):
        """SetDone(self, bool d)"""
        return __core.wxIndividualLayoutConstraint_SetDone(*args, **kwargs)

    def GetRelationship(*args, **kwargs):
        """GetRelationship(self) -> int"""
        return __core.wxIndividualLayoutConstraint_GetRelationship(*args, **kwargs)

    def SetRelationship(*args, **kwargs):
        """SetRelationship(self, int r)"""
        return __core.wxIndividualLayoutConstraint_SetRelationship(*args, **kwargs)

    def ResetIfWin(*args, **kwargs):
        """
        ResetIfWin(self, wxWindow otherW) -> bool

        Reset constraint if it mentions otherWin
        """
        return __core.wxIndividualLayoutConstraint_ResetIfWin(*args, **kwargs)

    def SatisfyConstraint(*args, **kwargs):
        """
        SatisfyConstraint(self, wxLayoutConstraints constraints, wxWindow win) -> bool

        Try to satisfy constraint
        """
        return __core.wxIndividualLayoutConstraint_SatisfyConstraint(*args, **kwargs)

    def GetEdge(*args, **kwargs):
        """
        GetEdge(self, int which, wxWindow thisWin, wxWindow other) -> int

        Get the value of this edge or dimension, or if this
        is not determinable, -1.
        """
        return __core.wxIndividualLayoutConstraint_GetEdge(*args, **kwargs)


class wxIndividualLayoutConstraintPtr(wxIndividualLayoutConstraint):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxIndividualLayoutConstraint
__core.wxIndividualLayoutConstraint_swigregister(wxIndividualLayoutConstraintPtr)

class wxLayoutConstraints(wxObject):
    """
    **Note:** constraints are now deprecated and you should use sizers
    instead.

    Objects of this class can be associated with a window to define its
    layout constraints, with respect to siblings or its parent.

    The class consists of the following eight constraints of class
    wx.IndividualLayoutConstraint, some or all of which should be accessed
    directly to set the appropriate constraints.

        * left: represents the left hand edge of the window
        * right: represents the right hand edge of the window
        * top: represents the top edge of the window
        * bottom: represents the bottom edge of the window
        * width: represents the width of the window
        * height: represents the height of the window
        * centreX: represents the horizontal centre point of the window
        * centreY: represents the vertical centre point of the window 

    Most constraints are initially set to have the relationship
    wxUnconstrained, which means that their values should be calculated by
    looking at known constraints. The exceptions are width and height,
    which are set to wxAsIs to ensure that if the user does not specify a
    constraint, the existing width and height will be used, to be
    compatible with panel items which often have take a default size. If
    the constraint is ``wx.AsIs``, the dimension will not be changed.

    :see: `wx.IndividualLayoutConstraint`, `wx.Window.SetConstraints`

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxLayoutConstraints instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    left = property(__core.wxLayoutConstraints_left_get)
    top = property(__core.wxLayoutConstraints_top_get)
    right = property(__core.wxLayoutConstraints_right_get)
    bottom = property(__core.wxLayoutConstraints_bottom_get)
    width = property(__core.wxLayoutConstraints_width_get)
    height = property(__core.wxLayoutConstraints_height_get)
    centreX = property(__core.wxLayoutConstraints_centreX_get)
    centreY = property(__core.wxLayoutConstraints_centreY_get)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> wxLayoutConstraints"""
        newobj = __core.new_wxLayoutConstraints(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SatisfyConstraints(*args, **kwargs):
        """SatisfyConstraints(Window win) -> (areSatisfied, noChanges)"""
        return __core.wxLayoutConstraints_SatisfyConstraints(*args, **kwargs)

    def AreSatisfied(*args, **kwargs):
        """AreSatisfied(self) -> bool"""
        return __core.wxLayoutConstraints_AreSatisfied(*args, **kwargs)


class wxLayoutConstraintsPtr(wxLayoutConstraints):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxLayoutConstraints
__core.wxLayoutConstraints_swigregister(wxLayoutConstraintsPtr)

#----------------------------------------------------------------------------

# Use Python's bool constants if available, make some if not
try:
    True
except NameError:
    __builtins__.True = 1==1
    __builtins__.False = 1==0
    def bool(value): return not not value
    __builtins__.bool = bool



# workarounds for bad wxRTTI names
__wxPyPtrTypeMap['wxGauge95']    = 'wxGauge'
__wxPyPtrTypeMap['wxSlider95']   = 'wxSlider'
__wxPyPtrTypeMap['wxStatusBar95']   = 'wxStatusBar'


#----------------------------------------------------------------------------
# Load version numbers from __version__...  Ensure that major and minor
# versions are the same for both wxPython and wxWidgets.

from __version__ import *
__version__ = VERSION_STRING

assert MAJOR_VERSION == _core_.MAJOR_VERSION, "wxPython/wxWidgets version mismatch"
assert MINOR_VERSION == _core_.MINOR_VERSION, "wxPython/wxWidgets version mismatch"
if RELEASE_VERSION != _core_.RELEASE_VERSION:
    import warnings
    warnings.warn("wxPython/wxWidgets release number mismatch")

#----------------------------------------------------------------------------

# Set the default string<-->unicode conversion encoding from the
# locale.  This encoding is used when string or unicode objects need
# to be converted in order to pass them to wxWidgets.  Please be aware
# that the default encoding within the same locale may be slightly
# different on different platforms.  For example, please see
# http://www.alanwood.net/demos/charsetdiffs.html for differences
# between the common latin/roman encodings.

default = _sys.getdefaultencoding()
if default == 'ascii':
    import locale
    import codecs
    try:
        default = locale.getdefaultlocale()[1]
        codecs.lookup(default)
    except (ValueError, LookupError):
        default = _sys.getdefaultencoding()
    del locale
    del codecs
if default:
    wx.SetDefaultPyEncoding(default)
del default

#----------------------------------------------------------------------------

class PyDeadObjectError(AttributeError):
    pass

class _wxPyDeadObject(object):
    """
    Instances of wx objects that are OOR capable will have their __class__
    changed to this class when the C++ object is deleted.  This should help
    prevent crashes due to referencing a bogus C++ pointer.
    """
    reprStr = "wxPython wrapper for DELETED %s object! (The C++ object no longer exists.)"
    attrStr = "The C++ part of the %s object has been deleted, attribute access no longer allowed."

    def __repr__(self):
        if not hasattr(self, "_name"):
            self._name = "[unknown]"
        return self.reprStr % self._name

    def __getattr__(self, *args):
        if not hasattr(self, "_name"):
            self._name = "[unknown]"
        raise PyDeadObjectError(self.attrStr % self._name)

    def __nonzero__(self):
        return 0



class PyUnbornObjectError(AttributeError):
    pass

class _wxPyUnbornObject(object):
    """
    Some stock objects are created when the wx._core module is
    imported, but their C++ instance is not created until the wx.App
    object is created and initialized.  These object instances will
    temporarily have their __class__ changed to this class so an
    exception will be raised if they are used before the C++ instance
    is ready.
    """

    reprStr = "wxPython wrapper for UNBORN object! (The C++ object is not initialized yet.)"
    attrStr = "The C++ part of this object has not been initialized, attribute access not allowed."

    def __repr__(self):
        #if not hasattr(self, "_name"):
        #    self._name = "[unknown]"
        return self.reprStr #% self._name

    def __getattr__(self, *args):
        #if not hasattr(self, "_name"):
        #    self._name = "[unknown]"
        raise PyUnbornObjectError(self.attrStr) # % self._name )

    def __nonzero__(self):
        return 0


#----------------------------------------------------------------------------

def CallAfter(callable, *args, **kw):
    """
    Call the specified function after the current and pending event
    handlers have been completed.  This is also good for making GUI
    method calls from non-GUI threads.  Any extra positional or
    keyword args are passed on to the callable when it is called.

    :see: `wx.FutureCall`
    """
    app = wx.GetApp()
    assert app is not None, 'No wx.App created yet'

    if not hasattr(app, "_CallAfterId"):
        app._CallAfterId = wx.NewEventType()
        app.Connect(-1, -1, app._CallAfterId,
                    lambda event: event.callable(*event.args, **event.kw) )
    evt = wx.PyEvent()
    evt.SetEventType(app._CallAfterId)
    evt.callable = callable
    evt.args = args
    evt.kw = kw
    wx.PostEvent(app, evt)

#----------------------------------------------------------------------------


class FutureCall:
    """
    A convenience class for `wx.Timer`, that calls the given callable
    object once after the given amount of milliseconds, passing any
    positional or keyword args.  The return value of the callable is
    availbale after it has been run with the `GetResult` method.

    If you don't need to get the return value or restart the timer
    then there is no need to hold a reference to this object.  It will
    hold a reference to itself while the timer is running (the timer
    has a reference to self.Notify) but the cycle will be broken when
    the timer completes, automatically cleaning up the wx.FutureCall
    object.

    :see: `wx.CallAfter`
    """
    def __init__(self, millis, callable, *args, **kwargs):
        self.millis = millis
        self.callable = callable
        self.SetArgs(*args, **kwargs)
        self.runCount = 0
        self.running = False
        self.hasRun = False
        self.result = None
        self.timer = None
        self.Start()

    def __del__(self):
        self.Stop()


    def Start(self, millis=None, *args, **kwargs):
        """
        (Re)start the timer
        """
        self.hasRun = False
        if millis is not None:
            self.millis = millis
        if args or kwargs:
            self.SetArgs(*args, **kwargs)
        self.Stop()
        self.timer = wx.PyTimer(self.Notify)
        self.timer.Start(self.millis, wx.TIMER_ONE_SHOT)
        self.running = True
    Restart = Start


    def Stop(self):
        """
        Stop and destroy the timer.
        """
        if self.timer is not None:
            self.timer.Stop()
            self.timer = None


    def GetInterval(self):
        if self.timer is not None:
            return self.timer.GetInterval()
        else:
            return 0


    def IsRunning(self):
        return self.timer is not None and self.timer.IsRunning()


    def SetArgs(self, *args, **kwargs):
        """
        (Re)set the args passed to the callable object.  This is
        useful in conjunction with Restart if you want to schedule a
        new call to the same callable object but with different
        parameters.
        """
        self.args = args
        self.kwargs = kwargs


    def HasRun(self):
        return self.hasRun

    def GetResult(self):
        return self.result

    def Notify(self):
        """
        The timer has expired so call the callable.
        """
        if self.callable and getattr(self.callable, 'im_self', True):
            self.runCount += 1
            self.running = False
            self.result = self.callable(*self.args, **self.kwargs)
        self.hasRun = True
        if not self.running:
            # if it wasn't restarted, then cleanup
            wx.CallAfter(self.Stop)



#----------------------------------------------------------------------------
# Control which items in this module should be documented by epydoc.
# We allow only classes and functions, which will help reduce the size
# of the docs by filtering out the zillions of constants, EVT objects,
# and etc that don't make much sense by themselves, but are instead
# documented (or will be) as part of the classes/functions/methods
# where they should be used.

class __DocFilter:
    """
    A filter for epydoc that only allows non-Ptr classes and
    fucntions, in order to reduce the clutter in the API docs.
    """
    def __init__(self, globals):
        self._globals = globals
        
    def __call__(self, name):
        import types
        obj = self._globals.get(name, None)
        if type(obj) not in [type, types.ClassType, types.FunctionType, types.BuiltinFunctionType]:
            return False
        if name.startswith('_') or name.endswith('Ptr') or name.startswith('EVT'):
            return False
        return True

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

# Import other modules in this package that should show up in the
# "core" wx namespace
from _gdi import *
from _windows import *
from _controls import *
from _misc import *


# Fixup the stock objects since they can't be used yet.  (They will be
# restored in wx.PyApp.OnInit.)
_core_._wxPyFixStockObjects()

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------


