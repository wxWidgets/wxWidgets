# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _core_

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

NOT_FOUND = _core_.NOT_FOUND
VSCROLL = _core_.VSCROLL
HSCROLL = _core_.HSCROLL
CAPTION = _core_.CAPTION
DOUBLE_BORDER = _core_.DOUBLE_BORDER
SUNKEN_BORDER = _core_.SUNKEN_BORDER
RAISED_BORDER = _core_.RAISED_BORDER
BORDER = _core_.BORDER
SIMPLE_BORDER = _core_.SIMPLE_BORDER
STATIC_BORDER = _core_.STATIC_BORDER
TRANSPARENT_WINDOW = _core_.TRANSPARENT_WINDOW
NO_BORDER = _core_.NO_BORDER
TAB_TRAVERSAL = _core_.TAB_TRAVERSAL
WANTS_CHARS = _core_.WANTS_CHARS
POPUP_WINDOW = _core_.POPUP_WINDOW
CENTER_FRAME = _core_.CENTER_FRAME
CENTRE_ON_SCREEN = _core_.CENTRE_ON_SCREEN
CENTER_ON_SCREEN = _core_.CENTER_ON_SCREEN
ED_CLIENT_MARGIN = _core_.ED_CLIENT_MARGIN
ED_BUTTONS_BOTTOM = _core_.ED_BUTTONS_BOTTOM
ED_BUTTONS_RIGHT = _core_.ED_BUTTONS_RIGHT
ED_STATIC_LINE = _core_.ED_STATIC_LINE
EXT_DIALOG_STYLE = _core_.EXT_DIALOG_STYLE
CLIP_CHILDREN = _core_.CLIP_CHILDREN
CLIP_SIBLINGS = _core_.CLIP_SIBLINGS
ALWAYS_SHOW_SB = _core_.ALWAYS_SHOW_SB
RETAINED = _core_.RETAINED
BACKINGSTORE = _core_.BACKINGSTORE
COLOURED = _core_.COLOURED
FIXED_LENGTH = _core_.FIXED_LENGTH
LB_NEEDED_SB = _core_.LB_NEEDED_SB
LB_ALWAYS_SB = _core_.LB_ALWAYS_SB
LB_SORT = _core_.LB_SORT
LB_SINGLE = _core_.LB_SINGLE
LB_MULTIPLE = _core_.LB_MULTIPLE
LB_EXTENDED = _core_.LB_EXTENDED
LB_OWNERDRAW = _core_.LB_OWNERDRAW
LB_HSCROLL = _core_.LB_HSCROLL
PROCESS_ENTER = _core_.PROCESS_ENTER
PASSWORD = _core_.PASSWORD
CB_SIMPLE = _core_.CB_SIMPLE
CB_DROPDOWN = _core_.CB_DROPDOWN
CB_SORT = _core_.CB_SORT
CB_READONLY = _core_.CB_READONLY
RA_HORIZONTAL = _core_.RA_HORIZONTAL
RA_VERTICAL = _core_.RA_VERTICAL
RA_SPECIFY_ROWS = _core_.RA_SPECIFY_ROWS
RA_SPECIFY_COLS = _core_.RA_SPECIFY_COLS
RB_GROUP = _core_.RB_GROUP
RB_SINGLE = _core_.RB_SINGLE
SL_HORIZONTAL = _core_.SL_HORIZONTAL
SL_VERTICAL = _core_.SL_VERTICAL
SL_AUTOTICKS = _core_.SL_AUTOTICKS
SL_LABELS = _core_.SL_LABELS
SL_LEFT = _core_.SL_LEFT
SL_TOP = _core_.SL_TOP
SL_RIGHT = _core_.SL_RIGHT
SL_BOTTOM = _core_.SL_BOTTOM
SL_BOTH = _core_.SL_BOTH
SL_SELRANGE = _core_.SL_SELRANGE
SB_HORIZONTAL = _core_.SB_HORIZONTAL
SB_VERTICAL = _core_.SB_VERTICAL
ST_SIZEGRIP = _core_.ST_SIZEGRIP
ST_NO_AUTORESIZE = _core_.ST_NO_AUTORESIZE
FLOOD_SURFACE = _core_.FLOOD_SURFACE
FLOOD_BORDER = _core_.FLOOD_BORDER
ODDEVEN_RULE = _core_.ODDEVEN_RULE
WINDING_RULE = _core_.WINDING_RULE
TOOL_TOP = _core_.TOOL_TOP
TOOL_BOTTOM = _core_.TOOL_BOTTOM
TOOL_LEFT = _core_.TOOL_LEFT
TOOL_RIGHT = _core_.TOOL_RIGHT
OK = _core_.OK
YES_NO = _core_.YES_NO
CANCEL = _core_.CANCEL
YES = _core_.YES
NO = _core_.NO
NO_DEFAULT = _core_.NO_DEFAULT
YES_DEFAULT = _core_.YES_DEFAULT
ICON_EXCLAMATION = _core_.ICON_EXCLAMATION
ICON_HAND = _core_.ICON_HAND
ICON_QUESTION = _core_.ICON_QUESTION
ICON_INFORMATION = _core_.ICON_INFORMATION
ICON_STOP = _core_.ICON_STOP
ICON_ASTERISK = _core_.ICON_ASTERISK
ICON_MASK = _core_.ICON_MASK
ICON_WARNING = _core_.ICON_WARNING
ICON_ERROR = _core_.ICON_ERROR
FORWARD = _core_.FORWARD
BACKWARD = _core_.BACKWARD
RESET = _core_.RESET
HELP = _core_.HELP
MORE = _core_.MORE
SETUP = _core_.SETUP
SIZE_AUTO_WIDTH = _core_.SIZE_AUTO_WIDTH
SIZE_AUTO_HEIGHT = _core_.SIZE_AUTO_HEIGHT
SIZE_AUTO = _core_.SIZE_AUTO
SIZE_USE_EXISTING = _core_.SIZE_USE_EXISTING
SIZE_ALLOW_MINUS_ONE = _core_.SIZE_ALLOW_MINUS_ONE
PORTRAIT = _core_.PORTRAIT
LANDSCAPE = _core_.LANDSCAPE
PRINT_QUALITY_HIGH = _core_.PRINT_QUALITY_HIGH
PRINT_QUALITY_MEDIUM = _core_.PRINT_QUALITY_MEDIUM
PRINT_QUALITY_LOW = _core_.PRINT_QUALITY_LOW
PRINT_QUALITY_DRAFT = _core_.PRINT_QUALITY_DRAFT
ID_ANY = _core_.ID_ANY
ID_SEPARATOR = _core_.ID_SEPARATOR
ID_LOWEST = _core_.ID_LOWEST
ID_OPEN = _core_.ID_OPEN
ID_CLOSE = _core_.ID_CLOSE
ID_NEW = _core_.ID_NEW
ID_SAVE = _core_.ID_SAVE
ID_SAVEAS = _core_.ID_SAVEAS
ID_REVERT = _core_.ID_REVERT
ID_EXIT = _core_.ID_EXIT
ID_UNDO = _core_.ID_UNDO
ID_REDO = _core_.ID_REDO
ID_HELP = _core_.ID_HELP
ID_PRINT = _core_.ID_PRINT
ID_PRINT_SETUP = _core_.ID_PRINT_SETUP
ID_PREVIEW = _core_.ID_PREVIEW
ID_ABOUT = _core_.ID_ABOUT
ID_HELP_CONTENTS = _core_.ID_HELP_CONTENTS
ID_HELP_COMMANDS = _core_.ID_HELP_COMMANDS
ID_HELP_PROCEDURES = _core_.ID_HELP_PROCEDURES
ID_HELP_CONTEXT = _core_.ID_HELP_CONTEXT
ID_CLOSE_ALL = _core_.ID_CLOSE_ALL
ID_PREFERENCES = _core_.ID_PREFERENCES
ID_CUT = _core_.ID_CUT
ID_COPY = _core_.ID_COPY
ID_PASTE = _core_.ID_PASTE
ID_CLEAR = _core_.ID_CLEAR
ID_FIND = _core_.ID_FIND
ID_DUPLICATE = _core_.ID_DUPLICATE
ID_SELECTALL = _core_.ID_SELECTALL
ID_DELETE = _core_.ID_DELETE
ID_REPLACE = _core_.ID_REPLACE
ID_REPLACE_ALL = _core_.ID_REPLACE_ALL
ID_PROPERTIES = _core_.ID_PROPERTIES
ID_VIEW_DETAILS = _core_.ID_VIEW_DETAILS
ID_VIEW_LARGEICONS = _core_.ID_VIEW_LARGEICONS
ID_VIEW_SMALLICONS = _core_.ID_VIEW_SMALLICONS
ID_VIEW_LIST = _core_.ID_VIEW_LIST
ID_VIEW_SORTDATE = _core_.ID_VIEW_SORTDATE
ID_VIEW_SORTNAME = _core_.ID_VIEW_SORTNAME
ID_VIEW_SORTSIZE = _core_.ID_VIEW_SORTSIZE
ID_VIEW_SORTTYPE = _core_.ID_VIEW_SORTTYPE
ID_FILE1 = _core_.ID_FILE1
ID_FILE2 = _core_.ID_FILE2
ID_FILE3 = _core_.ID_FILE3
ID_FILE4 = _core_.ID_FILE4
ID_FILE5 = _core_.ID_FILE5
ID_FILE6 = _core_.ID_FILE6
ID_FILE7 = _core_.ID_FILE7
ID_FILE8 = _core_.ID_FILE8
ID_FILE9 = _core_.ID_FILE9
ID_OK = _core_.ID_OK
ID_CANCEL = _core_.ID_CANCEL
ID_APPLY = _core_.ID_APPLY
ID_YES = _core_.ID_YES
ID_NO = _core_.ID_NO
ID_STATIC = _core_.ID_STATIC
ID_FORWARD = _core_.ID_FORWARD
ID_BACKWARD = _core_.ID_BACKWARD
ID_DEFAULT = _core_.ID_DEFAULT
ID_MORE = _core_.ID_MORE
ID_SETUP = _core_.ID_SETUP
ID_RESET = _core_.ID_RESET
ID_CONTEXT_HELP = _core_.ID_CONTEXT_HELP
ID_YESTOALL = _core_.ID_YESTOALL
ID_NOTOALL = _core_.ID_NOTOALL
ID_ABORT = _core_.ID_ABORT
ID_RETRY = _core_.ID_RETRY
ID_IGNORE = _core_.ID_IGNORE
ID_ADD = _core_.ID_ADD
ID_REMOVE = _core_.ID_REMOVE
ID_UP = _core_.ID_UP
ID_DOWN = _core_.ID_DOWN
ID_HOME = _core_.ID_HOME
ID_REFRESH = _core_.ID_REFRESH
ID_STOP = _core_.ID_STOP
ID_INDEX = _core_.ID_INDEX
ID_BOLD = _core_.ID_BOLD
ID_ITALIC = _core_.ID_ITALIC
ID_JUSTIFY_CENTER = _core_.ID_JUSTIFY_CENTER
ID_JUSTIFY_FILL = _core_.ID_JUSTIFY_FILL
ID_JUSTIFY_RIGHT = _core_.ID_JUSTIFY_RIGHT
ID_JUSTIFY_LEFT = _core_.ID_JUSTIFY_LEFT
ID_UNDERLINE = _core_.ID_UNDERLINE
ID_INDENT = _core_.ID_INDENT
ID_UNINDENT = _core_.ID_UNINDENT
ID_ZOOM_100 = _core_.ID_ZOOM_100
ID_ZOOM_FIT = _core_.ID_ZOOM_FIT
ID_ZOOM_IN = _core_.ID_ZOOM_IN
ID_ZOOM_OUT = _core_.ID_ZOOM_OUT
ID_UNDELETE = _core_.ID_UNDELETE
ID_REVERT_TO_SAVED = _core_.ID_REVERT_TO_SAVED
ID_HIGHEST = _core_.ID_HIGHEST
OPEN = _core_.OPEN
SAVE = _core_.SAVE
HIDE_READONLY = _core_.HIDE_READONLY
OVERWRITE_PROMPT = _core_.OVERWRITE_PROMPT
FILE_MUST_EXIST = _core_.FILE_MUST_EXIST
MULTIPLE = _core_.MULTIPLE
CHANGE_DIR = _core_.CHANGE_DIR
ACCEL_ALT = _core_.ACCEL_ALT
ACCEL_CTRL = _core_.ACCEL_CTRL
ACCEL_SHIFT = _core_.ACCEL_SHIFT
ACCEL_NORMAL = _core_.ACCEL_NORMAL
PD_AUTO_HIDE = _core_.PD_AUTO_HIDE
PD_APP_MODAL = _core_.PD_APP_MODAL
PD_CAN_ABORT = _core_.PD_CAN_ABORT
PD_ELAPSED_TIME = _core_.PD_ELAPSED_TIME
PD_ESTIMATED_TIME = _core_.PD_ESTIMATED_TIME
PD_REMAINING_TIME = _core_.PD_REMAINING_TIME
DD_NEW_DIR_BUTTON = _core_.DD_NEW_DIR_BUTTON
DD_DEFAULT_STYLE = _core_.DD_DEFAULT_STYLE
MENU_TEAROFF = _core_.MENU_TEAROFF
MB_DOCKABLE = _core_.MB_DOCKABLE
NO_FULL_REPAINT_ON_RESIZE = _core_.NO_FULL_REPAINT_ON_RESIZE
FULL_REPAINT_ON_RESIZE = _core_.FULL_REPAINT_ON_RESIZE
LI_HORIZONTAL = _core_.LI_HORIZONTAL
LI_VERTICAL = _core_.LI_VERTICAL
WS_EX_VALIDATE_RECURSIVELY = _core_.WS_EX_VALIDATE_RECURSIVELY
WS_EX_BLOCK_EVENTS = _core_.WS_EX_BLOCK_EVENTS
WS_EX_TRANSIENT = _core_.WS_EX_TRANSIENT
WS_EX_THEMED_BACKGROUND = _core_.WS_EX_THEMED_BACKGROUND
WS_EX_PROCESS_IDLE = _core_.WS_EX_PROCESS_IDLE
WS_EX_PROCESS_UI_UPDATES = _core_.WS_EX_PROCESS_UI_UPDATES
MM_TEXT = _core_.MM_TEXT
MM_LOMETRIC = _core_.MM_LOMETRIC
MM_HIMETRIC = _core_.MM_HIMETRIC
MM_LOENGLISH = _core_.MM_LOENGLISH
MM_HIENGLISH = _core_.MM_HIENGLISH
MM_TWIPS = _core_.MM_TWIPS
MM_ISOTROPIC = _core_.MM_ISOTROPIC
MM_ANISOTROPIC = _core_.MM_ANISOTROPIC
MM_POINTS = _core_.MM_POINTS
MM_METRIC = _core_.MM_METRIC
CENTRE = _core_.CENTRE
CENTER = _core_.CENTER
HORIZONTAL = _core_.HORIZONTAL
VERTICAL = _core_.VERTICAL
BOTH = _core_.BOTH
LEFT = _core_.LEFT
RIGHT = _core_.RIGHT
UP = _core_.UP
DOWN = _core_.DOWN
TOP = _core_.TOP
BOTTOM = _core_.BOTTOM
NORTH = _core_.NORTH
SOUTH = _core_.SOUTH
WEST = _core_.WEST
EAST = _core_.EAST
ALL = _core_.ALL
ALIGN_NOT = _core_.ALIGN_NOT
ALIGN_CENTER_HORIZONTAL = _core_.ALIGN_CENTER_HORIZONTAL
ALIGN_CENTRE_HORIZONTAL = _core_.ALIGN_CENTRE_HORIZONTAL
ALIGN_LEFT = _core_.ALIGN_LEFT
ALIGN_TOP = _core_.ALIGN_TOP
ALIGN_RIGHT = _core_.ALIGN_RIGHT
ALIGN_BOTTOM = _core_.ALIGN_BOTTOM
ALIGN_CENTER_VERTICAL = _core_.ALIGN_CENTER_VERTICAL
ALIGN_CENTRE_VERTICAL = _core_.ALIGN_CENTRE_VERTICAL
ALIGN_CENTER = _core_.ALIGN_CENTER
ALIGN_CENTRE = _core_.ALIGN_CENTRE
ALIGN_MASK = _core_.ALIGN_MASK
STRETCH_NOT = _core_.STRETCH_NOT
SHRINK = _core_.SHRINK
GROW = _core_.GROW
EXPAND = _core_.EXPAND
SHAPED = _core_.SHAPED
FIXED_MINSIZE = _core_.FIXED_MINSIZE
TILE = _core_.TILE
ADJUST_MINSIZE = _core_.ADJUST_MINSIZE
BORDER_DEFAULT = _core_.BORDER_DEFAULT
BORDER_NONE = _core_.BORDER_NONE
BORDER_STATIC = _core_.BORDER_STATIC
BORDER_SIMPLE = _core_.BORDER_SIMPLE
BORDER_RAISED = _core_.BORDER_RAISED
BORDER_SUNKEN = _core_.BORDER_SUNKEN
BORDER_DOUBLE = _core_.BORDER_DOUBLE
BORDER_MASK = _core_.BORDER_MASK
BG_STYLE_SYSTEM = _core_.BG_STYLE_SYSTEM
BG_STYLE_COLOUR = _core_.BG_STYLE_COLOUR
BG_STYLE_CUSTOM = _core_.BG_STYLE_CUSTOM
DEFAULT = _core_.DEFAULT
DECORATIVE = _core_.DECORATIVE
ROMAN = _core_.ROMAN
SCRIPT = _core_.SCRIPT
SWISS = _core_.SWISS
MODERN = _core_.MODERN
TELETYPE = _core_.TELETYPE
VARIABLE = _core_.VARIABLE
FIXED = _core_.FIXED
NORMAL = _core_.NORMAL
LIGHT = _core_.LIGHT
BOLD = _core_.BOLD
ITALIC = _core_.ITALIC
SLANT = _core_.SLANT
SOLID = _core_.SOLID
DOT = _core_.DOT
LONG_DASH = _core_.LONG_DASH
SHORT_DASH = _core_.SHORT_DASH
DOT_DASH = _core_.DOT_DASH
USER_DASH = _core_.USER_DASH
TRANSPARENT = _core_.TRANSPARENT
STIPPLE = _core_.STIPPLE
BDIAGONAL_HATCH = _core_.BDIAGONAL_HATCH
CROSSDIAG_HATCH = _core_.CROSSDIAG_HATCH
FDIAGONAL_HATCH = _core_.FDIAGONAL_HATCH
CROSS_HATCH = _core_.CROSS_HATCH
HORIZONTAL_HATCH = _core_.HORIZONTAL_HATCH
VERTICAL_HATCH = _core_.VERTICAL_HATCH
JOIN_BEVEL = _core_.JOIN_BEVEL
JOIN_MITER = _core_.JOIN_MITER
JOIN_ROUND = _core_.JOIN_ROUND
CAP_ROUND = _core_.CAP_ROUND
CAP_PROJECTING = _core_.CAP_PROJECTING
CAP_BUTT = _core_.CAP_BUTT
CLEAR = _core_.CLEAR
XOR = _core_.XOR
INVERT = _core_.INVERT
OR_REVERSE = _core_.OR_REVERSE
AND_REVERSE = _core_.AND_REVERSE
COPY = _core_.COPY
AND = _core_.AND
AND_INVERT = _core_.AND_INVERT
NO_OP = _core_.NO_OP
NOR = _core_.NOR
EQUIV = _core_.EQUIV
SRC_INVERT = _core_.SRC_INVERT
OR_INVERT = _core_.OR_INVERT
NAND = _core_.NAND
OR = _core_.OR
SET = _core_.SET
WXK_BACK = _core_.WXK_BACK
WXK_TAB = _core_.WXK_TAB
WXK_RETURN = _core_.WXK_RETURN
WXK_ESCAPE = _core_.WXK_ESCAPE
WXK_SPACE = _core_.WXK_SPACE
WXK_DELETE = _core_.WXK_DELETE
WXK_START = _core_.WXK_START
WXK_LBUTTON = _core_.WXK_LBUTTON
WXK_RBUTTON = _core_.WXK_RBUTTON
WXK_CANCEL = _core_.WXK_CANCEL
WXK_MBUTTON = _core_.WXK_MBUTTON
WXK_CLEAR = _core_.WXK_CLEAR
WXK_SHIFT = _core_.WXK_SHIFT
WXK_ALT = _core_.WXK_ALT
WXK_CONTROL = _core_.WXK_CONTROL
WXK_MENU = _core_.WXK_MENU
WXK_PAUSE = _core_.WXK_PAUSE
WXK_CAPITAL = _core_.WXK_CAPITAL
WXK_PRIOR = _core_.WXK_PRIOR
WXK_NEXT = _core_.WXK_NEXT
WXK_END = _core_.WXK_END
WXK_HOME = _core_.WXK_HOME
WXK_LEFT = _core_.WXK_LEFT
WXK_UP = _core_.WXK_UP
WXK_RIGHT = _core_.WXK_RIGHT
WXK_DOWN = _core_.WXK_DOWN
WXK_SELECT = _core_.WXK_SELECT
WXK_PRINT = _core_.WXK_PRINT
WXK_EXECUTE = _core_.WXK_EXECUTE
WXK_SNAPSHOT = _core_.WXK_SNAPSHOT
WXK_INSERT = _core_.WXK_INSERT
WXK_HELP = _core_.WXK_HELP
WXK_NUMPAD0 = _core_.WXK_NUMPAD0
WXK_NUMPAD1 = _core_.WXK_NUMPAD1
WXK_NUMPAD2 = _core_.WXK_NUMPAD2
WXK_NUMPAD3 = _core_.WXK_NUMPAD3
WXK_NUMPAD4 = _core_.WXK_NUMPAD4
WXK_NUMPAD5 = _core_.WXK_NUMPAD5
WXK_NUMPAD6 = _core_.WXK_NUMPAD6
WXK_NUMPAD7 = _core_.WXK_NUMPAD7
WXK_NUMPAD8 = _core_.WXK_NUMPAD8
WXK_NUMPAD9 = _core_.WXK_NUMPAD9
WXK_MULTIPLY = _core_.WXK_MULTIPLY
WXK_ADD = _core_.WXK_ADD
WXK_SEPARATOR = _core_.WXK_SEPARATOR
WXK_SUBTRACT = _core_.WXK_SUBTRACT
WXK_DECIMAL = _core_.WXK_DECIMAL
WXK_DIVIDE = _core_.WXK_DIVIDE
WXK_F1 = _core_.WXK_F1
WXK_F2 = _core_.WXK_F2
WXK_F3 = _core_.WXK_F3
WXK_F4 = _core_.WXK_F4
WXK_F5 = _core_.WXK_F5
WXK_F6 = _core_.WXK_F6
WXK_F7 = _core_.WXK_F7
WXK_F8 = _core_.WXK_F8
WXK_F9 = _core_.WXK_F9
WXK_F10 = _core_.WXK_F10
WXK_F11 = _core_.WXK_F11
WXK_F12 = _core_.WXK_F12
WXK_F13 = _core_.WXK_F13
WXK_F14 = _core_.WXK_F14
WXK_F15 = _core_.WXK_F15
WXK_F16 = _core_.WXK_F16
WXK_F17 = _core_.WXK_F17
WXK_F18 = _core_.WXK_F18
WXK_F19 = _core_.WXK_F19
WXK_F20 = _core_.WXK_F20
WXK_F21 = _core_.WXK_F21
WXK_F22 = _core_.WXK_F22
WXK_F23 = _core_.WXK_F23
WXK_F24 = _core_.WXK_F24
WXK_NUMLOCK = _core_.WXK_NUMLOCK
WXK_SCROLL = _core_.WXK_SCROLL
WXK_PAGEUP = _core_.WXK_PAGEUP
WXK_PAGEDOWN = _core_.WXK_PAGEDOWN
WXK_NUMPAD_SPACE = _core_.WXK_NUMPAD_SPACE
WXK_NUMPAD_TAB = _core_.WXK_NUMPAD_TAB
WXK_NUMPAD_ENTER = _core_.WXK_NUMPAD_ENTER
WXK_NUMPAD_F1 = _core_.WXK_NUMPAD_F1
WXK_NUMPAD_F2 = _core_.WXK_NUMPAD_F2
WXK_NUMPAD_F3 = _core_.WXK_NUMPAD_F3
WXK_NUMPAD_F4 = _core_.WXK_NUMPAD_F4
WXK_NUMPAD_HOME = _core_.WXK_NUMPAD_HOME
WXK_NUMPAD_LEFT = _core_.WXK_NUMPAD_LEFT
WXK_NUMPAD_UP = _core_.WXK_NUMPAD_UP
WXK_NUMPAD_RIGHT = _core_.WXK_NUMPAD_RIGHT
WXK_NUMPAD_DOWN = _core_.WXK_NUMPAD_DOWN
WXK_NUMPAD_PRIOR = _core_.WXK_NUMPAD_PRIOR
WXK_NUMPAD_PAGEUP = _core_.WXK_NUMPAD_PAGEUP
WXK_NUMPAD_NEXT = _core_.WXK_NUMPAD_NEXT
WXK_NUMPAD_PAGEDOWN = _core_.WXK_NUMPAD_PAGEDOWN
WXK_NUMPAD_END = _core_.WXK_NUMPAD_END
WXK_NUMPAD_BEGIN = _core_.WXK_NUMPAD_BEGIN
WXK_NUMPAD_INSERT = _core_.WXK_NUMPAD_INSERT
WXK_NUMPAD_DELETE = _core_.WXK_NUMPAD_DELETE
WXK_NUMPAD_EQUAL = _core_.WXK_NUMPAD_EQUAL
WXK_NUMPAD_MULTIPLY = _core_.WXK_NUMPAD_MULTIPLY
WXK_NUMPAD_ADD = _core_.WXK_NUMPAD_ADD
WXK_NUMPAD_SEPARATOR = _core_.WXK_NUMPAD_SEPARATOR
WXK_NUMPAD_SUBTRACT = _core_.WXK_NUMPAD_SUBTRACT
WXK_NUMPAD_DECIMAL = _core_.WXK_NUMPAD_DECIMAL
WXK_NUMPAD_DIVIDE = _core_.WXK_NUMPAD_DIVIDE
WXK_WINDOWS_LEFT = _core_.WXK_WINDOWS_LEFT
WXK_WINDOWS_RIGHT = _core_.WXK_WINDOWS_RIGHT
WXK_WINDOWS_MENU = _core_.WXK_WINDOWS_MENU
PAPER_NONE = _core_.PAPER_NONE
PAPER_LETTER = _core_.PAPER_LETTER
PAPER_LEGAL = _core_.PAPER_LEGAL
PAPER_A4 = _core_.PAPER_A4
PAPER_CSHEET = _core_.PAPER_CSHEET
PAPER_DSHEET = _core_.PAPER_DSHEET
PAPER_ESHEET = _core_.PAPER_ESHEET
PAPER_LETTERSMALL = _core_.PAPER_LETTERSMALL
PAPER_TABLOID = _core_.PAPER_TABLOID
PAPER_LEDGER = _core_.PAPER_LEDGER
PAPER_STATEMENT = _core_.PAPER_STATEMENT
PAPER_EXECUTIVE = _core_.PAPER_EXECUTIVE
PAPER_A3 = _core_.PAPER_A3
PAPER_A4SMALL = _core_.PAPER_A4SMALL
PAPER_A5 = _core_.PAPER_A5
PAPER_B4 = _core_.PAPER_B4
PAPER_B5 = _core_.PAPER_B5
PAPER_FOLIO = _core_.PAPER_FOLIO
PAPER_QUARTO = _core_.PAPER_QUARTO
PAPER_10X14 = _core_.PAPER_10X14
PAPER_11X17 = _core_.PAPER_11X17
PAPER_NOTE = _core_.PAPER_NOTE
PAPER_ENV_9 = _core_.PAPER_ENV_9
PAPER_ENV_10 = _core_.PAPER_ENV_10
PAPER_ENV_11 = _core_.PAPER_ENV_11
PAPER_ENV_12 = _core_.PAPER_ENV_12
PAPER_ENV_14 = _core_.PAPER_ENV_14
PAPER_ENV_DL = _core_.PAPER_ENV_DL
PAPER_ENV_C5 = _core_.PAPER_ENV_C5
PAPER_ENV_C3 = _core_.PAPER_ENV_C3
PAPER_ENV_C4 = _core_.PAPER_ENV_C4
PAPER_ENV_C6 = _core_.PAPER_ENV_C6
PAPER_ENV_C65 = _core_.PAPER_ENV_C65
PAPER_ENV_B4 = _core_.PAPER_ENV_B4
PAPER_ENV_B5 = _core_.PAPER_ENV_B5
PAPER_ENV_B6 = _core_.PAPER_ENV_B6
PAPER_ENV_ITALY = _core_.PAPER_ENV_ITALY
PAPER_ENV_MONARCH = _core_.PAPER_ENV_MONARCH
PAPER_ENV_PERSONAL = _core_.PAPER_ENV_PERSONAL
PAPER_FANFOLD_US = _core_.PAPER_FANFOLD_US
PAPER_FANFOLD_STD_GERMAN = _core_.PAPER_FANFOLD_STD_GERMAN
PAPER_FANFOLD_LGL_GERMAN = _core_.PAPER_FANFOLD_LGL_GERMAN
PAPER_ISO_B4 = _core_.PAPER_ISO_B4
PAPER_JAPANESE_POSTCARD = _core_.PAPER_JAPANESE_POSTCARD
PAPER_9X11 = _core_.PAPER_9X11
PAPER_10X11 = _core_.PAPER_10X11
PAPER_15X11 = _core_.PAPER_15X11
PAPER_ENV_INVITE = _core_.PAPER_ENV_INVITE
PAPER_LETTER_EXTRA = _core_.PAPER_LETTER_EXTRA
PAPER_LEGAL_EXTRA = _core_.PAPER_LEGAL_EXTRA
PAPER_TABLOID_EXTRA = _core_.PAPER_TABLOID_EXTRA
PAPER_A4_EXTRA = _core_.PAPER_A4_EXTRA
PAPER_LETTER_TRANSVERSE = _core_.PAPER_LETTER_TRANSVERSE
PAPER_A4_TRANSVERSE = _core_.PAPER_A4_TRANSVERSE
PAPER_LETTER_EXTRA_TRANSVERSE = _core_.PAPER_LETTER_EXTRA_TRANSVERSE
PAPER_A_PLUS = _core_.PAPER_A_PLUS
PAPER_B_PLUS = _core_.PAPER_B_PLUS
PAPER_LETTER_PLUS = _core_.PAPER_LETTER_PLUS
PAPER_A4_PLUS = _core_.PAPER_A4_PLUS
PAPER_A5_TRANSVERSE = _core_.PAPER_A5_TRANSVERSE
PAPER_B5_TRANSVERSE = _core_.PAPER_B5_TRANSVERSE
PAPER_A3_EXTRA = _core_.PAPER_A3_EXTRA
PAPER_A5_EXTRA = _core_.PAPER_A5_EXTRA
PAPER_B5_EXTRA = _core_.PAPER_B5_EXTRA
PAPER_A2 = _core_.PAPER_A2
PAPER_A3_TRANSVERSE = _core_.PAPER_A3_TRANSVERSE
PAPER_A3_EXTRA_TRANSVERSE = _core_.PAPER_A3_EXTRA_TRANSVERSE
DUPLEX_SIMPLEX = _core_.DUPLEX_SIMPLEX
DUPLEX_HORIZONTAL = _core_.DUPLEX_HORIZONTAL
DUPLEX_VERTICAL = _core_.DUPLEX_VERTICAL
ITEM_SEPARATOR = _core_.ITEM_SEPARATOR
ITEM_NORMAL = _core_.ITEM_NORMAL
ITEM_CHECK = _core_.ITEM_CHECK
ITEM_RADIO = _core_.ITEM_RADIO
ITEM_MAX = _core_.ITEM_MAX
HT_NOWHERE = _core_.HT_NOWHERE
HT_SCROLLBAR_FIRST = _core_.HT_SCROLLBAR_FIRST
HT_SCROLLBAR_ARROW_LINE_1 = _core_.HT_SCROLLBAR_ARROW_LINE_1
HT_SCROLLBAR_ARROW_LINE_2 = _core_.HT_SCROLLBAR_ARROW_LINE_2
HT_SCROLLBAR_ARROW_PAGE_1 = _core_.HT_SCROLLBAR_ARROW_PAGE_1
HT_SCROLLBAR_ARROW_PAGE_2 = _core_.HT_SCROLLBAR_ARROW_PAGE_2
HT_SCROLLBAR_THUMB = _core_.HT_SCROLLBAR_THUMB
HT_SCROLLBAR_BAR_1 = _core_.HT_SCROLLBAR_BAR_1
HT_SCROLLBAR_BAR_2 = _core_.HT_SCROLLBAR_BAR_2
HT_SCROLLBAR_LAST = _core_.HT_SCROLLBAR_LAST
HT_WINDOW_OUTSIDE = _core_.HT_WINDOW_OUTSIDE
HT_WINDOW_INSIDE = _core_.HT_WINDOW_INSIDE
HT_WINDOW_VERT_SCROLLBAR = _core_.HT_WINDOW_VERT_SCROLLBAR
HT_WINDOW_HORZ_SCROLLBAR = _core_.HT_WINDOW_HORZ_SCROLLBAR
HT_WINDOW_CORNER = _core_.HT_WINDOW_CORNER
HT_MAX = _core_.HT_MAX
MOD_NONE = _core_.MOD_NONE
MOD_ALT = _core_.MOD_ALT
MOD_CONTROL = _core_.MOD_CONTROL
MOD_SHIFT = _core_.MOD_SHIFT
MOD_WIN = _core_.MOD_WIN
UPDATE_UI_NONE = _core_.UPDATE_UI_NONE
UPDATE_UI_RECURSE = _core_.UPDATE_UI_RECURSE
UPDATE_UI_FROMIDLE = _core_.UPDATE_UI_FROMIDLE
#---------------------------------------------------------------------------

class Object(object):
    """
    The base class for most wx objects, although in wxPython not
    much functionality is needed nor exposed.
    """
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxObject instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetClassName(*args, **kwargs):
        """
        GetClassName(self) -> String

        Returns the class name of the C++ class using wxRTTI.
        """
        return _core_.Object_GetClassName(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """
        Destroy(self)

        Deletes the C++ object this Python object is a proxy for.
        """
        return _core_.Object_Destroy(*args, **kwargs)


class ObjectPtr(Object):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Object
_core_.Object_swigregister(ObjectPtr)
_wxPySetDictionary = _core_._wxPySetDictionary

_wxPyFixStockObjects = _core_._wxPyFixStockObjects

cvar = _core_.cvar
EmptyString = cvar.EmptyString

#---------------------------------------------------------------------------

BITMAP_TYPE_INVALID = _core_.BITMAP_TYPE_INVALID
BITMAP_TYPE_BMP = _core_.BITMAP_TYPE_BMP
BITMAP_TYPE_ICO = _core_.BITMAP_TYPE_ICO
BITMAP_TYPE_CUR = _core_.BITMAP_TYPE_CUR
BITMAP_TYPE_XBM = _core_.BITMAP_TYPE_XBM
BITMAP_TYPE_XBM_DATA = _core_.BITMAP_TYPE_XBM_DATA
BITMAP_TYPE_XPM = _core_.BITMAP_TYPE_XPM
BITMAP_TYPE_XPM_DATA = _core_.BITMAP_TYPE_XPM_DATA
BITMAP_TYPE_TIF = _core_.BITMAP_TYPE_TIF
BITMAP_TYPE_GIF = _core_.BITMAP_TYPE_GIF
BITMAP_TYPE_PNG = _core_.BITMAP_TYPE_PNG
BITMAP_TYPE_JPEG = _core_.BITMAP_TYPE_JPEG
BITMAP_TYPE_PNM = _core_.BITMAP_TYPE_PNM
BITMAP_TYPE_PCX = _core_.BITMAP_TYPE_PCX
BITMAP_TYPE_PICT = _core_.BITMAP_TYPE_PICT
BITMAP_TYPE_ICON = _core_.BITMAP_TYPE_ICON
BITMAP_TYPE_ANI = _core_.BITMAP_TYPE_ANI
BITMAP_TYPE_IFF = _core_.BITMAP_TYPE_IFF
BITMAP_TYPE_MACCURSOR = _core_.BITMAP_TYPE_MACCURSOR
BITMAP_TYPE_ANY = _core_.BITMAP_TYPE_ANY
CURSOR_NONE = _core_.CURSOR_NONE
CURSOR_ARROW = _core_.CURSOR_ARROW
CURSOR_RIGHT_ARROW = _core_.CURSOR_RIGHT_ARROW
CURSOR_BULLSEYE = _core_.CURSOR_BULLSEYE
CURSOR_CHAR = _core_.CURSOR_CHAR
CURSOR_CROSS = _core_.CURSOR_CROSS
CURSOR_HAND = _core_.CURSOR_HAND
CURSOR_IBEAM = _core_.CURSOR_IBEAM
CURSOR_LEFT_BUTTON = _core_.CURSOR_LEFT_BUTTON
CURSOR_MAGNIFIER = _core_.CURSOR_MAGNIFIER
CURSOR_MIDDLE_BUTTON = _core_.CURSOR_MIDDLE_BUTTON
CURSOR_NO_ENTRY = _core_.CURSOR_NO_ENTRY
CURSOR_PAINT_BRUSH = _core_.CURSOR_PAINT_BRUSH
CURSOR_PENCIL = _core_.CURSOR_PENCIL
CURSOR_POINT_LEFT = _core_.CURSOR_POINT_LEFT
CURSOR_POINT_RIGHT = _core_.CURSOR_POINT_RIGHT
CURSOR_QUESTION_ARROW = _core_.CURSOR_QUESTION_ARROW
CURSOR_RIGHT_BUTTON = _core_.CURSOR_RIGHT_BUTTON
CURSOR_SIZENESW = _core_.CURSOR_SIZENESW
CURSOR_SIZENS = _core_.CURSOR_SIZENS
CURSOR_SIZENWSE = _core_.CURSOR_SIZENWSE
CURSOR_SIZEWE = _core_.CURSOR_SIZEWE
CURSOR_SIZING = _core_.CURSOR_SIZING
CURSOR_SPRAYCAN = _core_.CURSOR_SPRAYCAN
CURSOR_WAIT = _core_.CURSOR_WAIT
CURSOR_WATCH = _core_.CURSOR_WATCH
CURSOR_BLANK = _core_.CURSOR_BLANK
CURSOR_DEFAULT = _core_.CURSOR_DEFAULT
CURSOR_COPY_ARROW = _core_.CURSOR_COPY_ARROW
CURSOR_ARROWWAIT = _core_.CURSOR_ARROWWAIT
CURSOR_MAX = _core_.CURSOR_MAX
#---------------------------------------------------------------------------

class Size(object):
    """
    wx.Size is a useful data structure used to represent the size of
    something.  It simply contians integer width and height
    proprtites.  In most places in wxPython where a wx.Size is
    expected a (width, height) tuple can be used instead.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSize instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    width = property(_core_.Size_width_get, _core_.Size_width_set)
    height = property(_core_.Size_height_get, _core_.Size_height_set)
    x = width; y = height 
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int w=0, int h=0) -> Size

        Creates a size object.
        """
        newobj = _core_.new_Size(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_Size):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def __eq__(*args, **kwargs):
        """
        __eq__(self, Size sz) -> bool

        Test for equality of wx.Size objects.
        """
        return _core_.Size___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, Size sz) -> bool

        Test for inequality.
        """
        return _core_.Size___ne__(*args, **kwargs)

    def __add__(*args, **kwargs):
        """
        __add__(self, Size sz) -> Size

        Add sz's proprties to this and return the result.
        """
        return _core_.Size___add__(*args, **kwargs)

    def __sub__(*args, **kwargs):
        """
        __sub__(self, Size sz) -> Size

        Subtract sz's properties from this and return the result.
        """
        return _core_.Size___sub__(*args, **kwargs)

    def IncTo(*args, **kwargs):
        """
        IncTo(self, Size sz)

        Increments this object so that both of its dimensions are not less
        than the corresponding dimensions of the size.
        """
        return _core_.Size_IncTo(*args, **kwargs)

    def DecTo(*args, **kwargs):
        """
        DecTo(self, Size sz)

        Decrements this object so that both of its dimensions are not greater
        than the corresponding dimensions of the size.
        """
        return _core_.Size_DecTo(*args, **kwargs)

    def Set(*args, **kwargs):
        """
        Set(self, int w, int h)

        Set both width and height.
        """
        return _core_.Size_Set(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(self, int w)"""
        return _core_.Size_SetWidth(*args, **kwargs)

    def SetHeight(*args, **kwargs):
        """SetHeight(self, int h)"""
        return _core_.Size_SetHeight(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _core_.Size_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return _core_.Size_GetHeight(*args, **kwargs)

    def IsFullySpecified(*args, **kwargs):
        """
        IsFullySpecified(self) -> bool

        Returns True if both components of the size are non-default values.
        """
        return _core_.Size_IsFullySpecified(*args, **kwargs)

    def SetDefaults(*args, **kwargs):
        """
        SetDefaults(self, Size size)

        Combine this size with the other one replacing the default components
        of this object (i.e. equal to -1) with those of the other.
        """
        return _core_.Size_SetDefaults(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (width,height)

        Returns the width and height properties as a tuple.
        """
        return _core_.Size_Get(*args, **kwargs)

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


class SizePtr(Size):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Size
_core_.Size_swigregister(SizePtr)

#---------------------------------------------------------------------------

class RealPoint(object):
    """
    A data structure for representing a point or position with floating
    point x and y properties.  In wxPython most places that expect a
    wx.RealPoint can also accept a (x,y) tuple.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRealPoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    x = property(_core_.RealPoint_x_get, _core_.RealPoint_x_set)
    y = property(_core_.RealPoint_y_get, _core_.RealPoint_y_set)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, double x=0.0, double y=0.0) -> RealPoint

        Create a wx.RealPoint object
        """
        newobj = _core_.new_RealPoint(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_RealPoint):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def __eq__(*args, **kwargs):
        """
        __eq__(self, RealPoint pt) -> bool

        Test for equality of wx.RealPoint objects.
        """
        return _core_.RealPoint___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, RealPoint pt) -> bool

        Test for inequality of wx.RealPoint objects.
        """
        return _core_.RealPoint___ne__(*args, **kwargs)

    def __add__(*args, **kwargs):
        """
        __add__(self, RealPoint pt) -> RealPoint

        Add pt's proprties to this and return the result.
        """
        return _core_.RealPoint___add__(*args, **kwargs)

    def __sub__(*args, **kwargs):
        """
        __sub__(self, RealPoint pt) -> RealPoint

        Subtract pt's proprties from this and return the result
        """
        return _core_.RealPoint___sub__(*args, **kwargs)

    def Set(*args, **kwargs):
        """
        Set(self, double x, double y)

        Set both the x and y properties
        """
        return _core_.RealPoint_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (x,y)

        Return the x and y properties as a tuple. 
        """
        return _core_.RealPoint_Get(*args, **kwargs)

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


class RealPointPtr(RealPoint):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RealPoint
_core_.RealPoint_swigregister(RealPointPtr)

#---------------------------------------------------------------------------

class Point(object):
    """
    A data structure for representing a point or position with integer x
    and y properties.  Most places in wxPython that expect a wx.Point can
    also accept a (x,y) tuple.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    x = property(_core_.Point_x_get, _core_.Point_x_set)
    y = property(_core_.Point_y_get, _core_.Point_y_set)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int x=0, int y=0) -> Point

        Create a wx.Point object
        """
        newobj = _core_.new_Point(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_Point):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def __eq__(*args, **kwargs):
        """
        __eq__(self, Point pt) -> bool

        Test for equality of wx.Point objects.
        """
        return _core_.Point___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, Point pt) -> bool

        Test for inequality of wx.Point objects.
        """
        return _core_.Point___ne__(*args, **kwargs)

    def __add__(*args, **kwargs):
        """
        __add__(self, Point pt) -> Point

        Add pt's proprties to this and return the result.
        """
        return _core_.Point___add__(*args, **kwargs)

    def __sub__(*args, **kwargs):
        """
        __sub__(self, Point pt) -> Point

        Subtract pt's proprties from this and return the result
        """
        return _core_.Point___sub__(*args, **kwargs)

    def __iadd__(*args, **kwargs):
        """
        __iadd__(self, Point pt) -> Point

        Add pt to this object.
        """
        return _core_.Point___iadd__(*args, **kwargs)

    def __isub__(*args, **kwargs):
        """
        __isub__(self, Point pt) -> Point

        Subtract pt from this object.
        """
        return _core_.Point___isub__(*args, **kwargs)

    def Set(*args, **kwargs):
        """
        Set(self, long x, long y)

        Set both the x and y properties
        """
        return _core_.Point_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (x,y)

        Return the x and y properties as a tuple. 
        """
        return _core_.Point_Get(*args, **kwargs)

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


class PointPtr(Point):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Point
_core_.Point_swigregister(PointPtr)

#---------------------------------------------------------------------------

class Rect(object):
    """
    A class for representing and manipulating rectangles.  It has x, y,
    width and height properties.  In wxPython most palces that expect a
    wx.Rect can also accept a (x,y,width,height) tuple.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRect instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int x=0, int y=0, int width=0, int height=0) -> Rect

        Create a new Rect object.
        """
        newobj = _core_.new_Rect(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_Rect):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetX(*args, **kwargs):
        """GetX(self) -> int"""
        return _core_.Rect_GetX(*args, **kwargs)

    def SetX(*args, **kwargs):
        """SetX(self, int x)"""
        return _core_.Rect_SetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return _core_.Rect_GetY(*args, **kwargs)

    def SetY(*args, **kwargs):
        """SetY(self, int y)"""
        return _core_.Rect_SetY(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _core_.Rect_GetWidth(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(self, int w)"""
        return _core_.Rect_SetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return _core_.Rect_GetHeight(*args, **kwargs)

    def SetHeight(*args, **kwargs):
        """SetHeight(self, int h)"""
        return _core_.Rect_SetHeight(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _core_.Rect_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, Point p)"""
        return _core_.Rect_SetPosition(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self) -> Size"""
        return _core_.Rect_GetSize(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, Size s)"""
        return _core_.Rect_SetSize(*args, **kwargs)

    def GetTopLeft(*args, **kwargs):
        """GetTopLeft(self) -> Point"""
        return _core_.Rect_GetTopLeft(*args, **kwargs)

    def SetTopLeft(*args, **kwargs):
        """SetTopLeft(self, Point p)"""
        return _core_.Rect_SetTopLeft(*args, **kwargs)

    def GetBottomRight(*args, **kwargs):
        """GetBottomRight(self) -> Point"""
        return _core_.Rect_GetBottomRight(*args, **kwargs)

    def SetBottomRight(*args, **kwargs):
        """SetBottomRight(self, Point p)"""
        return _core_.Rect_SetBottomRight(*args, **kwargs)

    def GetLeft(*args, **kwargs):
        """GetLeft(self) -> int"""
        return _core_.Rect_GetLeft(*args, **kwargs)

    def GetTop(*args, **kwargs):
        """GetTop(self) -> int"""
        return _core_.Rect_GetTop(*args, **kwargs)

    def GetBottom(*args, **kwargs):
        """GetBottom(self) -> int"""
        return _core_.Rect_GetBottom(*args, **kwargs)

    def GetRight(*args, **kwargs):
        """GetRight(self) -> int"""
        return _core_.Rect_GetRight(*args, **kwargs)

    def SetLeft(*args, **kwargs):
        """SetLeft(self, int left)"""
        return _core_.Rect_SetLeft(*args, **kwargs)

    def SetRight(*args, **kwargs):
        """SetRight(self, int right)"""
        return _core_.Rect_SetRight(*args, **kwargs)

    def SetTop(*args, **kwargs):
        """SetTop(self, int top)"""
        return _core_.Rect_SetTop(*args, **kwargs)

    def SetBottom(*args, **kwargs):
        """SetBottom(self, int bottom)"""
        return _core_.Rect_SetBottom(*args, **kwargs)

    position = property(GetPosition, SetPosition)
    size = property(GetSize, SetSize)
    left = property(GetLeft, SetLeft)
    right = property(GetRight, SetRight)
    top = property(GetTop, SetTop)
    bottom = property(GetBottom, SetBottom)

    def Inflate(*args, **kwargs):
        """
        Inflate(self, int dx, int dy) -> Rect

        Increase the rectangle size by dx in x direction and dy in y
        direction. Both (or one of) parameters may be negative to decrease the
        rectangle size.
        """
        return _core_.Rect_Inflate(*args, **kwargs)

    def Deflate(*args, **kwargs):
        """
        Deflate(self, int dx, int dy) -> Rect

        Decrease the rectangle size by dx in x direction and dy in y
        direction. Both (or one of) parameters may be negative to increase the
        rectngle size. This method is the opposite of Inflate.
        """
        return _core_.Rect_Deflate(*args, **kwargs)

    def OffsetXY(*args, **kwargs):
        """
        OffsetXY(self, int dx, int dy)

        Moves the rectangle by the specified offset. If dx is positive, the
        rectangle is moved to the right, if dy is positive, it is moved to the
        bottom, otherwise it is moved to the left or top respectively.
        """
        return _core_.Rect_OffsetXY(*args, **kwargs)

    def Offset(*args, **kwargs):
        """
        Offset(self, Point pt)

        Same as OffsetXY but uses dx,dy from Point
        """
        return _core_.Rect_Offset(*args, **kwargs)

    def Intersect(*args, **kwargs):
        """
        Intersect(self, Rect rect) -> Rect

        Returns the intersectsion of this rectangle and rect.
        """
        return _core_.Rect_Intersect(*args, **kwargs)

    def Union(*args, **kwargs):
        """
        Union(self, Rect rect) -> Rect

        Returns the union of this rectangle and rect.
        """
        return _core_.Rect_Union(*args, **kwargs)

    def __add__(*args, **kwargs):
        """
        __add__(self, Rect rect) -> Rect

        Add the properties of rect to this rectangle and return the result.
        """
        return _core_.Rect___add__(*args, **kwargs)

    def __iadd__(*args, **kwargs):
        """
        __iadd__(self, Rect rect) -> Rect

        Add the properties of rect to this rectangle, updating this rectangle.
        """
        return _core_.Rect___iadd__(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """
        __eq__(self, Rect rect) -> bool

        Test for equality.
        """
        return _core_.Rect___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, Rect rect) -> bool

        Test for inequality.
        """
        return _core_.Rect___ne__(*args, **kwargs)

    def InsideXY(*args, **kwargs):
        """
        InsideXY(self, int x, int y) -> bool

        Return True if the point is (not strcitly) inside the rect.
        """
        return _core_.Rect_InsideXY(*args, **kwargs)

    def Inside(*args, **kwargs):
        """
        Inside(self, Point pt) -> bool

        Return True if the point is (not strcitly) inside the rect.
        """
        return _core_.Rect_Inside(*args, **kwargs)

    def Intersects(*args, **kwargs):
        """
        Intersects(self, Rect rect) -> bool

        Returns True if the rectangles have a non empty intersection.
        """
        return _core_.Rect_Intersects(*args, **kwargs)

    x = property(_core_.Rect_x_get, _core_.Rect_x_set)
    y = property(_core_.Rect_y_get, _core_.Rect_y_set)
    width = property(_core_.Rect_width_get, _core_.Rect_width_set)
    height = property(_core_.Rect_height_get, _core_.Rect_height_set)
    def Set(*args, **kwargs):
        """
        Set(self, int x=0, int y=0, int width=0, int height=0)

        Set all rectangle properties.
        """
        return _core_.Rect_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (x,y,width,height)

        Return the rectangle properties as a tuple.
        """
        return _core_.Rect_Get(*args, **kwargs)

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


class RectPtr(Rect):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Rect
_core_.Rect_swigregister(RectPtr)

def RectPP(*args, **kwargs):
    """
    RectPP(Point topLeft, Point bottomRight) -> Rect

    Create a new Rect object from Points representing two corners.
    """
    val = _core_.new_RectPP(*args, **kwargs)
    val.thisown = 1
    return val

def RectPS(*args, **kwargs):
    """
    RectPS(Point pos, Size size) -> Rect

    Create a new Rect from a position and size.
    """
    val = _core_.new_RectPS(*args, **kwargs)
    val.thisown = 1
    return val

def RectS(*args, **kwargs):
    """
    RectS(Size size) -> Rect

    Create a new Rect from a size only.
    """
    val = _core_.new_RectS(*args, **kwargs)
    val.thisown = 1
    return val


def IntersectRect(*args, **kwargs):
    """
    IntersectRect(Rect r1, Rect r2) -> Rect

    Calculate and return the intersection of r1 and r2.
    """
    return _core_.IntersectRect(*args, **kwargs)
#---------------------------------------------------------------------------

class Point2D(object):
    """
    wx.Point2Ds represent a point or a vector in a 2d coordinate system
    with floating point values.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPoint2D instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, double x=0.0, double y=0.0) -> Point2D

        Create a w.Point2D object.
        """
        newobj = _core_.new_Point2D(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetFloor(*args, **kwargs):
        """
        GetFloor() -> (x,y)

        Convert to integer
        """
        return _core_.Point2D_GetFloor(*args, **kwargs)

    def GetRounded(*args, **kwargs):
        """
        GetRounded() -> (x,y)

        Convert to integer
        """
        return _core_.Point2D_GetRounded(*args, **kwargs)

    def GetVectorLength(*args, **kwargs):
        """GetVectorLength(self) -> double"""
        return _core_.Point2D_GetVectorLength(*args, **kwargs)

    def GetVectorAngle(*args, **kwargs):
        """GetVectorAngle(self) -> double"""
        return _core_.Point2D_GetVectorAngle(*args, **kwargs)

    def SetVectorLength(*args, **kwargs):
        """SetVectorLength(self, double length)"""
        return _core_.Point2D_SetVectorLength(*args, **kwargs)

    def SetVectorAngle(*args, **kwargs):
        """SetVectorAngle(self, double degrees)"""
        return _core_.Point2D_SetVectorAngle(*args, **kwargs)

    def SetPolarCoordinates(self, angle, length):
        self.SetVectorLength(length)
        self.SetVectorAngle(angle)
    def Normalize(self):
        self.SetVectorLength(1.0)

    def GetDistance(*args, **kwargs):
        """GetDistance(self, Point2D pt) -> double"""
        return _core_.Point2D_GetDistance(*args, **kwargs)

    def GetDistanceSquare(*args, **kwargs):
        """GetDistanceSquare(self, Point2D pt) -> double"""
        return _core_.Point2D_GetDistanceSquare(*args, **kwargs)

    def GetDotProduct(*args, **kwargs):
        """GetDotProduct(self, Point2D vec) -> double"""
        return _core_.Point2D_GetDotProduct(*args, **kwargs)

    def GetCrossProduct(*args, **kwargs):
        """GetCrossProduct(self, Point2D vec) -> double"""
        return _core_.Point2D_GetCrossProduct(*args, **kwargs)

    def __neg__(*args, **kwargs):
        """
        __neg__(self) -> Point2D

        the reflection of this point
        """
        return _core_.Point2D___neg__(*args, **kwargs)

    def __iadd__(*args, **kwargs):
        """__iadd__(self, Point2D pt) -> Point2D"""
        return _core_.Point2D___iadd__(*args, **kwargs)

    def __isub__(*args, **kwargs):
        """__isub__(self, Point2D pt) -> Point2D"""
        return _core_.Point2D___isub__(*args, **kwargs)

    def __imul__(*args, **kwargs):
        """__imul__(self, Point2D pt) -> Point2D"""
        return _core_.Point2D___imul__(*args, **kwargs)

    def __idiv__(*args, **kwargs):
        """__idiv__(self, Point2D pt) -> Point2D"""
        return _core_.Point2D___idiv__(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """
        __eq__(self, Point2D pt) -> bool

        Test for equality
        """
        return _core_.Point2D___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, Point2D pt) -> bool

        Test for inequality
        """
        return _core_.Point2D___ne__(*args, **kwargs)

    x = property(_core_.Point2D_x_get, _core_.Point2D_x_set)
    y = property(_core_.Point2D_y_get, _core_.Point2D_y_set)
    def Set(*args, **kwargs):
        """Set(self, double x=0, double y=0)"""
        return _core_.Point2D_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (x,y)

        Return x and y properties as a tuple.
        """
        return _core_.Point2D_Get(*args, **kwargs)

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


class Point2DPtr(Point2D):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Point2D
_core_.Point2D_swigregister(Point2DPtr)

def Point2DCopy(*args, **kwargs):
    """
    Point2DCopy(Point2D pt) -> Point2D

    Create a w.Point2D object.
    """
    val = _core_.new_Point2DCopy(*args, **kwargs)
    val.thisown = 1
    return val

def Point2DFromPoint(*args, **kwargs):
    """
    Point2DFromPoint(Point pt) -> Point2D

    Create a w.Point2D object.
    """
    val = _core_.new_Point2DFromPoint(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

FromStart = _core_.FromStart
FromCurrent = _core_.FromCurrent
FromEnd = _core_.FromEnd
class InputStream(object):
    """Proxy of C++ InputStream class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyInputStream instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, PyObject p) -> InputStream"""
        newobj = _core_.new_InputStream(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_InputStream):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def close(*args, **kwargs):
        """close(self)"""
        return _core_.InputStream_close(*args, **kwargs)

    def flush(*args, **kwargs):
        """flush(self)"""
        return _core_.InputStream_flush(*args, **kwargs)

    def eof(*args, **kwargs):
        """eof(self) -> bool"""
        return _core_.InputStream_eof(*args, **kwargs)

    def read(*args, **kwargs):
        """read(self, int size=-1) -> PyObject"""
        return _core_.InputStream_read(*args, **kwargs)

    def readline(*args, **kwargs):
        """readline(self, int size=-1) -> PyObject"""
        return _core_.InputStream_readline(*args, **kwargs)

    def readlines(*args, **kwargs):
        """readlines(self, int sizehint=-1) -> PyObject"""
        return _core_.InputStream_readlines(*args, **kwargs)

    def seek(*args, **kwargs):
        """seek(self, int offset, int whence=0)"""
        return _core_.InputStream_seek(*args, **kwargs)

    def tell(*args, **kwargs):
        """tell(self) -> int"""
        return _core_.InputStream_tell(*args, **kwargs)

    def Peek(*args, **kwargs):
        """Peek(self) -> char"""
        return _core_.InputStream_Peek(*args, **kwargs)

    def GetC(*args, **kwargs):
        """GetC(self) -> char"""
        return _core_.InputStream_GetC(*args, **kwargs)

    def LastRead(*args, **kwargs):
        """LastRead(self) -> size_t"""
        return _core_.InputStream_LastRead(*args, **kwargs)

    def CanRead(*args, **kwargs):
        """CanRead(self) -> bool"""
        return _core_.InputStream_CanRead(*args, **kwargs)

    def Eof(*args, **kwargs):
        """Eof(self) -> bool"""
        return _core_.InputStream_Eof(*args, **kwargs)

    def Ungetch(*args, **kwargs):
        """Ungetch(self, char c) -> bool"""
        return _core_.InputStream_Ungetch(*args, **kwargs)

    def SeekI(*args, **kwargs):
        """SeekI(self, long pos, int mode=FromStart) -> long"""
        return _core_.InputStream_SeekI(*args, **kwargs)

    def TellI(*args, **kwargs):
        """TellI(self) -> long"""
        return _core_.InputStream_TellI(*args, **kwargs)


class InputStreamPtr(InputStream):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = InputStream
_core_.InputStream_swigregister(InputStreamPtr)
DefaultPosition = cvar.DefaultPosition
DefaultSize = cvar.DefaultSize

class OutputStream(object):
    """Proxy of C++ OutputStream class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxOutputStream instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def write(*args, **kwargs):
        """write(self, PyObject obj)"""
        return _core_.OutputStream_write(*args, **kwargs)


class OutputStreamPtr(OutputStream):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = OutputStream
_core_.OutputStream_swigregister(OutputStreamPtr)

#---------------------------------------------------------------------------

class FSFile(Object):
    """Proxy of C++ FSFile class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFSFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, InputStream stream, String loc, String mimetype, String anchor, 
            DateTime modif) -> FSFile
        """
        newobj = _core_.new_FSFile(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self.thisown = 0   # It will normally be deleted by the user of the wxFileSystem

    def __del__(self, destroy=_core_.delete_FSFile):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetStream(*args, **kwargs):
        """GetStream(self) -> InputStream"""
        return _core_.FSFile_GetStream(*args, **kwargs)

    def GetMimeType(*args, **kwargs):
        """GetMimeType(self) -> String"""
        return _core_.FSFile_GetMimeType(*args, **kwargs)

    def GetLocation(*args, **kwargs):
        """GetLocation(self) -> String"""
        return _core_.FSFile_GetLocation(*args, **kwargs)

    def GetAnchor(*args, **kwargs):
        """GetAnchor(self) -> String"""
        return _core_.FSFile_GetAnchor(*args, **kwargs)

    def GetModificationTime(*args, **kwargs):
        """GetModificationTime(self) -> DateTime"""
        return _core_.FSFile_GetModificationTime(*args, **kwargs)


class FSFilePtr(FSFile):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FSFile
_core_.FSFile_swigregister(FSFilePtr)

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
_core_.CPPFileSystemHandler_swigregister(CPPFileSystemHandlerPtr)

class FileSystemHandler(CPPFileSystemHandler):
    """Proxy of C++ FileSystemHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyFileSystemHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> FileSystemHandler"""
        newobj = _core_.new_FileSystemHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, FileSystemHandler)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _core_.FileSystemHandler__setCallbackInfo(*args, **kwargs)

    def CanOpen(*args, **kwargs):
        """CanOpen(self, String location) -> bool"""
        return _core_.FileSystemHandler_CanOpen(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, FileSystem fs, String location) -> FSFile"""
        return _core_.FileSystemHandler_OpenFile(*args, **kwargs)

    def FindFirst(*args, **kwargs):
        """FindFirst(self, String spec, int flags=0) -> String"""
        return _core_.FileSystemHandler_FindFirst(*args, **kwargs)

    def FindNext(*args, **kwargs):
        """FindNext(self) -> String"""
        return _core_.FileSystemHandler_FindNext(*args, **kwargs)

    def GetProtocol(*args, **kwargs):
        """GetProtocol(self, String location) -> String"""
        return _core_.FileSystemHandler_GetProtocol(*args, **kwargs)

    def GetLeftLocation(*args, **kwargs):
        """GetLeftLocation(self, String location) -> String"""
        return _core_.FileSystemHandler_GetLeftLocation(*args, **kwargs)

    def GetAnchor(*args, **kwargs):
        """GetAnchor(self, String location) -> String"""
        return _core_.FileSystemHandler_GetAnchor(*args, **kwargs)

    def GetRightLocation(*args, **kwargs):
        """GetRightLocation(self, String location) -> String"""
        return _core_.FileSystemHandler_GetRightLocation(*args, **kwargs)

    def GetMimeTypeFromExt(*args, **kwargs):
        """GetMimeTypeFromExt(self, String location) -> String"""
        return _core_.FileSystemHandler_GetMimeTypeFromExt(*args, **kwargs)


class FileSystemHandlerPtr(FileSystemHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FileSystemHandler
_core_.FileSystemHandler_swigregister(FileSystemHandlerPtr)

class FileSystem(Object):
    """Proxy of C++ FileSystem class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFileSystem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> FileSystem"""
        newobj = _core_.new_FileSystem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_FileSystem):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def ChangePathTo(*args, **kwargs):
        """ChangePathTo(self, String location, bool is_dir=False)"""
        return _core_.FileSystem_ChangePathTo(*args, **kwargs)

    def GetPath(*args, **kwargs):
        """GetPath(self) -> String"""
        return _core_.FileSystem_GetPath(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, String location) -> FSFile"""
        return _core_.FileSystem_OpenFile(*args, **kwargs)

    def FindFirst(*args, **kwargs):
        """FindFirst(self, String spec, int flags=0) -> String"""
        return _core_.FileSystem_FindFirst(*args, **kwargs)

    def FindNext(*args, **kwargs):
        """FindNext(self) -> String"""
        return _core_.FileSystem_FindNext(*args, **kwargs)

    def AddHandler(*args, **kwargs):
        """AddHandler(CPPFileSystemHandler handler)"""
        return _core_.FileSystem_AddHandler(*args, **kwargs)

    AddHandler = staticmethod(AddHandler)
    def CleanUpHandlers(*args, **kwargs):
        """CleanUpHandlers()"""
        return _core_.FileSystem_CleanUpHandlers(*args, **kwargs)

    CleanUpHandlers = staticmethod(CleanUpHandlers)
    def FileNameToURL(*args, **kwargs):
        """FileNameToURL(String filename) -> String"""
        return _core_.FileSystem_FileNameToURL(*args, **kwargs)

    FileNameToURL = staticmethod(FileNameToURL)
    def URLToFileName(*args, **kwargs):
        """URLToFileName(String url) -> String"""
        return _core_.FileSystem_URLToFileName(*args, **kwargs)

    URLToFileName = staticmethod(URLToFileName)

class FileSystemPtr(FileSystem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FileSystem
_core_.FileSystem_swigregister(FileSystemPtr)

def FileSystem_AddHandler(*args, **kwargs):
    """FileSystem_AddHandler(CPPFileSystemHandler handler)"""
    return _core_.FileSystem_AddHandler(*args, **kwargs)

def FileSystem_CleanUpHandlers(*args, **kwargs):
    """FileSystem_CleanUpHandlers()"""
    return _core_.FileSystem_CleanUpHandlers(*args, **kwargs)

def FileSystem_FileNameToURL(*args, **kwargs):
    """FileSystem_FileNameToURL(String filename) -> String"""
    return _core_.FileSystem_FileNameToURL(*args, **kwargs)

def FileSystem_URLToFileName(*args, **kwargs):
    """FileSystem_URLToFileName(String url) -> String"""
    return _core_.FileSystem_URLToFileName(*args, **kwargs)

class InternetFSHandler(CPPFileSystemHandler):
    """Proxy of C++ InternetFSHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxInternetFSHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> InternetFSHandler"""
        newobj = _core_.new_InternetFSHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def CanOpen(*args, **kwargs):
        """CanOpen(self, String location) -> bool"""
        return _core_.InternetFSHandler_CanOpen(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, FileSystem fs, String location) -> FSFile"""
        return _core_.InternetFSHandler_OpenFile(*args, **kwargs)


class InternetFSHandlerPtr(InternetFSHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = InternetFSHandler
_core_.InternetFSHandler_swigregister(InternetFSHandlerPtr)

class ZipFSHandler(CPPFileSystemHandler):
    """Proxy of C++ ZipFSHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxZipFSHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> ZipFSHandler"""
        newobj = _core_.new_ZipFSHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def CanOpen(*args, **kwargs):
        """CanOpen(self, String location) -> bool"""
        return _core_.ZipFSHandler_CanOpen(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, FileSystem fs, String location) -> FSFile"""
        return _core_.ZipFSHandler_OpenFile(*args, **kwargs)

    def FindFirst(*args, **kwargs):
        """FindFirst(self, String spec, int flags=0) -> String"""
        return _core_.ZipFSHandler_FindFirst(*args, **kwargs)

    def FindNext(*args, **kwargs):
        """FindNext(self) -> String"""
        return _core_.ZipFSHandler_FindNext(*args, **kwargs)


class ZipFSHandlerPtr(ZipFSHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ZipFSHandler
_core_.ZipFSHandler_swigregister(ZipFSHandlerPtr)


def __wxMemoryFSHandler_AddFile_wxImage(*args, **kwargs):
    """__wxMemoryFSHandler_AddFile_wxImage(String filename, Image image, long type)"""
    return _core_.__wxMemoryFSHandler_AddFile_wxImage(*args, **kwargs)

def __wxMemoryFSHandler_AddFile_wxBitmap(*args, **kwargs):
    """__wxMemoryFSHandler_AddFile_wxBitmap(String filename, Bitmap bitmap, long type)"""
    return _core_.__wxMemoryFSHandler_AddFile_wxBitmap(*args, **kwargs)

def __wxMemoryFSHandler_AddFile_Data(*args, **kwargs):
    """__wxMemoryFSHandler_AddFile_Data(String filename, PyObject data)"""
    return _core_.__wxMemoryFSHandler_AddFile_Data(*args, **kwargs)
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

class MemoryFSHandler(CPPFileSystemHandler):
    """Proxy of C++ MemoryFSHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMemoryFSHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> MemoryFSHandler"""
        newobj = _core_.new_MemoryFSHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def RemoveFile(*args, **kwargs):
        """RemoveFile(String filename)"""
        return _core_.MemoryFSHandler_RemoveFile(*args, **kwargs)

    RemoveFile = staticmethod(RemoveFile)
    AddFile = staticmethod(MemoryFSHandler_AddFile) 
    def CanOpen(*args, **kwargs):
        """CanOpen(self, String location) -> bool"""
        return _core_.MemoryFSHandler_CanOpen(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, FileSystem fs, String location) -> FSFile"""
        return _core_.MemoryFSHandler_OpenFile(*args, **kwargs)

    def FindFirst(*args, **kwargs):
        """FindFirst(self, String spec, int flags=0) -> String"""
        return _core_.MemoryFSHandler_FindFirst(*args, **kwargs)

    def FindNext(*args, **kwargs):
        """FindNext(self) -> String"""
        return _core_.MemoryFSHandler_FindNext(*args, **kwargs)


class MemoryFSHandlerPtr(MemoryFSHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MemoryFSHandler
_core_.MemoryFSHandler_swigregister(MemoryFSHandlerPtr)

def MemoryFSHandler_RemoveFile(*args, **kwargs):
    """MemoryFSHandler_RemoveFile(String filename)"""
    return _core_.MemoryFSHandler_RemoveFile(*args, **kwargs)

#---------------------------------------------------------------------------

class ImageHandler(Object):
    """Proxy of C++ ImageHandler class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxImageHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetName(*args, **kwargs):
        """GetName(self) -> String"""
        return _core_.ImageHandler_GetName(*args, **kwargs)

    def GetExtension(*args, **kwargs):
        """GetExtension(self) -> String"""
        return _core_.ImageHandler_GetExtension(*args, **kwargs)

    def GetType(*args, **kwargs):
        """GetType(self) -> long"""
        return _core_.ImageHandler_GetType(*args, **kwargs)

    def GetMimeType(*args, **kwargs):
        """GetMimeType(self) -> String"""
        return _core_.ImageHandler_GetMimeType(*args, **kwargs)

    def CanRead(*args, **kwargs):
        """CanRead(self, String name) -> bool"""
        return _core_.ImageHandler_CanRead(*args, **kwargs)

    def SetName(*args, **kwargs):
        """SetName(self, String name)"""
        return _core_.ImageHandler_SetName(*args, **kwargs)

    def SetExtension(*args, **kwargs):
        """SetExtension(self, String extension)"""
        return _core_.ImageHandler_SetExtension(*args, **kwargs)

    def SetType(*args, **kwargs):
        """SetType(self, long type)"""
        return _core_.ImageHandler_SetType(*args, **kwargs)

    def SetMimeType(*args, **kwargs):
        """SetMimeType(self, String mimetype)"""
        return _core_.ImageHandler_SetMimeType(*args, **kwargs)


class ImageHandlerPtr(ImageHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ImageHandler
_core_.ImageHandler_swigregister(ImageHandlerPtr)

class ImageHistogram(object):
    """Proxy of C++ ImageHistogram class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxImageHistogram instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> ImageHistogram"""
        newobj = _core_.new_ImageHistogram(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def MakeKey(*args, **kwargs):
        """
        MakeKey(unsigned char r, unsigned char g, unsigned char b) -> unsigned long

        Get the key in the histogram for the given RGB values
        """
        return _core_.ImageHistogram_MakeKey(*args, **kwargs)

    MakeKey = staticmethod(MakeKey)
    def FindFirstUnusedColour(*args, **kwargs):
        """
        FindFirstUnusedColour(int startR=1, int startG=0, int startB=0) -> (success, r, g, b)

        Find first colour that is not used in the image and has higher RGB
        values than startR, startG, startB.  Returns a tuple consisting of a
        success flag and rgb values.
        """
        return _core_.ImageHistogram_FindFirstUnusedColour(*args, **kwargs)


class ImageHistogramPtr(ImageHistogram):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ImageHistogram
_core_.ImageHistogram_swigregister(ImageHistogramPtr)

def ImageHistogram_MakeKey(*args, **kwargs):
    """
    ImageHistogram_MakeKey(unsigned char r, unsigned char g, unsigned char b) -> unsigned long

    Get the key in the histogram for the given RGB values
    """
    return _core_.ImageHistogram_MakeKey(*args, **kwargs)

class Image(Object):
    """Proxy of C++ Image class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxImage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String name, long type=BITMAP_TYPE_ANY, int index=-1) -> Image"""
        newobj = _core_.new_Image(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_Image):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Create(*args, **kwargs):
        """Create(self, int width, int height)"""
        return _core_.Image_Create(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """
        Destroy(self)

        Deletes the C++ object this Python object is a proxy for.
        """
        return _core_.Image_Destroy(*args, **kwargs)

    def Scale(*args, **kwargs):
        """Scale(self, int width, int height) -> Image"""
        return _core_.Image_Scale(*args, **kwargs)

    def ShrinkBy(*args, **kwargs):
        """ShrinkBy(self, int xFactor, int yFactor) -> Image"""
        return _core_.Image_ShrinkBy(*args, **kwargs)

    def Rescale(*args, **kwargs):
        """Rescale(self, int width, int height) -> Image"""
        return _core_.Image_Rescale(*args, **kwargs)

    def SetRGB(*args, **kwargs):
        """SetRGB(self, int x, int y, unsigned char r, unsigned char g, unsigned char b)"""
        return _core_.Image_SetRGB(*args, **kwargs)

    def GetRed(*args, **kwargs):
        """GetRed(self, int x, int y) -> unsigned char"""
        return _core_.Image_GetRed(*args, **kwargs)

    def GetGreen(*args, **kwargs):
        """GetGreen(self, int x, int y) -> unsigned char"""
        return _core_.Image_GetGreen(*args, **kwargs)

    def GetBlue(*args, **kwargs):
        """GetBlue(self, int x, int y) -> unsigned char"""
        return _core_.Image_GetBlue(*args, **kwargs)

    def SetAlpha(*args, **kwargs):
        """SetAlpha(self, int x, int y, unsigned char alpha)"""
        return _core_.Image_SetAlpha(*args, **kwargs)

    def GetAlpha(*args, **kwargs):
        """GetAlpha(self, int x, int y) -> unsigned char"""
        return _core_.Image_GetAlpha(*args, **kwargs)

    def HasAlpha(*args, **kwargs):
        """HasAlpha(self) -> bool"""
        return _core_.Image_HasAlpha(*args, **kwargs)

    def FindFirstUnusedColour(*args, **kwargs):
        """
        FindFirstUnusedColour(int startR=1, int startG=0, int startB=0) -> (success, r, g, b)

        Find first colour that is not used in the image and has higher RGB
        values than startR, startG, startB.  Returns a tuple consisting of a
        success flag and rgb values.
        """
        return _core_.Image_FindFirstUnusedColour(*args, **kwargs)

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
        return _core_.Image_ConvertAlphaToMask(*args, **kwargs)

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
        return _core_.Image_ConvertColourToAlpha(*args, **kwargs)

    def SetMaskFromImage(*args, **kwargs):
        """SetMaskFromImage(self, Image mask, byte mr, byte mg, byte mb) -> bool"""
        return _core_.Image_SetMaskFromImage(*args, **kwargs)

    def CanRead(*args, **kwargs):
        """CanRead(String name) -> bool"""
        return _core_.Image_CanRead(*args, **kwargs)

    CanRead = staticmethod(CanRead)
    def GetImageCount(*args, **kwargs):
        """GetImageCount(String name, long type=BITMAP_TYPE_ANY) -> int"""
        return _core_.Image_GetImageCount(*args, **kwargs)

    GetImageCount = staticmethod(GetImageCount)
    def LoadFile(*args, **kwargs):
        """LoadFile(self, String name, long type=BITMAP_TYPE_ANY, int index=-1) -> bool"""
        return _core_.Image_LoadFile(*args, **kwargs)

    def LoadMimeFile(*args, **kwargs):
        """LoadMimeFile(self, String name, String mimetype, int index=-1) -> bool"""
        return _core_.Image_LoadMimeFile(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """SaveFile(self, String name, int type) -> bool"""
        return _core_.Image_SaveFile(*args, **kwargs)

    def SaveMimeFile(*args, **kwargs):
        """SaveMimeFile(self, String name, String mimetype) -> bool"""
        return _core_.Image_SaveMimeFile(*args, **kwargs)

    def CanReadStream(*args, **kwargs):
        """CanReadStream(InputStream stream) -> bool"""
        return _core_.Image_CanReadStream(*args, **kwargs)

    CanReadStream = staticmethod(CanReadStream)
    def LoadStream(*args, **kwargs):
        """LoadStream(self, InputStream stream, long type=BITMAP_TYPE_ANY, int index=-1) -> bool"""
        return _core_.Image_LoadStream(*args, **kwargs)

    def LoadMimeStream(*args, **kwargs):
        """LoadMimeStream(self, InputStream stream, String mimetype, int index=-1) -> bool"""
        return _core_.Image_LoadMimeStream(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _core_.Image_Ok(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _core_.Image_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return _core_.Image_GetHeight(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self) -> Size"""
        return _core_.Image_GetSize(*args, **kwargs)

    def GetSubImage(*args, **kwargs):
        """GetSubImage(self, Rect rect) -> Image"""
        return _core_.Image_GetSubImage(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self) -> Image"""
        return _core_.Image_Copy(*args, **kwargs)

    def Paste(*args, **kwargs):
        """Paste(self, Image image, int x, int y)"""
        return _core_.Image_Paste(*args, **kwargs)

    def GetData(*args, **kwargs):
        """GetData(self) -> PyObject"""
        return _core_.Image_GetData(*args, **kwargs)

    def SetData(*args, **kwargs):
        """SetData(self, PyObject data)"""
        return _core_.Image_SetData(*args, **kwargs)

    def GetDataBuffer(*args, **kwargs):
        """GetDataBuffer(self) -> PyObject"""
        return _core_.Image_GetDataBuffer(*args, **kwargs)

    def SetDataBuffer(*args, **kwargs):
        """SetDataBuffer(self, PyObject data)"""
        return _core_.Image_SetDataBuffer(*args, **kwargs)

    def GetAlphaData(*args, **kwargs):
        """GetAlphaData(self) -> PyObject"""
        return _core_.Image_GetAlphaData(*args, **kwargs)

    def SetAlphaData(*args, **kwargs):
        """SetAlphaData(self, PyObject data)"""
        return _core_.Image_SetAlphaData(*args, **kwargs)

    def GetAlphaBuffer(*args, **kwargs):
        """GetAlphaBuffer(self) -> PyObject"""
        return _core_.Image_GetAlphaBuffer(*args, **kwargs)

    def SetAlphaBuffer(*args, **kwargs):
        """SetAlphaBuffer(self, PyObject data)"""
        return _core_.Image_SetAlphaBuffer(*args, **kwargs)

    def SetMaskColour(*args, **kwargs):
        """SetMaskColour(self, unsigned char r, unsigned char g, unsigned char b)"""
        return _core_.Image_SetMaskColour(*args, **kwargs)

    def GetMaskRed(*args, **kwargs):
        """GetMaskRed(self) -> unsigned char"""
        return _core_.Image_GetMaskRed(*args, **kwargs)

    def GetMaskGreen(*args, **kwargs):
        """GetMaskGreen(self) -> unsigned char"""
        return _core_.Image_GetMaskGreen(*args, **kwargs)

    def GetMaskBlue(*args, **kwargs):
        """GetMaskBlue(self) -> unsigned char"""
        return _core_.Image_GetMaskBlue(*args, **kwargs)

    def SetMask(*args, **kwargs):
        """SetMask(self, bool mask=True)"""
        return _core_.Image_SetMask(*args, **kwargs)

    def HasMask(*args, **kwargs):
        """HasMask(self) -> bool"""
        return _core_.Image_HasMask(*args, **kwargs)

    def Rotate(*args, **kwargs):
        """
        Rotate(self, double angle, Point centre_of_rotation, bool interpolating=True, 
            Point offset_after_rotation=None) -> Image
        """
        return _core_.Image_Rotate(*args, **kwargs)

    def Rotate90(*args, **kwargs):
        """Rotate90(self, bool clockwise=True) -> Image"""
        return _core_.Image_Rotate90(*args, **kwargs)

    def Mirror(*args, **kwargs):
        """Mirror(self, bool horizontally=True) -> Image"""
        return _core_.Image_Mirror(*args, **kwargs)

    def Replace(*args, **kwargs):
        """
        Replace(self, unsigned char r1, unsigned char g1, unsigned char b1, 
            unsigned char r2, unsigned char g2, unsigned char b2)
        """
        return _core_.Image_Replace(*args, **kwargs)

    def ConvertToMono(*args, **kwargs):
        """ConvertToMono(self, unsigned char r, unsigned char g, unsigned char b) -> Image"""
        return _core_.Image_ConvertToMono(*args, **kwargs)

    def SetOption(*args, **kwargs):
        """SetOption(self, String name, String value)"""
        return _core_.Image_SetOption(*args, **kwargs)

    def SetOptionInt(*args, **kwargs):
        """SetOptionInt(self, String name, int value)"""
        return _core_.Image_SetOptionInt(*args, **kwargs)

    def GetOption(*args, **kwargs):
        """GetOption(self, String name) -> String"""
        return _core_.Image_GetOption(*args, **kwargs)

    def GetOptionInt(*args, **kwargs):
        """GetOptionInt(self, String name) -> int"""
        return _core_.Image_GetOptionInt(*args, **kwargs)

    def HasOption(*args, **kwargs):
        """HasOption(self, String name) -> bool"""
        return _core_.Image_HasOption(*args, **kwargs)

    def CountColours(*args, **kwargs):
        """CountColours(self, unsigned long stopafter=(unsigned long) -1) -> unsigned long"""
        return _core_.Image_CountColours(*args, **kwargs)

    def ComputeHistogram(*args, **kwargs):
        """ComputeHistogram(self, ImageHistogram h) -> unsigned long"""
        return _core_.Image_ComputeHistogram(*args, **kwargs)

    def AddHandler(*args, **kwargs):
        """AddHandler(ImageHandler handler)"""
        return _core_.Image_AddHandler(*args, **kwargs)

    AddHandler = staticmethod(AddHandler)
    def InsertHandler(*args, **kwargs):
        """InsertHandler(ImageHandler handler)"""
        return _core_.Image_InsertHandler(*args, **kwargs)

    InsertHandler = staticmethod(InsertHandler)
    def RemoveHandler(*args, **kwargs):
        """RemoveHandler(String name) -> bool"""
        return _core_.Image_RemoveHandler(*args, **kwargs)

    RemoveHandler = staticmethod(RemoveHandler)
    def GetImageExtWildcard(*args, **kwargs):
        """GetImageExtWildcard() -> String"""
        return _core_.Image_GetImageExtWildcard(*args, **kwargs)

    GetImageExtWildcard = staticmethod(GetImageExtWildcard)
    def ConvertToBitmap(*args, **kwargs):
        """ConvertToBitmap(self, int depth=-1) -> Bitmap"""
        return _core_.Image_ConvertToBitmap(*args, **kwargs)

    def ConvertToMonoBitmap(*args, **kwargs):
        """ConvertToMonoBitmap(self, unsigned char red, unsigned char green, unsigned char blue) -> Bitmap"""
        return _core_.Image_ConvertToMonoBitmap(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class ImagePtr(Image):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Image
_core_.Image_swigregister(ImagePtr)

def ImageFromMime(*args, **kwargs):
    """ImageFromMime(String name, String mimetype, int index=-1) -> Image"""
    val = _core_.new_ImageFromMime(*args, **kwargs)
    val.thisown = 1
    return val

def ImageFromStream(*args, **kwargs):
    """ImageFromStream(InputStream stream, long type=BITMAP_TYPE_ANY, int index=-1) -> Image"""
    val = _core_.new_ImageFromStream(*args, **kwargs)
    val.thisown = 1
    return val

def ImageFromStreamMime(*args, **kwargs):
    """ImageFromStreamMime(InputStream stream, String mimetype, int index=-1) -> Image"""
    val = _core_.new_ImageFromStreamMime(*args, **kwargs)
    val.thisown = 1
    return val

def EmptyImage(*args, **kwargs):
    """EmptyImage(int width=0, int height=0, bool clear=True) -> Image"""
    val = _core_.new_EmptyImage(*args, **kwargs)
    val.thisown = 1
    return val

def ImageFromBitmap(*args, **kwargs):
    """ImageFromBitmap(Bitmap bitmap) -> Image"""
    val = _core_.new_ImageFromBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def ImageFromData(*args, **kwargs):
    """ImageFromData(int width, int height, unsigned char data) -> Image"""
    val = _core_.new_ImageFromData(*args, **kwargs)
    val.thisown = 1
    return val

def ImageFromDataWithAlpha(*args, **kwargs):
    """ImageFromDataWithAlpha(int width, int height, unsigned char data, unsigned char alpha) -> Image"""
    val = _core_.new_ImageFromDataWithAlpha(*args, **kwargs)
    val.thisown = 1
    return val

def Image_CanRead(*args, **kwargs):
    """Image_CanRead(String name) -> bool"""
    return _core_.Image_CanRead(*args, **kwargs)

def Image_GetImageCount(*args, **kwargs):
    """Image_GetImageCount(String name, long type=BITMAP_TYPE_ANY) -> int"""
    return _core_.Image_GetImageCount(*args, **kwargs)

def Image_CanReadStream(*args, **kwargs):
    """Image_CanReadStream(InputStream stream) -> bool"""
    return _core_.Image_CanReadStream(*args, **kwargs)

def Image_AddHandler(*args, **kwargs):
    """Image_AddHandler(ImageHandler handler)"""
    return _core_.Image_AddHandler(*args, **kwargs)

def Image_InsertHandler(*args, **kwargs):
    """Image_InsertHandler(ImageHandler handler)"""
    return _core_.Image_InsertHandler(*args, **kwargs)

def Image_RemoveHandler(*args, **kwargs):
    """Image_RemoveHandler(String name) -> bool"""
    return _core_.Image_RemoveHandler(*args, **kwargs)

def Image_GetImageExtWildcard(*args, **kwargs):
    """Image_GetImageExtWildcard() -> String"""
    return _core_.Image_GetImageExtWildcard(*args, **kwargs)

def InitAllImageHandlers():
    """
    The former functionality of InitAllImageHanders is now done internal to
    the _core_ extension module and so this function has become a simple NOP.
    """
    pass

IMAGE_RESOLUTION_INCHES = _core_.IMAGE_RESOLUTION_INCHES
IMAGE_RESOLUTION_CM = _core_.IMAGE_RESOLUTION_CM
BMP_24BPP = _core_.BMP_24BPP
BMP_8BPP = _core_.BMP_8BPP
BMP_8BPP_GREY = _core_.BMP_8BPP_GREY
BMP_8BPP_GRAY = _core_.BMP_8BPP_GRAY
BMP_8BPP_RED = _core_.BMP_8BPP_RED
BMP_8BPP_PALETTE = _core_.BMP_8BPP_PALETTE
BMP_4BPP = _core_.BMP_4BPP
BMP_1BPP = _core_.BMP_1BPP
BMP_1BPP_BW = _core_.BMP_1BPP_BW
class BMPHandler(ImageHandler):
    """Proxy of C++ BMPHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBMPHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> BMPHandler"""
        newobj = _core_.new_BMPHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class BMPHandlerPtr(BMPHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BMPHandler
_core_.BMPHandler_swigregister(BMPHandlerPtr)
NullImage = cvar.NullImage
IMAGE_OPTION_BMP_FORMAT = cvar.IMAGE_OPTION_BMP_FORMAT
IMAGE_OPTION_CUR_HOTSPOT_X = cvar.IMAGE_OPTION_CUR_HOTSPOT_X
IMAGE_OPTION_CUR_HOTSPOT_Y = cvar.IMAGE_OPTION_CUR_HOTSPOT_Y
IMAGE_OPTION_RESOLUTION = cvar.IMAGE_OPTION_RESOLUTION
IMAGE_OPTION_RESOLUTIONUNIT = cvar.IMAGE_OPTION_RESOLUTIONUNIT

class ICOHandler(BMPHandler):
    """Proxy of C++ ICOHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxICOHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> ICOHandler"""
        newobj = _core_.new_ICOHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class ICOHandlerPtr(ICOHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ICOHandler
_core_.ICOHandler_swigregister(ICOHandlerPtr)

class CURHandler(ICOHandler):
    """Proxy of C++ CURHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCURHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> CURHandler"""
        newobj = _core_.new_CURHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class CURHandlerPtr(CURHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CURHandler
_core_.CURHandler_swigregister(CURHandlerPtr)

class ANIHandler(CURHandler):
    """Proxy of C++ ANIHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxANIHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> ANIHandler"""
        newobj = _core_.new_ANIHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class ANIHandlerPtr(ANIHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ANIHandler
_core_.ANIHandler_swigregister(ANIHandlerPtr)

class PNGHandler(ImageHandler):
    """Proxy of C++ PNGHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPNGHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> PNGHandler"""
        newobj = _core_.new_PNGHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class PNGHandlerPtr(PNGHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PNGHandler
_core_.PNGHandler_swigregister(PNGHandlerPtr)

class GIFHandler(ImageHandler):
    """Proxy of C++ GIFHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGIFHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GIFHandler"""
        newobj = _core_.new_GIFHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class GIFHandlerPtr(GIFHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GIFHandler
_core_.GIFHandler_swigregister(GIFHandlerPtr)

class PCXHandler(ImageHandler):
    """Proxy of C++ PCXHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPCXHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> PCXHandler"""
        newobj = _core_.new_PCXHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class PCXHandlerPtr(PCXHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PCXHandler
_core_.PCXHandler_swigregister(PCXHandlerPtr)

class JPEGHandler(ImageHandler):
    """Proxy of C++ JPEGHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxJPEGHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> JPEGHandler"""
        newobj = _core_.new_JPEGHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class JPEGHandlerPtr(JPEGHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = JPEGHandler
_core_.JPEGHandler_swigregister(JPEGHandlerPtr)

class PNMHandler(ImageHandler):
    """Proxy of C++ PNMHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPNMHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> PNMHandler"""
        newobj = _core_.new_PNMHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class PNMHandlerPtr(PNMHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PNMHandler
_core_.PNMHandler_swigregister(PNMHandlerPtr)

class XPMHandler(ImageHandler):
    """Proxy of C++ XPMHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxXPMHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> XPMHandler"""
        newobj = _core_.new_XPMHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class XPMHandlerPtr(XPMHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XPMHandler
_core_.XPMHandler_swigregister(XPMHandlerPtr)

class TIFFHandler(ImageHandler):
    """Proxy of C++ TIFFHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTIFFHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> TIFFHandler"""
        newobj = _core_.new_TIFFHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class TIFFHandlerPtr(TIFFHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TIFFHandler
_core_.TIFFHandler_swigregister(TIFFHandlerPtr)

QUANTIZE_INCLUDE_WINDOWS_COLOURS = _core_.QUANTIZE_INCLUDE_WINDOWS_COLOURS
QUANTIZE_FILL_DESTINATION_IMAGE = _core_.QUANTIZE_FILL_DESTINATION_IMAGE
class Quantize(object):
    """Performs quantization, or colour reduction, on a wxImage."""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxQuantize instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def Quantize(*args, **kwargs):
        """
        Quantize(Image src, Image dest, int desiredNoColours=236, int flags=wxQUANTIZE_INCLUDE_WINDOWS_COLOURS|wxQUANTIZE_FILL_DESTINATION_IMAGE) -> bool

        Reduce the colours in the source image and put the result into the
        destination image, setting the palette in the destination if
        needed. Both images may be the same, to overwrite the source image.
        """
        return _core_.Quantize_Quantize(*args, **kwargs)

    Quantize = staticmethod(Quantize)

class QuantizePtr(Quantize):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Quantize
_core_.Quantize_swigregister(QuantizePtr)

def Quantize_Quantize(*args, **kwargs):
    """
    Quantize_Quantize(Image src, Image dest, int desiredNoColours=236, int flags=wxQUANTIZE_INCLUDE_WINDOWS_COLOURS|wxQUANTIZE_FILL_DESTINATION_IMAGE) -> bool

    Reduce the colours in the source image and put the result into the
    destination image, setting the palette in the destination if
    needed. Both images may be the same, to overwrite the source image.
    """
    return _core_.Quantize_Quantize(*args, **kwargs)

#---------------------------------------------------------------------------

class EvtHandler(Object):
    """Proxy of C++ EvtHandler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEvtHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> EvtHandler"""
        newobj = _core_.new_EvtHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetNextHandler(*args, **kwargs):
        """GetNextHandler(self) -> EvtHandler"""
        return _core_.EvtHandler_GetNextHandler(*args, **kwargs)

    def GetPreviousHandler(*args, **kwargs):
        """GetPreviousHandler(self) -> EvtHandler"""
        return _core_.EvtHandler_GetPreviousHandler(*args, **kwargs)

    def SetNextHandler(*args, **kwargs):
        """SetNextHandler(self, EvtHandler handler)"""
        return _core_.EvtHandler_SetNextHandler(*args, **kwargs)

    def SetPreviousHandler(*args, **kwargs):
        """SetPreviousHandler(self, EvtHandler handler)"""
        return _core_.EvtHandler_SetPreviousHandler(*args, **kwargs)

    def GetEvtHandlerEnabled(*args, **kwargs):
        """GetEvtHandlerEnabled(self) -> bool"""
        return _core_.EvtHandler_GetEvtHandlerEnabled(*args, **kwargs)

    def SetEvtHandlerEnabled(*args, **kwargs):
        """SetEvtHandlerEnabled(self, bool enabled)"""
        return _core_.EvtHandler_SetEvtHandlerEnabled(*args, **kwargs)

    def ProcessEvent(*args, **kwargs):
        """ProcessEvent(self, Event event) -> bool"""
        return _core_.EvtHandler_ProcessEvent(*args, **kwargs)

    def AddPendingEvent(*args, **kwargs):
        """AddPendingEvent(self, Event event)"""
        return _core_.EvtHandler_AddPendingEvent(*args, **kwargs)

    def ProcessPendingEvents(*args, **kwargs):
        """ProcessPendingEvents(self)"""
        return _core_.EvtHandler_ProcessPendingEvents(*args, **kwargs)

    def Connect(*args, **kwargs):
        """Connect(self, int id, int lastId, int eventType, PyObject func)"""
        return _core_.EvtHandler_Connect(*args, **kwargs)

    def Disconnect(*args, **kwargs):
        """Disconnect(self, int id, int lastId=-1, wxEventType eventType=wxEVT_NULL) -> bool"""
        return _core_.EvtHandler_Disconnect(*args, **kwargs)

    def _setOORInfo(*args, **kwargs):
        """_setOORInfo(self, PyObject _self, bool incref=True)"""
        return _core_.EvtHandler__setOORInfo(*args, **kwargs)

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


class EvtHandlerPtr(EvtHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EvtHandler
_core_.EvtHandler_swigregister(EvtHandlerPtr)

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

EVENT_PROPAGATE_NONE = _core_.EVENT_PROPAGATE_NONE
EVENT_PROPAGATE_MAX = _core_.EVENT_PROPAGATE_MAX

def NewEventType(*args, **kwargs):
    """NewEventType() -> wxEventType"""
    return _core_.NewEventType(*args, **kwargs)
wxEVT_NULL = _core_.wxEVT_NULL
wxEVT_FIRST = _core_.wxEVT_FIRST
wxEVT_USER_FIRST = _core_.wxEVT_USER_FIRST
wxEVT_COMMAND_BUTTON_CLICKED = _core_.wxEVT_COMMAND_BUTTON_CLICKED
wxEVT_COMMAND_CHECKBOX_CLICKED = _core_.wxEVT_COMMAND_CHECKBOX_CLICKED
wxEVT_COMMAND_CHOICE_SELECTED = _core_.wxEVT_COMMAND_CHOICE_SELECTED
wxEVT_COMMAND_LISTBOX_SELECTED = _core_.wxEVT_COMMAND_LISTBOX_SELECTED
wxEVT_COMMAND_LISTBOX_DOUBLECLICKED = _core_.wxEVT_COMMAND_LISTBOX_DOUBLECLICKED
wxEVT_COMMAND_CHECKLISTBOX_TOGGLED = _core_.wxEVT_COMMAND_CHECKLISTBOX_TOGGLED
wxEVT_COMMAND_MENU_SELECTED = _core_.wxEVT_COMMAND_MENU_SELECTED
wxEVT_COMMAND_TOOL_CLICKED = _core_.wxEVT_COMMAND_TOOL_CLICKED
wxEVT_COMMAND_SLIDER_UPDATED = _core_.wxEVT_COMMAND_SLIDER_UPDATED
wxEVT_COMMAND_RADIOBOX_SELECTED = _core_.wxEVT_COMMAND_RADIOBOX_SELECTED
wxEVT_COMMAND_RADIOBUTTON_SELECTED = _core_.wxEVT_COMMAND_RADIOBUTTON_SELECTED
wxEVT_COMMAND_SCROLLBAR_UPDATED = _core_.wxEVT_COMMAND_SCROLLBAR_UPDATED
wxEVT_COMMAND_VLBOX_SELECTED = _core_.wxEVT_COMMAND_VLBOX_SELECTED
wxEVT_COMMAND_COMBOBOX_SELECTED = _core_.wxEVT_COMMAND_COMBOBOX_SELECTED
wxEVT_COMMAND_TOOL_RCLICKED = _core_.wxEVT_COMMAND_TOOL_RCLICKED
wxEVT_COMMAND_TOOL_ENTER = _core_.wxEVT_COMMAND_TOOL_ENTER
wxEVT_LEFT_DOWN = _core_.wxEVT_LEFT_DOWN
wxEVT_LEFT_UP = _core_.wxEVT_LEFT_UP
wxEVT_MIDDLE_DOWN = _core_.wxEVT_MIDDLE_DOWN
wxEVT_MIDDLE_UP = _core_.wxEVT_MIDDLE_UP
wxEVT_RIGHT_DOWN = _core_.wxEVT_RIGHT_DOWN
wxEVT_RIGHT_UP = _core_.wxEVT_RIGHT_UP
wxEVT_MOTION = _core_.wxEVT_MOTION
wxEVT_ENTER_WINDOW = _core_.wxEVT_ENTER_WINDOW
wxEVT_LEAVE_WINDOW = _core_.wxEVT_LEAVE_WINDOW
wxEVT_LEFT_DCLICK = _core_.wxEVT_LEFT_DCLICK
wxEVT_MIDDLE_DCLICK = _core_.wxEVT_MIDDLE_DCLICK
wxEVT_RIGHT_DCLICK = _core_.wxEVT_RIGHT_DCLICK
wxEVT_SET_FOCUS = _core_.wxEVT_SET_FOCUS
wxEVT_KILL_FOCUS = _core_.wxEVT_KILL_FOCUS
wxEVT_CHILD_FOCUS = _core_.wxEVT_CHILD_FOCUS
wxEVT_MOUSEWHEEL = _core_.wxEVT_MOUSEWHEEL
wxEVT_NC_LEFT_DOWN = _core_.wxEVT_NC_LEFT_DOWN
wxEVT_NC_LEFT_UP = _core_.wxEVT_NC_LEFT_UP
wxEVT_NC_MIDDLE_DOWN = _core_.wxEVT_NC_MIDDLE_DOWN
wxEVT_NC_MIDDLE_UP = _core_.wxEVT_NC_MIDDLE_UP
wxEVT_NC_RIGHT_DOWN = _core_.wxEVT_NC_RIGHT_DOWN
wxEVT_NC_RIGHT_UP = _core_.wxEVT_NC_RIGHT_UP
wxEVT_NC_MOTION = _core_.wxEVT_NC_MOTION
wxEVT_NC_ENTER_WINDOW = _core_.wxEVT_NC_ENTER_WINDOW
wxEVT_NC_LEAVE_WINDOW = _core_.wxEVT_NC_LEAVE_WINDOW
wxEVT_NC_LEFT_DCLICK = _core_.wxEVT_NC_LEFT_DCLICK
wxEVT_NC_MIDDLE_DCLICK = _core_.wxEVT_NC_MIDDLE_DCLICK
wxEVT_NC_RIGHT_DCLICK = _core_.wxEVT_NC_RIGHT_DCLICK
wxEVT_CHAR = _core_.wxEVT_CHAR
wxEVT_CHAR_HOOK = _core_.wxEVT_CHAR_HOOK
wxEVT_NAVIGATION_KEY = _core_.wxEVT_NAVIGATION_KEY
wxEVT_KEY_DOWN = _core_.wxEVT_KEY_DOWN
wxEVT_KEY_UP = _core_.wxEVT_KEY_UP
wxEVT_HOTKEY = _core_.wxEVT_HOTKEY
wxEVT_SET_CURSOR = _core_.wxEVT_SET_CURSOR
wxEVT_SCROLL_TOP = _core_.wxEVT_SCROLL_TOP
wxEVT_SCROLL_BOTTOM = _core_.wxEVT_SCROLL_BOTTOM
wxEVT_SCROLL_LINEUP = _core_.wxEVT_SCROLL_LINEUP
wxEVT_SCROLL_LINEDOWN = _core_.wxEVT_SCROLL_LINEDOWN
wxEVT_SCROLL_PAGEUP = _core_.wxEVT_SCROLL_PAGEUP
wxEVT_SCROLL_PAGEDOWN = _core_.wxEVT_SCROLL_PAGEDOWN
wxEVT_SCROLL_THUMBTRACK = _core_.wxEVT_SCROLL_THUMBTRACK
wxEVT_SCROLL_THUMBRELEASE = _core_.wxEVT_SCROLL_THUMBRELEASE
wxEVT_SCROLL_ENDSCROLL = _core_.wxEVT_SCROLL_ENDSCROLL
wxEVT_SCROLLWIN_TOP = _core_.wxEVT_SCROLLWIN_TOP
wxEVT_SCROLLWIN_BOTTOM = _core_.wxEVT_SCROLLWIN_BOTTOM
wxEVT_SCROLLWIN_LINEUP = _core_.wxEVT_SCROLLWIN_LINEUP
wxEVT_SCROLLWIN_LINEDOWN = _core_.wxEVT_SCROLLWIN_LINEDOWN
wxEVT_SCROLLWIN_PAGEUP = _core_.wxEVT_SCROLLWIN_PAGEUP
wxEVT_SCROLLWIN_PAGEDOWN = _core_.wxEVT_SCROLLWIN_PAGEDOWN
wxEVT_SCROLLWIN_THUMBTRACK = _core_.wxEVT_SCROLLWIN_THUMBTRACK
wxEVT_SCROLLWIN_THUMBRELEASE = _core_.wxEVT_SCROLLWIN_THUMBRELEASE
wxEVT_SIZE = _core_.wxEVT_SIZE
wxEVT_MOVE = _core_.wxEVT_MOVE
wxEVT_CLOSE_WINDOW = _core_.wxEVT_CLOSE_WINDOW
wxEVT_END_SESSION = _core_.wxEVT_END_SESSION
wxEVT_QUERY_END_SESSION = _core_.wxEVT_QUERY_END_SESSION
wxEVT_ACTIVATE_APP = _core_.wxEVT_ACTIVATE_APP
wxEVT_POWER = _core_.wxEVT_POWER
wxEVT_ACTIVATE = _core_.wxEVT_ACTIVATE
wxEVT_CREATE = _core_.wxEVT_CREATE
wxEVT_DESTROY = _core_.wxEVT_DESTROY
wxEVT_SHOW = _core_.wxEVT_SHOW
wxEVT_ICONIZE = _core_.wxEVT_ICONIZE
wxEVT_MAXIMIZE = _core_.wxEVT_MAXIMIZE
wxEVT_MOUSE_CAPTURE_CHANGED = _core_.wxEVT_MOUSE_CAPTURE_CHANGED
wxEVT_PAINT = _core_.wxEVT_PAINT
wxEVT_ERASE_BACKGROUND = _core_.wxEVT_ERASE_BACKGROUND
wxEVT_NC_PAINT = _core_.wxEVT_NC_PAINT
wxEVT_PAINT_ICON = _core_.wxEVT_PAINT_ICON
wxEVT_MENU_OPEN = _core_.wxEVT_MENU_OPEN
wxEVT_MENU_CLOSE = _core_.wxEVT_MENU_CLOSE
wxEVT_MENU_HIGHLIGHT = _core_.wxEVT_MENU_HIGHLIGHT
wxEVT_CONTEXT_MENU = _core_.wxEVT_CONTEXT_MENU
wxEVT_SYS_COLOUR_CHANGED = _core_.wxEVT_SYS_COLOUR_CHANGED
wxEVT_DISPLAY_CHANGED = _core_.wxEVT_DISPLAY_CHANGED
wxEVT_SETTING_CHANGED = _core_.wxEVT_SETTING_CHANGED
wxEVT_QUERY_NEW_PALETTE = _core_.wxEVT_QUERY_NEW_PALETTE
wxEVT_PALETTE_CHANGED = _core_.wxEVT_PALETTE_CHANGED
wxEVT_DROP_FILES = _core_.wxEVT_DROP_FILES
wxEVT_DRAW_ITEM = _core_.wxEVT_DRAW_ITEM
wxEVT_MEASURE_ITEM = _core_.wxEVT_MEASURE_ITEM
wxEVT_COMPARE_ITEM = _core_.wxEVT_COMPARE_ITEM
wxEVT_INIT_DIALOG = _core_.wxEVT_INIT_DIALOG
wxEVT_IDLE = _core_.wxEVT_IDLE
wxEVT_UPDATE_UI = _core_.wxEVT_UPDATE_UI
wxEVT_SIZING = _core_.wxEVT_SIZING
wxEVT_MOVING = _core_.wxEVT_MOVING
wxEVT_COMMAND_LEFT_CLICK = _core_.wxEVT_COMMAND_LEFT_CLICK
wxEVT_COMMAND_LEFT_DCLICK = _core_.wxEVT_COMMAND_LEFT_DCLICK
wxEVT_COMMAND_RIGHT_CLICK = _core_.wxEVT_COMMAND_RIGHT_CLICK
wxEVT_COMMAND_RIGHT_DCLICK = _core_.wxEVT_COMMAND_RIGHT_DCLICK
wxEVT_COMMAND_SET_FOCUS = _core_.wxEVT_COMMAND_SET_FOCUS
wxEVT_COMMAND_KILL_FOCUS = _core_.wxEVT_COMMAND_KILL_FOCUS
wxEVT_COMMAND_ENTER = _core_.wxEVT_COMMAND_ENTER
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

class Event(Object):
    """Proxy of C++ Event class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_core_.delete_Event):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetEventType(*args, **kwargs):
        """SetEventType(self, wxEventType typ)"""
        return _core_.Event_SetEventType(*args, **kwargs)

    def GetEventType(*args, **kwargs):
        """GetEventType(self) -> wxEventType"""
        return _core_.Event_GetEventType(*args, **kwargs)

    def GetEventObject(*args, **kwargs):
        """GetEventObject(self) -> Object"""
        return _core_.Event_GetEventObject(*args, **kwargs)

    def SetEventObject(*args, **kwargs):
        """SetEventObject(self, Object obj)"""
        return _core_.Event_SetEventObject(*args, **kwargs)

    def GetTimestamp(*args, **kwargs):
        """GetTimestamp(self) -> long"""
        return _core_.Event_GetTimestamp(*args, **kwargs)

    def SetTimestamp(*args, **kwargs):
        """SetTimestamp(self, long ts=0)"""
        return _core_.Event_SetTimestamp(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> int"""
        return _core_.Event_GetId(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(self, int Id)"""
        return _core_.Event_SetId(*args, **kwargs)

    def IsCommandEvent(*args, **kwargs):
        """IsCommandEvent(self) -> bool"""
        return _core_.Event_IsCommandEvent(*args, **kwargs)

    def Skip(*args, **kwargs):
        """Skip(self, bool skip=True)"""
        return _core_.Event_Skip(*args, **kwargs)

    def GetSkipped(*args, **kwargs):
        """GetSkipped(self) -> bool"""
        return _core_.Event_GetSkipped(*args, **kwargs)

    def ShouldPropagate(*args, **kwargs):
        """ShouldPropagate(self) -> bool"""
        return _core_.Event_ShouldPropagate(*args, **kwargs)

    def StopPropagation(*args, **kwargs):
        """StopPropagation(self) -> int"""
        return _core_.Event_StopPropagation(*args, **kwargs)

    def ResumePropagation(*args, **kwargs):
        """ResumePropagation(self, int propagationLevel)"""
        return _core_.Event_ResumePropagation(*args, **kwargs)

    def Clone(*args, **kwargs):
        """Clone(self) -> Event"""
        return _core_.Event_Clone(*args, **kwargs)


class EventPtr(Event):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Event
_core_.Event_swigregister(EventPtr)

#---------------------------------------------------------------------------

class PropagationDisabler(object):
    """Proxy of C++ PropagationDisabler class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropagationDisabler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Event event) -> PropagationDisabler"""
        newobj = _core_.new_PropagationDisabler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_PropagationDisabler):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class PropagationDisablerPtr(PropagationDisabler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PropagationDisabler
_core_.PropagationDisabler_swigregister(PropagationDisablerPtr)

class PropagateOnce(object):
    """Proxy of C++ PropagateOnce class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropagateOnce instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Event event) -> PropagateOnce"""
        newobj = _core_.new_PropagateOnce(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_PropagateOnce):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class PropagateOncePtr(PropagateOnce):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PropagateOnce
_core_.PropagateOnce_swigregister(PropagateOncePtr)

#---------------------------------------------------------------------------

class CommandEvent(Event):
    """Proxy of C++ CommandEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCommandEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int winid=0) -> CommandEvent"""
        newobj = _core_.new_CommandEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> int"""
        return _core_.CommandEvent_GetSelection(*args, **kwargs)

    def SetString(*args, **kwargs):
        """SetString(self, String s)"""
        return _core_.CommandEvent_SetString(*args, **kwargs)

    def GetString(*args, **kwargs):
        """GetString(self) -> String"""
        return _core_.CommandEvent_GetString(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked(self) -> bool"""
        return _core_.CommandEvent_IsChecked(*args, **kwargs)

    Checked = IsChecked 
    def IsSelection(*args, **kwargs):
        """IsSelection(self) -> bool"""
        return _core_.CommandEvent_IsSelection(*args, **kwargs)

    def SetExtraLong(*args, **kwargs):
        """SetExtraLong(self, long extraLong)"""
        return _core_.CommandEvent_SetExtraLong(*args, **kwargs)

    def GetExtraLong(*args, **kwargs):
        """GetExtraLong(self) -> long"""
        return _core_.CommandEvent_GetExtraLong(*args, **kwargs)

    def SetInt(*args, **kwargs):
        """SetInt(self, int i)"""
        return _core_.CommandEvent_SetInt(*args, **kwargs)

    def GetInt(*args, **kwargs):
        """GetInt(self) -> long"""
        return _core_.CommandEvent_GetInt(*args, **kwargs)

    def Clone(*args, **kwargs):
        """Clone(self) -> Event"""
        return _core_.CommandEvent_Clone(*args, **kwargs)


class CommandEventPtr(CommandEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CommandEvent
_core_.CommandEvent_swigregister(CommandEventPtr)

#---------------------------------------------------------------------------

class NotifyEvent(CommandEvent):
    """Proxy of C++ NotifyEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotifyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int winid=0) -> NotifyEvent"""
        newobj = _core_.new_NotifyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Veto(*args, **kwargs):
        """Veto(self)"""
        return _core_.NotifyEvent_Veto(*args, **kwargs)

    def Allow(*args, **kwargs):
        """Allow(self)"""
        return _core_.NotifyEvent_Allow(*args, **kwargs)

    def IsAllowed(*args, **kwargs):
        """IsAllowed(self) -> bool"""
        return _core_.NotifyEvent_IsAllowed(*args, **kwargs)


class NotifyEventPtr(NotifyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NotifyEvent
_core_.NotifyEvent_swigregister(NotifyEventPtr)

#---------------------------------------------------------------------------

class ScrollEvent(CommandEvent):
    """Proxy of C++ ScrollEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScrollEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxEventType commandType=wxEVT_NULL, int winid=0, int pos=0, 
            int orient=0) -> ScrollEvent
        """
        newobj = _core_.new_ScrollEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetOrientation(*args, **kwargs):
        """GetOrientation(self) -> int"""
        return _core_.ScrollEvent_GetOrientation(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> int"""
        return _core_.ScrollEvent_GetPosition(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(self, int orient)"""
        return _core_.ScrollEvent_SetOrientation(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, int pos)"""
        return _core_.ScrollEvent_SetPosition(*args, **kwargs)


class ScrollEventPtr(ScrollEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ScrollEvent
_core_.ScrollEvent_swigregister(ScrollEventPtr)

#---------------------------------------------------------------------------

class ScrollWinEvent(Event):
    """Proxy of C++ ScrollWinEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScrollWinEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int pos=0, int orient=0) -> ScrollWinEvent"""
        newobj = _core_.new_ScrollWinEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetOrientation(*args, **kwargs):
        """GetOrientation(self) -> int"""
        return _core_.ScrollWinEvent_GetOrientation(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> int"""
        return _core_.ScrollWinEvent_GetPosition(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(self, int orient)"""
        return _core_.ScrollWinEvent_SetOrientation(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, int pos)"""
        return _core_.ScrollWinEvent_SetPosition(*args, **kwargs)


class ScrollWinEventPtr(ScrollWinEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ScrollWinEvent
_core_.ScrollWinEvent_swigregister(ScrollWinEventPtr)

#---------------------------------------------------------------------------

MOUSE_BTN_ANY = _core_.MOUSE_BTN_ANY
MOUSE_BTN_NONE = _core_.MOUSE_BTN_NONE
MOUSE_BTN_LEFT = _core_.MOUSE_BTN_LEFT
MOUSE_BTN_MIDDLE = _core_.MOUSE_BTN_MIDDLE
MOUSE_BTN_RIGHT = _core_.MOUSE_BTN_RIGHT
class MouseEvent(Event):
    """Proxy of C++ MouseEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMouseEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType mouseType=wxEVT_NULL) -> MouseEvent"""
        newobj = _core_.new_MouseEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def IsButton(*args, **kwargs):
        """IsButton(self) -> bool"""
        return _core_.MouseEvent_IsButton(*args, **kwargs)

    def ButtonDown(*args, **kwargs):
        """ButtonDown(self, int but=MOUSE_BTN_ANY) -> bool"""
        return _core_.MouseEvent_ButtonDown(*args, **kwargs)

    def ButtonDClick(*args, **kwargs):
        """ButtonDClick(self, int but=MOUSE_BTN_ANY) -> bool"""
        return _core_.MouseEvent_ButtonDClick(*args, **kwargs)

    def ButtonUp(*args, **kwargs):
        """ButtonUp(self, int but=MOUSE_BTN_ANY) -> bool"""
        return _core_.MouseEvent_ButtonUp(*args, **kwargs)

    def Button(*args, **kwargs):
        """Button(self, int but) -> bool"""
        return _core_.MouseEvent_Button(*args, **kwargs)

    def ButtonIsDown(*args, **kwargs):
        """ButtonIsDown(self, int but) -> bool"""
        return _core_.MouseEvent_ButtonIsDown(*args, **kwargs)

    def GetButton(*args, **kwargs):
        """GetButton(self) -> int"""
        return _core_.MouseEvent_GetButton(*args, **kwargs)

    def ControlDown(*args, **kwargs):
        """ControlDown(self) -> bool"""
        return _core_.MouseEvent_ControlDown(*args, **kwargs)

    def MetaDown(*args, **kwargs):
        """MetaDown(self) -> bool"""
        return _core_.MouseEvent_MetaDown(*args, **kwargs)

    def AltDown(*args, **kwargs):
        """AltDown(self) -> bool"""
        return _core_.MouseEvent_AltDown(*args, **kwargs)

    def ShiftDown(*args, **kwargs):
        """ShiftDown(self) -> bool"""
        return _core_.MouseEvent_ShiftDown(*args, **kwargs)

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
        return _core_.MouseEvent_CmdDown(*args, **kwargs)

    def LeftDown(*args, **kwargs):
        """LeftDown(self) -> bool"""
        return _core_.MouseEvent_LeftDown(*args, **kwargs)

    def MiddleDown(*args, **kwargs):
        """MiddleDown(self) -> bool"""
        return _core_.MouseEvent_MiddleDown(*args, **kwargs)

    def RightDown(*args, **kwargs):
        """RightDown(self) -> bool"""
        return _core_.MouseEvent_RightDown(*args, **kwargs)

    def LeftUp(*args, **kwargs):
        """LeftUp(self) -> bool"""
        return _core_.MouseEvent_LeftUp(*args, **kwargs)

    def MiddleUp(*args, **kwargs):
        """MiddleUp(self) -> bool"""
        return _core_.MouseEvent_MiddleUp(*args, **kwargs)

    def RightUp(*args, **kwargs):
        """RightUp(self) -> bool"""
        return _core_.MouseEvent_RightUp(*args, **kwargs)

    def LeftDClick(*args, **kwargs):
        """LeftDClick(self) -> bool"""
        return _core_.MouseEvent_LeftDClick(*args, **kwargs)

    def MiddleDClick(*args, **kwargs):
        """MiddleDClick(self) -> bool"""
        return _core_.MouseEvent_MiddleDClick(*args, **kwargs)

    def RightDClick(*args, **kwargs):
        """RightDClick(self) -> bool"""
        return _core_.MouseEvent_RightDClick(*args, **kwargs)

    def LeftIsDown(*args, **kwargs):
        """LeftIsDown(self) -> bool"""
        return _core_.MouseEvent_LeftIsDown(*args, **kwargs)

    def MiddleIsDown(*args, **kwargs):
        """MiddleIsDown(self) -> bool"""
        return _core_.MouseEvent_MiddleIsDown(*args, **kwargs)

    def RightIsDown(*args, **kwargs):
        """RightIsDown(self) -> bool"""
        return _core_.MouseEvent_RightIsDown(*args, **kwargs)

    def Dragging(*args, **kwargs):
        """Dragging(self) -> bool"""
        return _core_.MouseEvent_Dragging(*args, **kwargs)

    def Moving(*args, **kwargs):
        """Moving(self) -> bool"""
        return _core_.MouseEvent_Moving(*args, **kwargs)

    def Entering(*args, **kwargs):
        """Entering(self) -> bool"""
        return _core_.MouseEvent_Entering(*args, **kwargs)

    def Leaving(*args, **kwargs):
        """Leaving(self) -> bool"""
        return _core_.MouseEvent_Leaving(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Returns the position of the mouse in window coordinates when the event
        happened.
        """
        return _core_.MouseEvent_GetPosition(*args, **kwargs)

    def GetPositionTuple(*args, **kwargs):
        """
        GetPositionTuple() -> (x,y)

        Returns the position of the mouse in window coordinates when the event
        happened.
        """
        return _core_.MouseEvent_GetPositionTuple(*args, **kwargs)

    def GetLogicalPosition(*args, **kwargs):
        """GetLogicalPosition(self, DC dc) -> Point"""
        return _core_.MouseEvent_GetLogicalPosition(*args, **kwargs)

    def GetX(*args, **kwargs):
        """GetX(self) -> int"""
        return _core_.MouseEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return _core_.MouseEvent_GetY(*args, **kwargs)

    def GetWheelRotation(*args, **kwargs):
        """GetWheelRotation(self) -> int"""
        return _core_.MouseEvent_GetWheelRotation(*args, **kwargs)

    def GetWheelDelta(*args, **kwargs):
        """GetWheelDelta(self) -> int"""
        return _core_.MouseEvent_GetWheelDelta(*args, **kwargs)

    def GetLinesPerAction(*args, **kwargs):
        """GetLinesPerAction(self) -> int"""
        return _core_.MouseEvent_GetLinesPerAction(*args, **kwargs)

    def IsPageScroll(*args, **kwargs):
        """IsPageScroll(self) -> bool"""
        return _core_.MouseEvent_IsPageScroll(*args, **kwargs)

    m_x = property(_core_.MouseEvent_m_x_get, _core_.MouseEvent_m_x_set)
    m_y = property(_core_.MouseEvent_m_y_get, _core_.MouseEvent_m_y_set)
    m_leftDown = property(_core_.MouseEvent_m_leftDown_get, _core_.MouseEvent_m_leftDown_set)
    m_middleDown = property(_core_.MouseEvent_m_middleDown_get, _core_.MouseEvent_m_middleDown_set)
    m_rightDown = property(_core_.MouseEvent_m_rightDown_get, _core_.MouseEvent_m_rightDown_set)
    m_controlDown = property(_core_.MouseEvent_m_controlDown_get, _core_.MouseEvent_m_controlDown_set)
    m_shiftDown = property(_core_.MouseEvent_m_shiftDown_get, _core_.MouseEvent_m_shiftDown_set)
    m_altDown = property(_core_.MouseEvent_m_altDown_get, _core_.MouseEvent_m_altDown_set)
    m_metaDown = property(_core_.MouseEvent_m_metaDown_get, _core_.MouseEvent_m_metaDown_set)
    m_wheelRotation = property(_core_.MouseEvent_m_wheelRotation_get, _core_.MouseEvent_m_wheelRotation_set)
    m_wheelDelta = property(_core_.MouseEvent_m_wheelDelta_get, _core_.MouseEvent_m_wheelDelta_set)
    m_linesPerAction = property(_core_.MouseEvent_m_linesPerAction_get, _core_.MouseEvent_m_linesPerAction_set)

class MouseEventPtr(MouseEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MouseEvent
_core_.MouseEvent_swigregister(MouseEventPtr)

#---------------------------------------------------------------------------

class SetCursorEvent(Event):
    """Proxy of C++ SetCursorEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSetCursorEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int x=0, int y=0) -> SetCursorEvent"""
        newobj = _core_.new_SetCursorEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetX(*args, **kwargs):
        """GetX(self) -> int"""
        return _core_.SetCursorEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return _core_.SetCursorEvent_GetY(*args, **kwargs)

    def SetCursor(*args, **kwargs):
        """SetCursor(self, Cursor cursor)"""
        return _core_.SetCursorEvent_SetCursor(*args, **kwargs)

    def GetCursor(*args, **kwargs):
        """GetCursor(self) -> Cursor"""
        return _core_.SetCursorEvent_GetCursor(*args, **kwargs)

    def HasCursor(*args, **kwargs):
        """HasCursor(self) -> bool"""
        return _core_.SetCursorEvent_HasCursor(*args, **kwargs)


class SetCursorEventPtr(SetCursorEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SetCursorEvent
_core_.SetCursorEvent_swigregister(SetCursorEventPtr)

#---------------------------------------------------------------------------

class KeyEvent(Event):
    """Proxy of C++ KeyEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxKeyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType keyType=wxEVT_NULL) -> KeyEvent"""
        newobj = _core_.new_KeyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def ControlDown(*args, **kwargs):
        """ControlDown(self) -> bool"""
        return _core_.KeyEvent_ControlDown(*args, **kwargs)

    def MetaDown(*args, **kwargs):
        """MetaDown(self) -> bool"""
        return _core_.KeyEvent_MetaDown(*args, **kwargs)

    def AltDown(*args, **kwargs):
        """AltDown(self) -> bool"""
        return _core_.KeyEvent_AltDown(*args, **kwargs)

    def ShiftDown(*args, **kwargs):
        """ShiftDown(self) -> bool"""
        return _core_.KeyEvent_ShiftDown(*args, **kwargs)

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
        return _core_.KeyEvent_CmdDown(*args, **kwargs)

    def HasModifiers(*args, **kwargs):
        """HasModifiers(self) -> bool"""
        return _core_.KeyEvent_HasModifiers(*args, **kwargs)

    def GetKeyCode(*args, **kwargs):
        """GetKeyCode(self) -> int"""
        return _core_.KeyEvent_GetKeyCode(*args, **kwargs)

    KeyCode = GetKeyCode 
    def GetUnicodeKey(*args, **kwargs):
        """GetUnicodeKey(self) -> int"""
        return _core_.KeyEvent_GetUnicodeKey(*args, **kwargs)

    GetUniChar = GetUnicodeKey 
    def GetRawKeyCode(*args, **kwargs):
        """GetRawKeyCode(self) -> unsigned int"""
        return _core_.KeyEvent_GetRawKeyCode(*args, **kwargs)

    def GetRawKeyFlags(*args, **kwargs):
        """GetRawKeyFlags(self) -> unsigned int"""
        return _core_.KeyEvent_GetRawKeyFlags(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Find the position of the event.
        """
        return _core_.KeyEvent_GetPosition(*args, **kwargs)

    def GetPositionTuple(*args, **kwargs):
        """
        GetPositionTuple() -> (x,y)

        Find the position of the event.
        """
        return _core_.KeyEvent_GetPositionTuple(*args, **kwargs)

    def GetX(*args, **kwargs):
        """GetX(self) -> int"""
        return _core_.KeyEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return _core_.KeyEvent_GetY(*args, **kwargs)

    m_x = property(_core_.KeyEvent_m_x_get, _core_.KeyEvent_m_x_set)
    m_y = property(_core_.KeyEvent_m_y_get, _core_.KeyEvent_m_y_set)
    m_keyCode = property(_core_.KeyEvent_m_keyCode_get, _core_.KeyEvent_m_keyCode_set)
    m_controlDown = property(_core_.KeyEvent_m_controlDown_get, _core_.KeyEvent_m_controlDown_set)
    m_shiftDown = property(_core_.KeyEvent_m_shiftDown_get, _core_.KeyEvent_m_shiftDown_set)
    m_altDown = property(_core_.KeyEvent_m_altDown_get, _core_.KeyEvent_m_altDown_set)
    m_metaDown = property(_core_.KeyEvent_m_metaDown_get, _core_.KeyEvent_m_metaDown_set)
    m_scanCode = property(_core_.KeyEvent_m_scanCode_get, _core_.KeyEvent_m_scanCode_set)
    m_rawCode = property(_core_.KeyEvent_m_rawCode_get, _core_.KeyEvent_m_rawCode_set)
    m_rawFlags = property(_core_.KeyEvent_m_rawFlags_get, _core_.KeyEvent_m_rawFlags_set)

class KeyEventPtr(KeyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = KeyEvent
_core_.KeyEvent_swigregister(KeyEventPtr)

#---------------------------------------------------------------------------

class SizeEvent(Event):
    """Proxy of C++ SizeEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSizeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Size sz=DefaultSize, int winid=0) -> SizeEvent"""
        newobj = _core_.new_SizeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetSize(*args, **kwargs):
        """GetSize(self) -> Size"""
        return _core_.SizeEvent_GetSize(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """GetRect(self) -> Rect"""
        return _core_.SizeEvent_GetRect(*args, **kwargs)

    def SetRect(*args, **kwargs):
        """SetRect(self, Rect rect)"""
        return _core_.SizeEvent_SetRect(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, Size size)"""
        return _core_.SizeEvent_SetSize(*args, **kwargs)

    m_size = property(_core_.SizeEvent_m_size_get, _core_.SizeEvent_m_size_set)
    m_rect = property(_core_.SizeEvent_m_rect_get, _core_.SizeEvent_m_rect_set)

class SizeEventPtr(SizeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SizeEvent
_core_.SizeEvent_swigregister(SizeEventPtr)

#---------------------------------------------------------------------------

class MoveEvent(Event):
    """Proxy of C++ MoveEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMoveEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Point pos=DefaultPosition, int winid=0) -> MoveEvent"""
        newobj = _core_.new_MoveEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _core_.MoveEvent_GetPosition(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """GetRect(self) -> Rect"""
        return _core_.MoveEvent_GetRect(*args, **kwargs)

    def SetRect(*args, **kwargs):
        """SetRect(self, Rect rect)"""
        return _core_.MoveEvent_SetRect(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, Point pos)"""
        return _core_.MoveEvent_SetPosition(*args, **kwargs)

    m_pos = property(_core_.MoveEvent_m_pos_get, _core_.MoveEvent_m_pos_set)
    m_rect = property(_core_.MoveEvent_m_rect_get, _core_.MoveEvent_m_rect_set)

class MoveEventPtr(MoveEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MoveEvent
_core_.MoveEvent_swigregister(MoveEventPtr)

#---------------------------------------------------------------------------

class PaintEvent(Event):
    """Proxy of C++ PaintEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPaintEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int Id=0) -> PaintEvent"""
        newobj = _core_.new_PaintEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class PaintEventPtr(PaintEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PaintEvent
_core_.PaintEvent_swigregister(PaintEventPtr)

class NcPaintEvent(Event):
    """Proxy of C++ NcPaintEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNcPaintEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid=0) -> NcPaintEvent"""
        newobj = _core_.new_NcPaintEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class NcPaintEventPtr(NcPaintEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NcPaintEvent
_core_.NcPaintEvent_swigregister(NcPaintEventPtr)

#---------------------------------------------------------------------------

class EraseEvent(Event):
    """Proxy of C++ EraseEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEraseEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int Id=0, DC dc=(wxDC *) NULL) -> EraseEvent"""
        newobj = _core_.new_EraseEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetDC(*args, **kwargs):
        """GetDC(self) -> DC"""
        return _core_.EraseEvent_GetDC(*args, **kwargs)


class EraseEventPtr(EraseEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EraseEvent
_core_.EraseEvent_swigregister(EraseEventPtr)

#---------------------------------------------------------------------------

class FocusEvent(Event):
    """Proxy of C++ FocusEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFocusEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, int winid=0) -> FocusEvent"""
        newobj = _core_.new_FocusEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> Window"""
        return _core_.FocusEvent_GetWindow(*args, **kwargs)

    def SetWindow(*args, **kwargs):
        """SetWindow(self, Window win)"""
        return _core_.FocusEvent_SetWindow(*args, **kwargs)


class FocusEventPtr(FocusEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FocusEvent
_core_.FocusEvent_swigregister(FocusEventPtr)

#---------------------------------------------------------------------------

class ChildFocusEvent(CommandEvent):
    """Proxy of C++ ChildFocusEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxChildFocusEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Window win=None) -> ChildFocusEvent"""
        newobj = _core_.new_ChildFocusEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> Window"""
        return _core_.ChildFocusEvent_GetWindow(*args, **kwargs)


class ChildFocusEventPtr(ChildFocusEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ChildFocusEvent
_core_.ChildFocusEvent_swigregister(ChildFocusEventPtr)

#---------------------------------------------------------------------------

class ActivateEvent(Event):
    """Proxy of C++ ActivateEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxActivateEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, bool active=True, int Id=0) -> ActivateEvent"""
        newobj = _core_.new_ActivateEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetActive(*args, **kwargs):
        """GetActive(self) -> bool"""
        return _core_.ActivateEvent_GetActive(*args, **kwargs)


class ActivateEventPtr(ActivateEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ActivateEvent
_core_.ActivateEvent_swigregister(ActivateEventPtr)

#---------------------------------------------------------------------------

class InitDialogEvent(Event):
    """Proxy of C++ InitDialogEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxInitDialogEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int Id=0) -> InitDialogEvent"""
        newobj = _core_.new_InitDialogEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class InitDialogEventPtr(InitDialogEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = InitDialogEvent
_core_.InitDialogEvent_swigregister(InitDialogEventPtr)

#---------------------------------------------------------------------------

class MenuEvent(Event):
    """Proxy of C++ MenuEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMenuEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, int winid=0, Menu menu=None) -> MenuEvent"""
        newobj = _core_.new_MenuEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetMenuId(*args, **kwargs):
        """GetMenuId(self) -> int"""
        return _core_.MenuEvent_GetMenuId(*args, **kwargs)

    def IsPopup(*args, **kwargs):
        """IsPopup(self) -> bool"""
        return _core_.MenuEvent_IsPopup(*args, **kwargs)

    def GetMenu(*args, **kwargs):
        """GetMenu(self) -> Menu"""
        return _core_.MenuEvent_GetMenu(*args, **kwargs)


class MenuEventPtr(MenuEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MenuEvent
_core_.MenuEvent_swigregister(MenuEventPtr)

#---------------------------------------------------------------------------

class CloseEvent(Event):
    """Proxy of C++ CloseEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCloseEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, int winid=0) -> CloseEvent"""
        newobj = _core_.new_CloseEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetLoggingOff(*args, **kwargs):
        """SetLoggingOff(self, bool logOff)"""
        return _core_.CloseEvent_SetLoggingOff(*args, **kwargs)

    def GetLoggingOff(*args, **kwargs):
        """GetLoggingOff(self) -> bool"""
        return _core_.CloseEvent_GetLoggingOff(*args, **kwargs)

    def Veto(*args, **kwargs):
        """Veto(self, bool veto=True)"""
        return _core_.CloseEvent_Veto(*args, **kwargs)

    def SetCanVeto(*args, **kwargs):
        """SetCanVeto(self, bool canVeto)"""
        return _core_.CloseEvent_SetCanVeto(*args, **kwargs)

    def CanVeto(*args, **kwargs):
        """CanVeto(self) -> bool"""
        return _core_.CloseEvent_CanVeto(*args, **kwargs)

    def GetVeto(*args, **kwargs):
        """GetVeto(self) -> bool"""
        return _core_.CloseEvent_GetVeto(*args, **kwargs)


class CloseEventPtr(CloseEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CloseEvent
_core_.CloseEvent_swigregister(CloseEventPtr)

#---------------------------------------------------------------------------

class ShowEvent(Event):
    """Proxy of C++ ShowEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxShowEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid=0, bool show=False) -> ShowEvent"""
        newobj = _core_.new_ShowEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetShow(*args, **kwargs):
        """SetShow(self, bool show)"""
        return _core_.ShowEvent_SetShow(*args, **kwargs)

    def GetShow(*args, **kwargs):
        """GetShow(self) -> bool"""
        return _core_.ShowEvent_GetShow(*args, **kwargs)


class ShowEventPtr(ShowEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ShowEvent
_core_.ShowEvent_swigregister(ShowEventPtr)

#---------------------------------------------------------------------------

class IconizeEvent(Event):
    """Proxy of C++ IconizeEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIconizeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int id=0, bool iconized=True) -> IconizeEvent"""
        newobj = _core_.new_IconizeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Iconized(*args, **kwargs):
        """Iconized(self) -> bool"""
        return _core_.IconizeEvent_Iconized(*args, **kwargs)


class IconizeEventPtr(IconizeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IconizeEvent
_core_.IconizeEvent_swigregister(IconizeEventPtr)

#---------------------------------------------------------------------------

class MaximizeEvent(Event):
    """Proxy of C++ MaximizeEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMaximizeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int id=0) -> MaximizeEvent"""
        newobj = _core_.new_MaximizeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class MaximizeEventPtr(MaximizeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MaximizeEvent
_core_.MaximizeEvent_swigregister(MaximizeEventPtr)

#---------------------------------------------------------------------------

class DropFilesEvent(Event):
    """Proxy of C++ DropFilesEvent class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDropFilesEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _core_.DropFilesEvent_GetPosition(*args, **kwargs)

    def GetNumberOfFiles(*args, **kwargs):
        """GetNumberOfFiles(self) -> int"""
        return _core_.DropFilesEvent_GetNumberOfFiles(*args, **kwargs)

    def GetFiles(*args, **kwargs):
        """GetFiles(self) -> PyObject"""
        return _core_.DropFilesEvent_GetFiles(*args, **kwargs)


class DropFilesEventPtr(DropFilesEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DropFilesEvent
_core_.DropFilesEvent_swigregister(DropFilesEventPtr)

#---------------------------------------------------------------------------

UPDATE_UI_PROCESS_ALL = _core_.UPDATE_UI_PROCESS_ALL
UPDATE_UI_PROCESS_SPECIFIED = _core_.UPDATE_UI_PROCESS_SPECIFIED
class UpdateUIEvent(CommandEvent):
    """Proxy of C++ UpdateUIEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxUpdateUIEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int commandId=0) -> UpdateUIEvent"""
        newobj = _core_.new_UpdateUIEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetChecked(*args, **kwargs):
        """GetChecked(self) -> bool"""
        return _core_.UpdateUIEvent_GetChecked(*args, **kwargs)

    def GetEnabled(*args, **kwargs):
        """GetEnabled(self) -> bool"""
        return _core_.UpdateUIEvent_GetEnabled(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText(self) -> String"""
        return _core_.UpdateUIEvent_GetText(*args, **kwargs)

    def GetSetText(*args, **kwargs):
        """GetSetText(self) -> bool"""
        return _core_.UpdateUIEvent_GetSetText(*args, **kwargs)

    def GetSetChecked(*args, **kwargs):
        """GetSetChecked(self) -> bool"""
        return _core_.UpdateUIEvent_GetSetChecked(*args, **kwargs)

    def GetSetEnabled(*args, **kwargs):
        """GetSetEnabled(self) -> bool"""
        return _core_.UpdateUIEvent_GetSetEnabled(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(self, bool check)"""
        return _core_.UpdateUIEvent_Check(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(self, bool enable)"""
        return _core_.UpdateUIEvent_Enable(*args, **kwargs)

    def SetText(*args, **kwargs):
        """SetText(self, String text)"""
        return _core_.UpdateUIEvent_SetText(*args, **kwargs)

    def SetUpdateInterval(*args, **kwargs):
        """SetUpdateInterval(long updateInterval)"""
        return _core_.UpdateUIEvent_SetUpdateInterval(*args, **kwargs)

    SetUpdateInterval = staticmethod(SetUpdateInterval)
    def GetUpdateInterval(*args, **kwargs):
        """GetUpdateInterval() -> long"""
        return _core_.UpdateUIEvent_GetUpdateInterval(*args, **kwargs)

    GetUpdateInterval = staticmethod(GetUpdateInterval)
    def CanUpdate(*args, **kwargs):
        """CanUpdate(Window win) -> bool"""
        return _core_.UpdateUIEvent_CanUpdate(*args, **kwargs)

    CanUpdate = staticmethod(CanUpdate)
    def ResetUpdateTime(*args, **kwargs):
        """ResetUpdateTime()"""
        return _core_.UpdateUIEvent_ResetUpdateTime(*args, **kwargs)

    ResetUpdateTime = staticmethod(ResetUpdateTime)
    def SetMode(*args, **kwargs):
        """SetMode(int mode)"""
        return _core_.UpdateUIEvent_SetMode(*args, **kwargs)

    SetMode = staticmethod(SetMode)
    def GetMode(*args, **kwargs):
        """GetMode() -> int"""
        return _core_.UpdateUIEvent_GetMode(*args, **kwargs)

    GetMode = staticmethod(GetMode)

class UpdateUIEventPtr(UpdateUIEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = UpdateUIEvent
_core_.UpdateUIEvent_swigregister(UpdateUIEventPtr)

def UpdateUIEvent_SetUpdateInterval(*args, **kwargs):
    """UpdateUIEvent_SetUpdateInterval(long updateInterval)"""
    return _core_.UpdateUIEvent_SetUpdateInterval(*args, **kwargs)

def UpdateUIEvent_GetUpdateInterval(*args, **kwargs):
    """UpdateUIEvent_GetUpdateInterval() -> long"""
    return _core_.UpdateUIEvent_GetUpdateInterval(*args, **kwargs)

def UpdateUIEvent_CanUpdate(*args, **kwargs):
    """UpdateUIEvent_CanUpdate(Window win) -> bool"""
    return _core_.UpdateUIEvent_CanUpdate(*args, **kwargs)

def UpdateUIEvent_ResetUpdateTime(*args, **kwargs):
    """UpdateUIEvent_ResetUpdateTime()"""
    return _core_.UpdateUIEvent_ResetUpdateTime(*args, **kwargs)

def UpdateUIEvent_SetMode(*args, **kwargs):
    """UpdateUIEvent_SetMode(int mode)"""
    return _core_.UpdateUIEvent_SetMode(*args, **kwargs)

def UpdateUIEvent_GetMode(*args, **kwargs):
    """UpdateUIEvent_GetMode() -> int"""
    return _core_.UpdateUIEvent_GetMode(*args, **kwargs)

#---------------------------------------------------------------------------

class SysColourChangedEvent(Event):
    """Proxy of C++ SysColourChangedEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSysColourChangedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> SysColourChangedEvent"""
        newobj = _core_.new_SysColourChangedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class SysColourChangedEventPtr(SysColourChangedEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SysColourChangedEvent
_core_.SysColourChangedEvent_swigregister(SysColourChangedEventPtr)

#---------------------------------------------------------------------------

class MouseCaptureChangedEvent(Event):
    """Proxy of C++ MouseCaptureChangedEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMouseCaptureChangedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid=0, Window gainedCapture=None) -> MouseCaptureChangedEvent"""
        newobj = _core_.new_MouseCaptureChangedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetCapturedWindow(*args, **kwargs):
        """GetCapturedWindow(self) -> Window"""
        return _core_.MouseCaptureChangedEvent_GetCapturedWindow(*args, **kwargs)


class MouseCaptureChangedEventPtr(MouseCaptureChangedEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MouseCaptureChangedEvent
_core_.MouseCaptureChangedEvent_swigregister(MouseCaptureChangedEventPtr)

#---------------------------------------------------------------------------

class DisplayChangedEvent(Event):
    """Proxy of C++ DisplayChangedEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDisplayChangedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> DisplayChangedEvent"""
        newobj = _core_.new_DisplayChangedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class DisplayChangedEventPtr(DisplayChangedEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DisplayChangedEvent
_core_.DisplayChangedEvent_swigregister(DisplayChangedEventPtr)

#---------------------------------------------------------------------------

class PaletteChangedEvent(Event):
    """Proxy of C++ PaletteChangedEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPaletteChangedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int id=0) -> PaletteChangedEvent"""
        newobj = _core_.new_PaletteChangedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetChangedWindow(*args, **kwargs):
        """SetChangedWindow(self, Window win)"""
        return _core_.PaletteChangedEvent_SetChangedWindow(*args, **kwargs)

    def GetChangedWindow(*args, **kwargs):
        """GetChangedWindow(self) -> Window"""
        return _core_.PaletteChangedEvent_GetChangedWindow(*args, **kwargs)


class PaletteChangedEventPtr(PaletteChangedEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PaletteChangedEvent
_core_.PaletteChangedEvent_swigregister(PaletteChangedEventPtr)

#---------------------------------------------------------------------------

class QueryNewPaletteEvent(Event):
    """Proxy of C++ QueryNewPaletteEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxQueryNewPaletteEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid=0) -> QueryNewPaletteEvent"""
        newobj = _core_.new_QueryNewPaletteEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetPaletteRealized(*args, **kwargs):
        """SetPaletteRealized(self, bool realized)"""
        return _core_.QueryNewPaletteEvent_SetPaletteRealized(*args, **kwargs)

    def GetPaletteRealized(*args, **kwargs):
        """GetPaletteRealized(self) -> bool"""
        return _core_.QueryNewPaletteEvent_GetPaletteRealized(*args, **kwargs)


class QueryNewPaletteEventPtr(QueryNewPaletteEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = QueryNewPaletteEvent
_core_.QueryNewPaletteEvent_swigregister(QueryNewPaletteEventPtr)

#---------------------------------------------------------------------------

class NavigationKeyEvent(Event):
    """Proxy of C++ NavigationKeyEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNavigationKeyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> NavigationKeyEvent"""
        newobj = _core_.new_NavigationKeyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetDirection(*args, **kwargs):
        """GetDirection(self) -> bool"""
        return _core_.NavigationKeyEvent_GetDirection(*args, **kwargs)

    def SetDirection(*args, **kwargs):
        """SetDirection(self, bool forward)"""
        return _core_.NavigationKeyEvent_SetDirection(*args, **kwargs)

    def IsWindowChange(*args, **kwargs):
        """IsWindowChange(self) -> bool"""
        return _core_.NavigationKeyEvent_IsWindowChange(*args, **kwargs)

    def SetWindowChange(*args, **kwargs):
        """SetWindowChange(self, bool ischange)"""
        return _core_.NavigationKeyEvent_SetWindowChange(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(self, long flags)"""
        return _core_.NavigationKeyEvent_SetFlags(*args, **kwargs)

    def GetCurrentFocus(*args, **kwargs):
        """GetCurrentFocus(self) -> Window"""
        return _core_.NavigationKeyEvent_GetCurrentFocus(*args, **kwargs)

    def SetCurrentFocus(*args, **kwargs):
        """SetCurrentFocus(self, Window win)"""
        return _core_.NavigationKeyEvent_SetCurrentFocus(*args, **kwargs)

    IsBackward = _core_.NavigationKeyEvent_IsBackward
    IsForward = _core_.NavigationKeyEvent_IsForward
    WinChange = _core_.NavigationKeyEvent_WinChange

class NavigationKeyEventPtr(NavigationKeyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NavigationKeyEvent
_core_.NavigationKeyEvent_swigregister(NavigationKeyEventPtr)

#---------------------------------------------------------------------------

class WindowCreateEvent(CommandEvent):
    """Proxy of C++ WindowCreateEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWindowCreateEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Window win=None) -> WindowCreateEvent"""
        newobj = _core_.new_WindowCreateEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> Window"""
        return _core_.WindowCreateEvent_GetWindow(*args, **kwargs)


class WindowCreateEventPtr(WindowCreateEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WindowCreateEvent
_core_.WindowCreateEvent_swigregister(WindowCreateEventPtr)

class WindowDestroyEvent(CommandEvent):
    """Proxy of C++ WindowDestroyEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWindowDestroyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Window win=None) -> WindowDestroyEvent"""
        newobj = _core_.new_WindowDestroyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> Window"""
        return _core_.WindowDestroyEvent_GetWindow(*args, **kwargs)


class WindowDestroyEventPtr(WindowDestroyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WindowDestroyEvent
_core_.WindowDestroyEvent_swigregister(WindowDestroyEventPtr)

#---------------------------------------------------------------------------

class ContextMenuEvent(CommandEvent):
    """Proxy of C++ ContextMenuEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxContextMenuEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, int winid=0, Point pt=DefaultPosition) -> ContextMenuEvent"""
        newobj = _core_.new_ContextMenuEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _core_.ContextMenuEvent_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, Point pos)"""
        return _core_.ContextMenuEvent_SetPosition(*args, **kwargs)


class ContextMenuEventPtr(ContextMenuEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ContextMenuEvent
_core_.ContextMenuEvent_swigregister(ContextMenuEventPtr)

#---------------------------------------------------------------------------

IDLE_PROCESS_ALL = _core_.IDLE_PROCESS_ALL
IDLE_PROCESS_SPECIFIED = _core_.IDLE_PROCESS_SPECIFIED
class IdleEvent(Event):
    """Proxy of C++ IdleEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIdleEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> IdleEvent"""
        newobj = _core_.new_IdleEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def RequestMore(*args, **kwargs):
        """RequestMore(self, bool needMore=True)"""
        return _core_.IdleEvent_RequestMore(*args, **kwargs)

    def MoreRequested(*args, **kwargs):
        """MoreRequested(self) -> bool"""
        return _core_.IdleEvent_MoreRequested(*args, **kwargs)

    def SetMode(*args, **kwargs):
        """SetMode(int mode)"""
        return _core_.IdleEvent_SetMode(*args, **kwargs)

    SetMode = staticmethod(SetMode)
    def GetMode(*args, **kwargs):
        """GetMode() -> int"""
        return _core_.IdleEvent_GetMode(*args, **kwargs)

    GetMode = staticmethod(GetMode)
    def CanSend(*args, **kwargs):
        """CanSend(Window win) -> bool"""
        return _core_.IdleEvent_CanSend(*args, **kwargs)

    CanSend = staticmethod(CanSend)

class IdleEventPtr(IdleEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IdleEvent
_core_.IdleEvent_swigregister(IdleEventPtr)

def IdleEvent_SetMode(*args, **kwargs):
    """IdleEvent_SetMode(int mode)"""
    return _core_.IdleEvent_SetMode(*args, **kwargs)

def IdleEvent_GetMode(*args, **kwargs):
    """IdleEvent_GetMode() -> int"""
    return _core_.IdleEvent_GetMode(*args, **kwargs)

def IdleEvent_CanSend(*args, **kwargs):
    """IdleEvent_CanSend(Window win) -> bool"""
    return _core_.IdleEvent_CanSend(*args, **kwargs)

#---------------------------------------------------------------------------

class PyEvent(Event):
    """Proxy of C++ PyEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid=0, wxEventType commandType=wxEVT_NULL) -> PyEvent"""
        newobj = _core_.new_PyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self.SetSelf(self)

    def __del__(self, destroy=_core_.delete_PyEvent):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetSelf(*args, **kwargs):
        """SetSelf(self, PyObject self)"""
        return _core_.PyEvent_SetSelf(*args, **kwargs)

    def GetSelf(*args, **kwargs):
        """GetSelf(self) -> PyObject"""
        return _core_.PyEvent_GetSelf(*args, **kwargs)


class PyEventPtr(PyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyEvent
_core_.PyEvent_swigregister(PyEventPtr)

class PyCommandEvent(CommandEvent):
    """Proxy of C++ PyCommandEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyCommandEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int id=0) -> PyCommandEvent"""
        newobj = _core_.new_PyCommandEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self.SetSelf(self)

    def __del__(self, destroy=_core_.delete_PyCommandEvent):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetSelf(*args, **kwargs):
        """SetSelf(self, PyObject self)"""
        return _core_.PyCommandEvent_SetSelf(*args, **kwargs)

    def GetSelf(*args, **kwargs):
        """GetSelf(self) -> PyObject"""
        return _core_.PyCommandEvent_GetSelf(*args, **kwargs)


class PyCommandEventPtr(PyCommandEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyCommandEvent
_core_.PyCommandEvent_swigregister(PyCommandEventPtr)

#---------------------------------------------------------------------------

PYAPP_ASSERT_SUPPRESS = _core_.PYAPP_ASSERT_SUPPRESS
PYAPP_ASSERT_EXCEPTION = _core_.PYAPP_ASSERT_EXCEPTION
PYAPP_ASSERT_DIALOG = _core_.PYAPP_ASSERT_DIALOG
PYAPP_ASSERT_LOG = _core_.PYAPP_ASSERT_LOG
PRINT_WINDOWS = _core_.PRINT_WINDOWS
PRINT_POSTSCRIPT = _core_.PRINT_POSTSCRIPT
class PyApp(EvtHandler):
    """
    The ``wx.PyApp`` class is an *implementation detail*, please use the
    `wx.App` class (or some other derived class) instead.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyApp instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> PyApp

        Create a new application object, starting the bootstrap process.
        """
        newobj = _core_.new_PyApp(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyApp)
        self._setOORInfo(self)

    def __del__(self, destroy=_core_.delete_PyApp):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _core_.PyApp__setCallbackInfo(*args, **kwargs)

    def GetAppName(*args, **kwargs):
        """
        GetAppName(self) -> String

        Get the application name.
        """
        return _core_.PyApp_GetAppName(*args, **kwargs)

    def SetAppName(*args, **kwargs):
        """
        SetAppName(self, String name)

        Set the application name. This value may be used automatically by
        `wx.Config` and such.
        """
        return _core_.PyApp_SetAppName(*args, **kwargs)

    def GetClassName(*args, **kwargs):
        """
        GetClassName(self) -> String

        Get the application's class name.
        """
        return _core_.PyApp_GetClassName(*args, **kwargs)

    def SetClassName(*args, **kwargs):
        """
        SetClassName(self, String name)

        Set the application's class name. This value may be used for
        X-resources if applicable for the platform
        """
        return _core_.PyApp_SetClassName(*args, **kwargs)

    def GetVendorName(*args, **kwargs):
        """
        GetVendorName(self) -> String

        Get the application's vendor name.
        """
        return _core_.PyApp_GetVendorName(*args, **kwargs)

    def SetVendorName(*args, **kwargs):
        """
        SetVendorName(self, String name)

        Set the application's vendor name. This value may be used
        automatically by `wx.Config` and such.
        """
        return _core_.PyApp_SetVendorName(*args, **kwargs)

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
        return _core_.PyApp_GetTraits(*args, **kwargs)

    def ProcessPendingEvents(*args, **kwargs):
        """
        ProcessPendingEvents(self)

        Process all events in the Pending Events list -- it is necessary to
        call this function to process posted events. This normally happens
        during each event loop iteration.
        """
        return _core_.PyApp_ProcessPendingEvents(*args, **kwargs)

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
        return _core_.PyApp_Yield(*args, **kwargs)

    def WakeUpIdle(*args, **kwargs):
        """
        WakeUpIdle(self)

        Make sure that idle events are sent again.
        :see: `wx.WakeUpIdle`
        """
        return _core_.PyApp_WakeUpIdle(*args, **kwargs)

    def IsMainLoopRunning(*args, **kwargs):
        """
        IsMainLoopRunning() -> bool

        Returns True if we're running the main loop, i.e. if the events can
        currently be dispatched.
        """
        return _core_.PyApp_IsMainLoopRunning(*args, **kwargs)

    IsMainLoopRunning = staticmethod(IsMainLoopRunning)
    def MainLoop(*args, **kwargs):
        """
        MainLoop(self) -> int

        Execute the main GUI loop, the function doesn't normally return until
        all top level windows have been closed and destroyed.
        """
        return _core_.PyApp_MainLoop(*args, **kwargs)

    def Exit(*args, **kwargs):
        """
        Exit(self)

        Exit the main loop thus terminating the application.
        :see: `wx.Exit`
        """
        return _core_.PyApp_Exit(*args, **kwargs)

    def ExitMainLoop(*args, **kwargs):
        """
        ExitMainLoop(self)

        Exit the main GUI loop during the next iteration of the main
        loop, (i.e. it does not stop the program immediately!)
        """
        return _core_.PyApp_ExitMainLoop(*args, **kwargs)

    def Pending(*args, **kwargs):
        """
        Pending(self) -> bool

        Returns True if there are unprocessed events in the event queue.
        """
        return _core_.PyApp_Pending(*args, **kwargs)

    def Dispatch(*args, **kwargs):
        """
        Dispatch(self) -> bool

        Process the first event in the event queue (blocks until an event
        appears if there are none currently)
        """
        return _core_.PyApp_Dispatch(*args, **kwargs)

    def ProcessIdle(*args, **kwargs):
        """
        ProcessIdle(self) -> bool

        Called from the MainLoop when the application becomes idle (there are
        no pending events) and sends a `wx.IdleEvent` to all interested
        parties.  Returns True if more idle events are needed, False if not.
        """
        return _core_.PyApp_ProcessIdle(*args, **kwargs)

    def SendIdleEvents(*args, **kwargs):
        """
        SendIdleEvents(self, Window win, IdleEvent event) -> bool

        Send idle event to window and all subwindows.  Returns True if more
        idle time is requested.
        """
        return _core_.PyApp_SendIdleEvents(*args, **kwargs)

    def IsActive(*args, **kwargs):
        """
        IsActive(self) -> bool

        Return True if our app has focus.
        """
        return _core_.PyApp_IsActive(*args, **kwargs)

    def SetTopWindow(*args, **kwargs):
        """
        SetTopWindow(self, Window win)

        Set the *main* top level window
        """
        return _core_.PyApp_SetTopWindow(*args, **kwargs)

    def GetTopWindow(*args, **kwargs):
        """
        GetTopWindow(self) -> Window

        Return the *main* top level window (if it hadn't been set previously
        with SetTopWindow(), will return just some top level window and, if
        there not any, will return None)
        """
        return _core_.PyApp_GetTopWindow(*args, **kwargs)

    def SetExitOnFrameDelete(*args, **kwargs):
        """
        SetExitOnFrameDelete(self, bool flag)

        Control the exit behaviour: by default, the program will exit the main
        loop (and so, usually, terminate) when the last top-level program
        window is deleted.  Beware that if you disable this behaviour (with
        SetExitOnFrameDelete(False)), you'll have to call ExitMainLoop()
        explicitly from somewhere.
        """
        return _core_.PyApp_SetExitOnFrameDelete(*args, **kwargs)

    def GetExitOnFrameDelete(*args, **kwargs):
        """
        GetExitOnFrameDelete(self) -> bool

        Get the current exit behaviour setting.
        """
        return _core_.PyApp_GetExitOnFrameDelete(*args, **kwargs)

    def SetUseBestVisual(*args, **kwargs):
        """
        SetUseBestVisual(self, bool flag)

        Set whether the app should try to use the best available visual on
        systems where more than one is available, (Sun, SGI, XFree86 4, etc.)
        """
        return _core_.PyApp_SetUseBestVisual(*args, **kwargs)

    def GetUseBestVisual(*args, **kwargs):
        """
        GetUseBestVisual(self) -> bool

        Get current UseBestVisual setting.
        """
        return _core_.PyApp_GetUseBestVisual(*args, **kwargs)

    def SetPrintMode(*args, **kwargs):
        """SetPrintMode(self, int mode)"""
        return _core_.PyApp_SetPrintMode(*args, **kwargs)

    def GetPrintMode(*args, **kwargs):
        """GetPrintMode(self) -> int"""
        return _core_.PyApp_GetPrintMode(*args, **kwargs)

    def SetAssertMode(*args, **kwargs):
        """
        SetAssertMode(self, int mode)

        Set the OnAssert behaviour for debug and hybrid builds.
        """
        return _core_.PyApp_SetAssertMode(*args, **kwargs)

    def GetAssertMode(*args, **kwargs):
        """
        GetAssertMode(self) -> int

        Get the current OnAssert behaviour setting.
        """
        return _core_.PyApp_GetAssertMode(*args, **kwargs)

    def GetMacSupportPCMenuShortcuts(*args, **kwargs):
        """GetMacSupportPCMenuShortcuts() -> bool"""
        return _core_.PyApp_GetMacSupportPCMenuShortcuts(*args, **kwargs)

    GetMacSupportPCMenuShortcuts = staticmethod(GetMacSupportPCMenuShortcuts)
    def GetMacAboutMenuItemId(*args, **kwargs):
        """GetMacAboutMenuItemId() -> long"""
        return _core_.PyApp_GetMacAboutMenuItemId(*args, **kwargs)

    GetMacAboutMenuItemId = staticmethod(GetMacAboutMenuItemId)
    def GetMacPreferencesMenuItemId(*args, **kwargs):
        """GetMacPreferencesMenuItemId() -> long"""
        return _core_.PyApp_GetMacPreferencesMenuItemId(*args, **kwargs)

    GetMacPreferencesMenuItemId = staticmethod(GetMacPreferencesMenuItemId)
    def GetMacExitMenuItemId(*args, **kwargs):
        """GetMacExitMenuItemId() -> long"""
        return _core_.PyApp_GetMacExitMenuItemId(*args, **kwargs)

    GetMacExitMenuItemId = staticmethod(GetMacExitMenuItemId)
    def GetMacHelpMenuTitleName(*args, **kwargs):
        """GetMacHelpMenuTitleName() -> String"""
        return _core_.PyApp_GetMacHelpMenuTitleName(*args, **kwargs)

    GetMacHelpMenuTitleName = staticmethod(GetMacHelpMenuTitleName)
    def SetMacSupportPCMenuShortcuts(*args, **kwargs):
        """SetMacSupportPCMenuShortcuts(bool val)"""
        return _core_.PyApp_SetMacSupportPCMenuShortcuts(*args, **kwargs)

    SetMacSupportPCMenuShortcuts = staticmethod(SetMacSupportPCMenuShortcuts)
    def SetMacAboutMenuItemId(*args, **kwargs):
        """SetMacAboutMenuItemId(long val)"""
        return _core_.PyApp_SetMacAboutMenuItemId(*args, **kwargs)

    SetMacAboutMenuItemId = staticmethod(SetMacAboutMenuItemId)
    def SetMacPreferencesMenuItemId(*args, **kwargs):
        """SetMacPreferencesMenuItemId(long val)"""
        return _core_.PyApp_SetMacPreferencesMenuItemId(*args, **kwargs)

    SetMacPreferencesMenuItemId = staticmethod(SetMacPreferencesMenuItemId)
    def SetMacExitMenuItemId(*args, **kwargs):
        """SetMacExitMenuItemId(long val)"""
        return _core_.PyApp_SetMacExitMenuItemId(*args, **kwargs)

    SetMacExitMenuItemId = staticmethod(SetMacExitMenuItemId)
    def SetMacHelpMenuTitleName(*args, **kwargs):
        """SetMacHelpMenuTitleName(String val)"""
        return _core_.PyApp_SetMacHelpMenuTitleName(*args, **kwargs)

    SetMacHelpMenuTitleName = staticmethod(SetMacHelpMenuTitleName)
    def _BootstrapApp(*args, **kwargs):
        """
        _BootstrapApp(self)

        For internal use only
        """
        return _core_.PyApp__BootstrapApp(*args, **kwargs)

    def GetComCtl32Version(*args, **kwargs):
        """
        GetComCtl32Version() -> int

        Returns 400, 470, 471, etc. for comctl32.dll 4.00, 4.70, 4.71 or 0 if
        it wasn't found at all.  Raises an exception on non-Windows platforms.
        """
        return _core_.PyApp_GetComCtl32Version(*args, **kwargs)

    GetComCtl32Version = staticmethod(GetComCtl32Version)

class PyAppPtr(PyApp):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyApp
_core_.PyApp_swigregister(PyAppPtr)

def PyApp_IsMainLoopRunning(*args, **kwargs):
    """
    PyApp_IsMainLoopRunning() -> bool

    Returns True if we're running the main loop, i.e. if the events can
    currently be dispatched.
    """
    return _core_.PyApp_IsMainLoopRunning(*args, **kwargs)

def PyApp_GetMacSupportPCMenuShortcuts(*args, **kwargs):
    """PyApp_GetMacSupportPCMenuShortcuts() -> bool"""
    return _core_.PyApp_GetMacSupportPCMenuShortcuts(*args, **kwargs)

def PyApp_GetMacAboutMenuItemId(*args, **kwargs):
    """PyApp_GetMacAboutMenuItemId() -> long"""
    return _core_.PyApp_GetMacAboutMenuItemId(*args, **kwargs)

def PyApp_GetMacPreferencesMenuItemId(*args, **kwargs):
    """PyApp_GetMacPreferencesMenuItemId() -> long"""
    return _core_.PyApp_GetMacPreferencesMenuItemId(*args, **kwargs)

def PyApp_GetMacExitMenuItemId(*args, **kwargs):
    """PyApp_GetMacExitMenuItemId() -> long"""
    return _core_.PyApp_GetMacExitMenuItemId(*args, **kwargs)

def PyApp_GetMacHelpMenuTitleName(*args, **kwargs):
    """PyApp_GetMacHelpMenuTitleName() -> String"""
    return _core_.PyApp_GetMacHelpMenuTitleName(*args, **kwargs)

def PyApp_SetMacSupportPCMenuShortcuts(*args, **kwargs):
    """PyApp_SetMacSupportPCMenuShortcuts(bool val)"""
    return _core_.PyApp_SetMacSupportPCMenuShortcuts(*args, **kwargs)

def PyApp_SetMacAboutMenuItemId(*args, **kwargs):
    """PyApp_SetMacAboutMenuItemId(long val)"""
    return _core_.PyApp_SetMacAboutMenuItemId(*args, **kwargs)

def PyApp_SetMacPreferencesMenuItemId(*args, **kwargs):
    """PyApp_SetMacPreferencesMenuItemId(long val)"""
    return _core_.PyApp_SetMacPreferencesMenuItemId(*args, **kwargs)

def PyApp_SetMacExitMenuItemId(*args, **kwargs):
    """PyApp_SetMacExitMenuItemId(long val)"""
    return _core_.PyApp_SetMacExitMenuItemId(*args, **kwargs)

def PyApp_SetMacHelpMenuTitleName(*args, **kwargs):
    """PyApp_SetMacHelpMenuTitleName(String val)"""
    return _core_.PyApp_SetMacHelpMenuTitleName(*args, **kwargs)

def PyApp_GetComCtl32Version(*args, **kwargs):
    """
    PyApp_GetComCtl32Version() -> int

    Returns 400, 470, 471, etc. for comctl32.dll 4.00, 4.70, 4.71 or 0 if
    it wasn't found at all.  Raises an exception on non-Windows platforms.
    """
    return _core_.PyApp_GetComCtl32Version(*args, **kwargs)

#---------------------------------------------------------------------------


def Exit(*args, **kwargs):
    """
    Exit()

    Force an exit of the application.  Convenience for wx.GetApp().Exit()
    """
    return _core_.Exit(*args, **kwargs)

def Yield(*args, **kwargs):
    """
    Yield() -> bool

    Yield to other apps/messages.  Convenience for wx.GetApp().Yield()
    """
    return _core_.Yield(*args, **kwargs)

def YieldIfNeeded(*args, **kwargs):
    """
    YieldIfNeeded() -> bool

    Yield to other apps/messages.  Convenience for wx.GetApp().Yield(True)
    """
    return _core_.YieldIfNeeded(*args, **kwargs)

def SafeYield(*args, **kwargs):
    """
    SafeYield(Window win=None, bool onlyIfNeeded=False) -> bool

    This function is similar to `wx.Yield`, except that it disables the
    user input to all program windows before calling `wx.Yield` and
    re-enables it again afterwards. If ``win`` is not None, this window
    will remain enabled, allowing the implementation of some limited user
    interaction.

    :Returns: the result of the call to `wx.Yield`.
    """
    return _core_.SafeYield(*args, **kwargs)

def WakeUpIdle(*args, **kwargs):
    """
    WakeUpIdle()

    Cause the message queue to become empty again, so idle events will be
    sent.
    """
    return _core_.WakeUpIdle(*args, **kwargs)

def PostEvent(*args, **kwargs):
    """
    PostEvent(EvtHandler dest, Event event)

    Send an event to a window or other wx.EvtHandler to be processed
    later.
    """
    return _core_.PostEvent(*args, **kwargs)

def App_CleanUp(*args, **kwargs):
    """
    App_CleanUp()

    For internal use only, it is used to cleanup after wxWidgets when
    Python shuts down.
    """
    return _core_.App_CleanUp(*args, **kwargs)

def GetApp(*args, **kwargs):
    """
    GetApp() -> PyApp

    Return a reference to the current wx.App object.
    """
    return _core_.GetApp(*args, **kwargs)

def SetDefaultPyEncoding(*args, **kwargs):
    """
    SetDefaultPyEncoding(string encoding)

    Sets the encoding that wxPython will use when it needs to convert a
    Python string or unicode object to or from a wxString.
    """
    return _core_.SetDefaultPyEncoding(*args, **kwargs)

def GetDefaultPyEncoding(*args, **kwargs):
    """
    GetDefaultPyEncoding() -> string

    Gets the current encoding that wxPython will use when it needs to
    convert a Python string or unicode object to or from a wxString.
    """
    return _core_.GetDefaultPyEncoding(*args, **kwargs)
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

class EventLoop(object):
    """Proxy of C++ EventLoop class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEventLoop instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> EventLoop"""
        newobj = _core_.new_EventLoop(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_EventLoop):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Run(*args, **kwargs):
        """Run(self) -> int"""
        return _core_.EventLoop_Run(*args, **kwargs)

    def Exit(*args, **kwargs):
        """Exit(self, int rc=0)"""
        return _core_.EventLoop_Exit(*args, **kwargs)

    def Pending(*args, **kwargs):
        """Pending(self) -> bool"""
        return _core_.EventLoop_Pending(*args, **kwargs)

    def Dispatch(*args, **kwargs):
        """Dispatch(self) -> bool"""
        return _core_.EventLoop_Dispatch(*args, **kwargs)

    def IsRunning(*args, **kwargs):
        """IsRunning(self) -> bool"""
        return _core_.EventLoop_IsRunning(*args, **kwargs)

    def GetActive(*args, **kwargs):
        """GetActive() -> EventLoop"""
        return _core_.EventLoop_GetActive(*args, **kwargs)

    GetActive = staticmethod(GetActive)
    def SetActive(*args, **kwargs):
        """SetActive(EventLoop loop)"""
        return _core_.EventLoop_SetActive(*args, **kwargs)

    SetActive = staticmethod(SetActive)

class EventLoopPtr(EventLoop):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EventLoop
_core_.EventLoop_swigregister(EventLoopPtr)

def EventLoop_GetActive(*args, **kwargs):
    """EventLoop_GetActive() -> EventLoop"""
    return _core_.EventLoop_GetActive(*args, **kwargs)

def EventLoop_SetActive(*args, **kwargs):
    """EventLoop_SetActive(EventLoop loop)"""
    return _core_.EventLoop_SetActive(*args, **kwargs)

#---------------------------------------------------------------------------

class AcceleratorEntry(object):
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
        __init__(self, int flags=0, int keyCode=0, int cmdID=0) -> AcceleratorEntry

        Construct a wx.AcceleratorEntry.
        """
        newobj = _core_.new_AcceleratorEntry(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_AcceleratorEntry):
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
        return _core_.AcceleratorEntry_Set(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """
        GetFlags(self) -> int

        Get the AcceleratorEntry's flags.
        """
        return _core_.AcceleratorEntry_GetFlags(*args, **kwargs)

    def GetKeyCode(*args, **kwargs):
        """
        GetKeyCode(self) -> int

        Get the AcceleratorEntry's keycode.
        """
        return _core_.AcceleratorEntry_GetKeyCode(*args, **kwargs)

    def GetCommand(*args, **kwargs):
        """
        GetCommand(self) -> int

        Get the AcceleratorEntry's command ID.
        """
        return _core_.AcceleratorEntry_GetCommand(*args, **kwargs)


class AcceleratorEntryPtr(AcceleratorEntry):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = AcceleratorEntry
_core_.AcceleratorEntry_swigregister(AcceleratorEntryPtr)

class AcceleratorTable(Object):
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
        newobj = _core_.new_AcceleratorTable(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_AcceleratorTable):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _core_.AcceleratorTable_Ok(*args, **kwargs)


class AcceleratorTablePtr(AcceleratorTable):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = AcceleratorTable
_core_.AcceleratorTable_swigregister(AcceleratorTablePtr)


def GetAccelFromString(*args, **kwargs):
    """GetAccelFromString(String label) -> AcceleratorEntry"""
    return _core_.GetAccelFromString(*args, **kwargs)
#---------------------------------------------------------------------------

class VisualAttributes(object):
    """struct containing all the visual attributes of a control"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxVisualAttributes instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> VisualAttributes

        struct containing all the visual attributes of a control
        """
        newobj = _core_.new_VisualAttributes(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_core_.delete_VisualAttributes):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    font = property(_core_.VisualAttributes_font_get, _core_.VisualAttributes_font_set)
    colFg = property(_core_.VisualAttributes_colFg_get, _core_.VisualAttributes_colFg_set)
    colBg = property(_core_.VisualAttributes_colBg_get, _core_.VisualAttributes_colBg_set)

class VisualAttributesPtr(VisualAttributes):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = VisualAttributes
_core_.VisualAttributes_swigregister(VisualAttributesPtr)
NullAcceleratorTable = cvar.NullAcceleratorTable
PanelNameStr = cvar.PanelNameStr

WINDOW_VARIANT_NORMAL = _core_.WINDOW_VARIANT_NORMAL
WINDOW_VARIANT_SMALL = _core_.WINDOW_VARIANT_SMALL
WINDOW_VARIANT_MINI = _core_.WINDOW_VARIANT_MINI
WINDOW_VARIANT_LARGE = _core_.WINDOW_VARIANT_LARGE
WINDOW_VARIANT_MAX = _core_.WINDOW_VARIANT_MAX
class Window(EvtHandler):
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
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=PanelNameStr) -> Window

        Construct and show a generic Window.
        """
        newobj = _core_.new_Window(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=PanelNameStr) -> bool

        Create the GUI part of the Window for 2-phase creation mode.
        """
        return _core_.Window_Create(*args, **kwargs)

    def Close(*args, **kwargs):
        """
        Close(self, bool force=False) -> bool

        This function simply generates a EVT_CLOSE event whose handler usually
        tries to close the window. It doesn't close the window itself,
        however.  If force is False (the default) then the window's close
        handler will be allowed to veto the destruction of the window.
        """
        return _core_.Window_Close(*args, **kwargs)

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
        return _core_.Window_Destroy(*args, **kwargs)

    def DestroyChildren(*args, **kwargs):
        """
        DestroyChildren(self) -> bool

        Destroys all children of a window. Called automatically by the
        destructor.
        """
        return _core_.Window_DestroyChildren(*args, **kwargs)

    def IsBeingDeleted(*args, **kwargs):
        """
        IsBeingDeleted(self) -> bool

        Is the window in the process of being deleted?
        """
        return _core_.Window_IsBeingDeleted(*args, **kwargs)

    def SetTitle(*args, **kwargs):
        """
        SetTitle(self, String title)

        Sets the window's title. Applicable only to frames and dialogs.
        """
        return _core_.Window_SetTitle(*args, **kwargs)

    def GetTitle(*args, **kwargs):
        """
        GetTitle(self) -> String

        Gets the window's title. Applicable only to frames and dialogs.
        """
        return _core_.Window_GetTitle(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """
        SetLabel(self, String label)

        Set the text which the window shows in its label if applicable.
        """
        return _core_.Window_SetLabel(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """
        GetLabel(self) -> String

        Generic way of getting a label from any window, for identification
        purposes.  The interpretation of this function differs from class to
        class. For frames and dialogs, the value returned is the title. For
        buttons or static text controls, it is the button text. This function
        can be useful for meta-programs such as testing tools or special-needs
        access programs)which need to identify windows by name.
        """
        return _core_.Window_GetLabel(*args, **kwargs)

    def SetName(*args, **kwargs):
        """
        SetName(self, String name)

        Sets the window's name.  The window name is used for ressource setting
        in X, it is not the same as the window title/label
        """
        return _core_.Window_SetName(*args, **kwargs)

    def GetName(*args, **kwargs):
        """
        GetName(self) -> String

        Returns the windows name.  This name is not guaranteed to be unique;
        it is up to the programmer to supply an appropriate name in the window
        constructor or via wx.Window.SetName.
        """
        return _core_.Window_GetName(*args, **kwargs)

    def SetWindowVariant(*args, **kwargs):
        """
        SetWindowVariant(self, int variant)

        Sets the variant of the window/font size to use for this window, if
        the platform supports variants, for example, wxMac.
        """
        return _core_.Window_SetWindowVariant(*args, **kwargs)

    def GetWindowVariant(*args, **kwargs):
        """GetWindowVariant(self) -> int"""
        return _core_.Window_GetWindowVariant(*args, **kwargs)

    def SetId(*args, **kwargs):
        """
        SetId(self, int winid)

        Sets the identifier of the window.  Each window has an integer
        identifier. If the application has not provided one, an identifier
        will be generated. Normally, the identifier should be provided on
        creation and should not be modified subsequently.
        """
        return _core_.Window_SetId(*args, **kwargs)

    def GetId(*args, **kwargs):
        """
        GetId(self) -> int

        Returns the identifier of the window.  Each window has an integer
        identifier. If the application has not provided one (or the default Id
        -1 is used) then an unique identifier with a negative value will be
        generated.
        """
        return _core_.Window_GetId(*args, **kwargs)

    def NewControlId(*args, **kwargs):
        """
        NewControlId() -> int

        Generate a control id for the controls which were not given one.
        """
        return _core_.Window_NewControlId(*args, **kwargs)

    NewControlId = staticmethod(NewControlId)
    def NextControlId(*args, **kwargs):
        """
        NextControlId(int winid) -> int

        Get the id of the control following the one with the given
        autogenerated) id
        """
        return _core_.Window_NextControlId(*args, **kwargs)

    NextControlId = staticmethod(NextControlId)
    def PrevControlId(*args, **kwargs):
        """
        PrevControlId(int winid) -> int

        Get the id of the control preceding the one with the given
        autogenerated) id
        """
        return _core_.Window_PrevControlId(*args, **kwargs)

    PrevControlId = staticmethod(PrevControlId)
    def SetSize(*args, **kwargs):
        """
        SetSize(self, Size size)

        Sets the size of the window in pixels.
        """
        return _core_.Window_SetSize(*args, **kwargs)

    def SetDimensions(*args, **kwargs):
        """
        SetDimensions(self, int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)

        Sets the position and size of the window in pixels.  The sizeFlags
        parameter indicates the interpretation of the other params if they are
        -1.  wx.SIZE_AUTO*: a -1 indicates that a class-specific default
        shoudl be used.  wx.SIZE_USE_EXISTING: existing dimensions should be
        used if -1 values are supplied.  wxSIZE_ALLOW_MINUS_ONE: allow
        dimensions of -1 and less to be interpreted as real dimensions, not
        default values.
        """
        return _core_.Window_SetDimensions(*args, **kwargs)

    def SetRect(*args, **kwargs):
        """
        SetRect(self, Rect rect, int sizeFlags=SIZE_AUTO)

        Sets the position and size of the window in pixels using a wx.Rect.
        """
        return _core_.Window_SetRect(*args, **kwargs)

    def SetSizeWH(*args, **kwargs):
        """
        SetSizeWH(self, int width, int height)

        Sets the size of the window in pixels.
        """
        return _core_.Window_SetSizeWH(*args, **kwargs)

    def Move(*args, **kwargs):
        """
        Move(self, Point pt, int flags=SIZE_USE_EXISTING)

        Moves the window to the given position.
        """
        return _core_.Window_Move(*args, **kwargs)

    SetPosition = Move 
    def MoveXY(*args, **kwargs):
        """
        MoveXY(self, int x, int y, int flags=SIZE_USE_EXISTING)

        Moves the window to the given position.
        """
        return _core_.Window_MoveXY(*args, **kwargs)

    def SetBestFittingSize(*args, **kwargs):
        """
        SetBestFittingSize(self, Size size=DefaultSize)

        A 'Smart' SetSize that will fill in default size components with the
        window's *best size* values.  Also set's the minsize for use with sizers.
        """
        return _core_.Window_SetBestFittingSize(*args, **kwargs)

    def Raise(*args, **kwargs):
        """
        Raise(self)

        Raises the window to the top of the window hierarchy if it is a
        managed window (dialog or frame).
        """
        return _core_.Window_Raise(*args, **kwargs)

    def Lower(*args, **kwargs):
        """
        Lower(self)

        Lowers the window to the bottom of the window hierarchy if it is a
        managed window (dialog or frame).
        """
        return _core_.Window_Lower(*args, **kwargs)

    def SetClientSize(*args, **kwargs):
        """
        SetClientSize(self, Size size)

        This sets the size of the window client area in pixels. Using this
        function to size a window tends to be more device-independent than
        wx.Window.SetSize, since the application need not worry about what
        dimensions the border or title bar have when trying to fit the window
        around panel items, for example.
        """
        return _core_.Window_SetClientSize(*args, **kwargs)

    def SetClientSizeWH(*args, **kwargs):
        """
        SetClientSizeWH(self, int width, int height)

        This sets the size of the window client area in pixels. Using this
        function to size a window tends to be more device-independent than
        wx.Window.SetSize, since the application need not worry about what
        dimensions the border or title bar have when trying to fit the window
        around panel items, for example.
        """
        return _core_.Window_SetClientSizeWH(*args, **kwargs)

    def SetClientRect(*args, **kwargs):
        """
        SetClientRect(self, Rect rect)

        This sets the size of the window client area in pixels. Using this
        function to size a window tends to be more device-independent than
        wx.Window.SetSize, since the application need not worry about what
        dimensions the border or title bar have when trying to fit the window
        around panel items, for example.
        """
        return _core_.Window_SetClientRect(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Get the window's position.
        """
        return _core_.Window_GetPosition(*args, **kwargs)

    def GetPositionTuple(*args, **kwargs):
        """
        GetPositionTuple() -> (x,y)

        Get the window's position.
        """
        return _core_.Window_GetPositionTuple(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> Size

        Get the window size.
        """
        return _core_.Window_GetSize(*args, **kwargs)

    def GetSizeTuple(*args, **kwargs):
        """
        GetSizeTuple() -> (width, height)

        Get the window size.
        """
        return _core_.Window_GetSizeTuple(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """
        GetRect(self) -> Rect

        Returns the size and position of the window as a wx.Rect object.
        """
        return _core_.Window_GetRect(*args, **kwargs)

    def GetClientSize(*args, **kwargs):
        """
        GetClientSize(self) -> Size

        This gets the size of the window's 'client area' in pixels. The client
        area is the area which may be drawn on by the programmer, excluding
        title bar, border, scrollbars, etc.
        """
        return _core_.Window_GetClientSize(*args, **kwargs)

    def GetClientSizeTuple(*args, **kwargs):
        """
        GetClientSizeTuple() -> (width, height)

        This gets the size of the window's 'client area' in pixels. The client
        area is the area which may be drawn on by the programmer, excluding
        title bar, border, scrollbars, etc.
        """
        return _core_.Window_GetClientSizeTuple(*args, **kwargs)

    def GetClientAreaOrigin(*args, **kwargs):
        """
        GetClientAreaOrigin(self) -> Point

        Get the origin of the client area of the window relative to the
        window's top left corner (the client area may be shifted because of
        the borders, scrollbars, other decorations...)
        """
        return _core_.Window_GetClientAreaOrigin(*args, **kwargs)

    def GetClientRect(*args, **kwargs):
        """
        GetClientRect(self) -> Rect

        Get the client area position and size as a `wx.Rect` object.
        """
        return _core_.Window_GetClientRect(*args, **kwargs)

    def GetBestSize(*args, **kwargs):
        """
        GetBestSize(self) -> Size

        This function returns the best acceptable minimal size for the
        window, if applicable. For example, for a static text control, it will
        be the minimal size such that the control label is not truncated. For
        windows containing subwindows (suzh aswx.Panel), the size returned by
        this function will be the same as the size the window would have had
        after calling Fit.
        """
        return _core_.Window_GetBestSize(*args, **kwargs)

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
        return _core_.Window_GetBestSizeTuple(*args, **kwargs)

    def InvalidateBestSize(*args, **kwargs):
        """
        InvalidateBestSize(self)

        Reset the cached best size value so it will be recalculated the next
        time it is needed.
        """
        return _core_.Window_InvalidateBestSize(*args, **kwargs)

    def GetBestFittingSize(*args, **kwargs):
        """
        GetBestFittingSize(self) -> Size

        This function will merge the window's best size into the window's
        minimum size, giving priority to the min size components, and returns
        the results.

        """
        return _core_.Window_GetBestFittingSize(*args, **kwargs)

    def GetAdjustedBestSize(*args, **kwargs):
        """
        GetAdjustedBestSize(self) -> Size

        This method is similar to GetBestSize, except in one
        thing. GetBestSize should return the minimum untruncated size of the
        window, while this method will return the largest of BestSize and any
        user specified minimum size. ie. it is the minimum size the window
        should currently be drawn at, not the minimal size it can possibly
        tolerate.
        """
        return _core_.Window_GetAdjustedBestSize(*args, **kwargs)

    def Center(*args, **kwargs):
        """
        Center(self, int direction=BOTH)

        Centers the window.  The parameter specifies the direction for
        cetering, and may be wx.HORIZONTAL, wx.VERTICAL or wx.BOTH. It may
        also include wx.CENTER_ON_SCREEN flag if you want to center the window
        on the entire screen and not on its parent window.  If it is a
        top-level window and has no parent then it will always be centered
        relative to the screen.
        """
        return _core_.Window_Center(*args, **kwargs)

    Centre = Center 
    def CenterOnScreen(*args, **kwargs):
        """
        CenterOnScreen(self, int dir=BOTH)

        Center on screen (only works for top level windows)
        """
        return _core_.Window_CenterOnScreen(*args, **kwargs)

    CentreOnScreen = CenterOnScreen 
    def CenterOnParent(*args, **kwargs):
        """
        CenterOnParent(self, int dir=BOTH)

        Center with respect to the the parent window
        """
        return _core_.Window_CenterOnParent(*args, **kwargs)

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
        return _core_.Window_Fit(*args, **kwargs)

    def FitInside(*args, **kwargs):
        """
        FitInside(self)

        Similar to Fit, but sizes the interior (virtual) size of a
        window. Mainly useful with scrolled windows to reset scrollbars after
        sizing changes that do not trigger a size event, and/or scrolled
        windows without an interior sizer. This function similarly won't do
        anything if there are no subwindows.
        """
        return _core_.Window_FitInside(*args, **kwargs)

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
        return _core_.Window_SetSizeHints(*args, **kwargs)

    def SetSizeHintsSz(*args, **kwargs):
        """
        SetSizeHintsSz(self, Size minSize, Size maxSize=DefaultSize, Size incSize=DefaultSize)

        Allows specification of minimum and maximum window sizes, and window
        size increments. If a pair of values is not set (or set to -1), the
        default values will be used.  If this function is called, the user
        will not be able to size the window outside the given bounds (if it is
        a top-level window.)  Sizers will also inspect the minimum window size
        and will use that value if set when calculating layout.

        The resizing increments are only significant under Motif or Xt.
        """
        return _core_.Window_SetSizeHintsSz(*args, **kwargs)

    def SetVirtualSizeHints(*args, **kwargs):
        """
        SetVirtualSizeHints(self, int minW, int minH, int maxW=-1, int maxH=-1)

        Allows specification of minimum and maximum virtual window sizes. If a
        pair of values is not set (or set to -1), the default values will be
        used.  If this function is called, the user will not be able to size
        the virtual area of the window outside the given bounds.
        """
        return _core_.Window_SetVirtualSizeHints(*args, **kwargs)

    def SetVirtualSizeHintsSz(*args, **kwargs):
        """
        SetVirtualSizeHintsSz(self, Size minSize, Size maxSize=DefaultSize)

        Allows specification of minimum and maximum virtual window sizes. If a
        pair of values is not set (or set to -1), the default values will be
        used.  If this function is called, the user will not be able to size
        the virtual area of the window outside the given bounds.
        """
        return _core_.Window_SetVirtualSizeHintsSz(*args, **kwargs)

    def GetMaxSize(*args, **kwargs):
        """GetMaxSize(self) -> Size"""
        return _core_.Window_GetMaxSize(*args, **kwargs)

    def GetMinSize(*args, **kwargs):
        """GetMinSize(self) -> Size"""
        return _core_.Window_GetMinSize(*args, **kwargs)

    def SetMinSize(*args, **kwargs):
        """
        SetMinSize(self, Size minSize)

        A more convenient method than `SetSizeHints` for setting just the
        min size.
        """
        return _core_.Window_SetMinSize(*args, **kwargs)

    def SetMaxSize(*args, **kwargs):
        """
        SetMaxSize(self, Size maxSize)

        A more convenient method than `SetSizeHints` for setting just the
        max size.
        """
        return _core_.Window_SetMaxSize(*args, **kwargs)

    def GetMinWidth(*args, **kwargs):
        """GetMinWidth(self) -> int"""
        return _core_.Window_GetMinWidth(*args, **kwargs)

    def GetMinHeight(*args, **kwargs):
        """GetMinHeight(self) -> int"""
        return _core_.Window_GetMinHeight(*args, **kwargs)

    def GetMaxWidth(*args, **kwargs):
        """GetMaxWidth(self) -> int"""
        return _core_.Window_GetMaxWidth(*args, **kwargs)

    def GetMaxHeight(*args, **kwargs):
        """GetMaxHeight(self) -> int"""
        return _core_.Window_GetMaxHeight(*args, **kwargs)

    def SetVirtualSize(*args, **kwargs):
        """
        SetVirtualSize(self, Size size)

        Set the the virtual size of a window in pixels.  For most windows this
        is just the client area of the window, but for some like scrolled
        windows it is more or less independent of the screen window size.
        """
        return _core_.Window_SetVirtualSize(*args, **kwargs)

    def SetVirtualSizeWH(*args, **kwargs):
        """
        SetVirtualSizeWH(self, int w, int h)

        Set the the virtual size of a window in pixels.  For most windows this
        is just the client area of the window, but for some like scrolled
        windows it is more or less independent of the screen window size.
        """
        return _core_.Window_SetVirtualSizeWH(*args, **kwargs)

    def GetVirtualSize(*args, **kwargs):
        """
        GetVirtualSize(self) -> Size

        Get the the virtual size of the window in pixels.  For most windows
        this is just the client area of the window, but for some like scrolled
        windows it is more or less independent of the screen window size.
        """
        return _core_.Window_GetVirtualSize(*args, **kwargs)

    def GetVirtualSizeTuple(*args, **kwargs):
        """
        GetVirtualSizeTuple() -> (width, height)

        Get the the virtual size of the window in pixels.  For most windows
        this is just the client area of the window, but for some like scrolled
        windows it is more or less independent of the screen window size.
        """
        return _core_.Window_GetVirtualSizeTuple(*args, **kwargs)

    def GetBestVirtualSize(*args, **kwargs):
        """
        GetBestVirtualSize(self) -> Size

        Return the largest of ClientSize and BestSize (as determined by a
        sizer, interior children, or other means)
        """
        return _core_.Window_GetBestVirtualSize(*args, **kwargs)

    def Show(*args, **kwargs):
        """
        Show(self, bool show=True) -> bool

        Shows or hides the window. You may need to call Raise for a top level
        window if you want to bring it to top, although this is not needed if
        Show is called immediately after the frame creation.  Returns True if
        the window has been shown or hidden or False if nothing was done
        because it already was in the requested state.
        """
        return _core_.Window_Show(*args, **kwargs)

    def Hide(*args, **kwargs):
        """
        Hide(self) -> bool

        Equivalent to calling Show(False).
        """
        return _core_.Window_Hide(*args, **kwargs)

    def Enable(*args, **kwargs):
        """
        Enable(self, bool enable=True) -> bool

        Enable or disable the window for user input. Note that when a parent
        window is disabled, all of its children are disabled as well and they
        are reenabled again when the parent is.  Returns true if the window
        has been enabled or disabled, false if nothing was done, i.e. if the
        window had already been in the specified state.
        """
        return _core_.Window_Enable(*args, **kwargs)

    def Disable(*args, **kwargs):
        """
        Disable(self) -> bool

        Disables the window, same as Enable(false).
        """
        return _core_.Window_Disable(*args, **kwargs)

    def IsShown(*args, **kwargs):
        """
        IsShown(self) -> bool

        Returns true if the window is shown, false if it has been hidden.
        """
        return _core_.Window_IsShown(*args, **kwargs)

    def IsEnabled(*args, **kwargs):
        """
        IsEnabled(self) -> bool

        Returns true if the window is enabled for input, false otherwise.
        """
        return _core_.Window_IsEnabled(*args, **kwargs)

    def SetWindowStyleFlag(*args, **kwargs):
        """
        SetWindowStyleFlag(self, long style)

        Sets the style of the window. Please note that some styles cannot be
        changed after the window creation and that Refresh() might need to be
        called after changing the others for the change to take place
        immediately.
        """
        return _core_.Window_SetWindowStyleFlag(*args, **kwargs)

    def GetWindowStyleFlag(*args, **kwargs):
        """
        GetWindowStyleFlag(self) -> long

        Gets the window style that was passed to the constructor or Create
        method.
        """
        return _core_.Window_GetWindowStyleFlag(*args, **kwargs)

    SetWindowStyle = SetWindowStyleFlag; GetWindowStyle = GetWindowStyleFlag 
    def HasFlag(*args, **kwargs):
        """
        HasFlag(self, int flag) -> bool

        Test if the given style is set for this window.
        """
        return _core_.Window_HasFlag(*args, **kwargs)

    def IsRetained(*args, **kwargs):
        """
        IsRetained(self) -> bool

        Returns true if the window is retained, false otherwise.  Retained
        windows are only available on X platforms.
        """
        return _core_.Window_IsRetained(*args, **kwargs)

    def SetExtraStyle(*args, **kwargs):
        """
        SetExtraStyle(self, long exStyle)

        Sets the extra style bits for the window.  Extra styles are the less
        often used style bits which can't be set with the constructor or with
        SetWindowStyleFlag()
        """
        return _core_.Window_SetExtraStyle(*args, **kwargs)

    def GetExtraStyle(*args, **kwargs):
        """
        GetExtraStyle(self) -> long

        Returns the extra style bits for the window.
        """
        return _core_.Window_GetExtraStyle(*args, **kwargs)

    def MakeModal(*args, **kwargs):
        """
        MakeModal(self, bool modal=True)

        Disables all other windows in the application so that the user can
        only interact with this window.  Passing False will reverse this
        effect.
        """
        return _core_.Window_MakeModal(*args, **kwargs)

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
        return _core_.Window_SetThemeEnabled(*args, **kwargs)

    def GetThemeEnabled(*args, **kwargs):
        """
        GetThemeEnabled(self) -> bool

        Return the themeEnabled flag.
        """
        return _core_.Window_GetThemeEnabled(*args, **kwargs)

    def SetFocus(*args, **kwargs):
        """
        SetFocus(self)

        Set's the focus to this window, allowing it to receive keyboard input.
        """
        return _core_.Window_SetFocus(*args, **kwargs)

    def SetFocusFromKbd(*args, **kwargs):
        """
        SetFocusFromKbd(self)

        Set focus to this window as the result of a keyboard action.  Normally
        only called internally.
        """
        return _core_.Window_SetFocusFromKbd(*args, **kwargs)

    def FindFocus(*args, **kwargs):
        """
        FindFocus() -> Window

        Returns the window or control that currently has the keyboard focus,
        or None.
        """
        return _core_.Window_FindFocus(*args, **kwargs)

    FindFocus = staticmethod(FindFocus)
    def AcceptsFocus(*args, **kwargs):
        """
        AcceptsFocus(self) -> bool

        Can this window have focus?
        """
        return _core_.Window_AcceptsFocus(*args, **kwargs)

    def AcceptsFocusFromKeyboard(*args, **kwargs):
        """
        AcceptsFocusFromKeyboard(self) -> bool

        Can this window be given focus by keyboard navigation? if not, the
        only way to give it focus (provided it accepts it at all) is to click
        it.
        """
        return _core_.Window_AcceptsFocusFromKeyboard(*args, **kwargs)

    def GetDefaultItem(*args, **kwargs):
        """
        GetDefaultItem(self) -> Window

        Get the default child of this parent, i.e. the one which is activated
        by pressing <Enter> such as the OK button on a wx.Dialog.
        """
        return _core_.Window_GetDefaultItem(*args, **kwargs)

    def SetDefaultItem(*args, **kwargs):
        """
        SetDefaultItem(self, Window child) -> Window

        Set this child as default, return the old default.
        """
        return _core_.Window_SetDefaultItem(*args, **kwargs)

    def SetTmpDefaultItem(*args, **kwargs):
        """
        SetTmpDefaultItem(self, Window win)

        Set this child as temporary default
        """
        return _core_.Window_SetTmpDefaultItem(*args, **kwargs)

    def Navigate(*args, **kwargs):
        """
        Navigate(self, int flags=NavigationKeyEvent.IsForward) -> bool

        Does keyboard navigation from this window to another, by sending a
        `wx.NavigationKeyEvent`.
        """
        return _core_.Window_Navigate(*args, **kwargs)

    def MoveAfterInTabOrder(*args, **kwargs):
        """
        MoveAfterInTabOrder(self, Window win)

        Moves this window in the tab navigation order after the specified
        sibling window.  This means that when the user presses the TAB key on
        that other window, the focus switches to this window.

        The default tab order is the same as creation order.  This function
        and `MoveBeforeInTabOrder` allow to change it after creating all the
        windows.

        """
        return _core_.Window_MoveAfterInTabOrder(*args, **kwargs)

    def MoveBeforeInTabOrder(*args, **kwargs):
        """
        MoveBeforeInTabOrder(self, Window win)

        Same as `MoveAfterInTabOrder` except that it inserts this window just
        before win instead of putting it right after it.
        """
        return _core_.Window_MoveBeforeInTabOrder(*args, **kwargs)

    def GetChildren(*args, **kwargs):
        """
        GetChildren(self) -> PyObject

        Returns a list of the window's children.  NOTE: Currently this is a
        copy of the child window list maintained by the window, so the return
        value of this function is only valid as long as the window's children
        do not change.
        """
        return _core_.Window_GetChildren(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """
        GetParent(self) -> Window

        Returns the parent window of this window, or None if there isn't one.
        """
        return _core_.Window_GetParent(*args, **kwargs)

    def GetGrandParent(*args, **kwargs):
        """
        GetGrandParent(self) -> Window

        Returns the parent of the parent of this window, or None if there
        isn't one.
        """
        return _core_.Window_GetGrandParent(*args, **kwargs)

    def IsTopLevel(*args, **kwargs):
        """
        IsTopLevel(self) -> bool

        Returns true if the given window is a top-level one. Currently all
        frames and dialogs are always considered to be top-level windows (even
        if they have a parent window).
        """
        return _core_.Window_IsTopLevel(*args, **kwargs)

    def Reparent(*args, **kwargs):
        """
        Reparent(self, Window newParent) -> bool

        Reparents the window, i.e the window will be removed from its current
        parent window (e.g. a non-standard toolbar in a wxFrame) and then
        re-inserted into another. Available on Windows and GTK.  Returns True
        if the parent was changed, False otherwise (error or newParent ==
        oldParent)
        """
        return _core_.Window_Reparent(*args, **kwargs)

    def AddChild(*args, **kwargs):
        """
        AddChild(self, Window child)

        Adds a child window. This is called automatically by window creation
        functions so should not be required by the application programmer.
        """
        return _core_.Window_AddChild(*args, **kwargs)

    def RemoveChild(*args, **kwargs):
        """
        RemoveChild(self, Window child)

        Removes a child window. This is called automatically by window
        deletion functions so should not be required by the application
        programmer.
        """
        return _core_.Window_RemoveChild(*args, **kwargs)

    def FindWindowById(*args, **kwargs):
        """
        FindWindowById(self, long winid) -> Window

        Find a chld of this window by window ID
        """
        return _core_.Window_FindWindowById(*args, **kwargs)

    def FindWindowByName(*args, **kwargs):
        """
        FindWindowByName(self, String name) -> Window

        Find a child of this window by name
        """
        return _core_.Window_FindWindowByName(*args, **kwargs)

    def GetEventHandler(*args, **kwargs):
        """
        GetEventHandler(self) -> EvtHandler

        Returns the event handler for this window. By default, the window is
        its own event handler.
        """
        return _core_.Window_GetEventHandler(*args, **kwargs)

    def SetEventHandler(*args, **kwargs):
        """
        SetEventHandler(self, EvtHandler handler)

        Sets the event handler for this window.  An event handler is an object
        that is capable of processing the events sent to a window. By default,
        the window is its own event handler, but an application may wish to
        substitute another, for example to allow central implementation of
        event-handling for a variety of different window classes.

        It is usually better to use `wx.Window.PushEventHandler` since this sets
        up a chain of event handlers, where an event not handled by one event
        handler is handed to the next one in the chain.
        """
        return _core_.Window_SetEventHandler(*args, **kwargs)

    def PushEventHandler(*args, **kwargs):
        """
        PushEventHandler(self, EvtHandler handler)

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
        return _core_.Window_PushEventHandler(*args, **kwargs)

    def PopEventHandler(*args, **kwargs):
        """
        PopEventHandler(self, bool deleteHandler=False) -> EvtHandler

        Removes and returns the top-most event handler on the event handler
        stack.  If deleteHandler is True then the wx.EvtHandler object will be
        destroyed after it is popped.
        """
        return _core_.Window_PopEventHandler(*args, **kwargs)

    def RemoveEventHandler(*args, **kwargs):
        """
        RemoveEventHandler(self, EvtHandler handler) -> bool

        Find the given handler in the event handler chain and remove (but not
        delete) it from the event handler chain, return True if it was found
        and False otherwise (this also results in an assert failure so this
        function should only be called when the handler is supposed to be
        there.)
        """
        return _core_.Window_RemoveEventHandler(*args, **kwargs)

    def SetValidator(*args, **kwargs):
        """
        SetValidator(self, Validator validator)

        Deletes the current validator (if any) and sets the window validator,
        having called wx.Validator.Clone to create a new validator of this
        type.
        """
        return _core_.Window_SetValidator(*args, **kwargs)

    def GetValidator(*args, **kwargs):
        """
        GetValidator(self) -> Validator

        Returns a pointer to the current validator for the window, or None if
        there is none.
        """
        return _core_.Window_GetValidator(*args, **kwargs)

    def Validate(*args, **kwargs):
        """
        Validate(self) -> bool

        Validates the current values of the child controls using their
        validators.  If the window has wx.WS_EX_VALIDATE_RECURSIVELY extra
        style flag set, the method will also call Validate() of all child
        windows.  Returns false if any of the validations failed.
        """
        return _core_.Window_Validate(*args, **kwargs)

    def TransferDataToWindow(*args, **kwargs):
        """
        TransferDataToWindow(self) -> bool

        Transfers values to child controls from data areas specified by their
        validators.  If the window has wx.WS_EX_VALIDATE_RECURSIVELY extra
        style flag set, the method will also call TransferDataToWindow() of
        all child windows.
        """
        return _core_.Window_TransferDataToWindow(*args, **kwargs)

    def TransferDataFromWindow(*args, **kwargs):
        """
        TransferDataFromWindow(self) -> bool

        Transfers values from child controls to data areas specified by their
        validators. Returns false if a transfer failed.  If the window has
        wx.WS_EX_VALIDATE_RECURSIVELY extra style flag set, the method will
        also call TransferDataFromWindow() of all child windows.
        """
        return _core_.Window_TransferDataFromWindow(*args, **kwargs)

    def InitDialog(*args, **kwargs):
        """
        InitDialog(self)

        Sends an EVT_INIT_DIALOG event, whose handler usually transfers data
        to the dialog via validators.
        """
        return _core_.Window_InitDialog(*args, **kwargs)

    def SetAcceleratorTable(*args, **kwargs):
        """
        SetAcceleratorTable(self, AcceleratorTable accel)

        Sets the accelerator table for this window.
        """
        return _core_.Window_SetAcceleratorTable(*args, **kwargs)

    def GetAcceleratorTable(*args, **kwargs):
        """
        GetAcceleratorTable(self) -> AcceleratorTable

        Gets the accelerator table for this window.
        """
        return _core_.Window_GetAcceleratorTable(*args, **kwargs)

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
        return _core_.Window_RegisterHotKey(*args, **kwargs)

    def UnregisterHotKey(*args, **kwargs):
        """
        UnregisterHotKey(self, int hotkeyId) -> bool

        Unregisters a system wide hotkey.
        """
        return _core_.Window_UnregisterHotKey(*args, **kwargs)

    def ConvertDialogPointToPixels(*args, **kwargs):
        """
        ConvertDialogPointToPixels(self, Point pt) -> Point

        Converts a point or size from dialog units to pixels.  Dialog units
        are used for maintaining a dialog's proportions even if the font
        changes. For the x dimension, the dialog units are multiplied by the
        average character width and then divided by 4. For the y dimension,
        the dialog units are multiplied by the average character height and
        then divided by 8.
        """
        return _core_.Window_ConvertDialogPointToPixels(*args, **kwargs)

    def ConvertDialogSizeToPixels(*args, **kwargs):
        """
        ConvertDialogSizeToPixels(self, Size sz) -> Size

        Converts a point or size from dialog units to pixels.  Dialog units
        are used for maintaining a dialog's proportions even if the font
        changes. For the x dimension, the dialog units are multiplied by the
        average character width and then divided by 4. For the y dimension,
        the dialog units are multiplied by the average character height and
        then divided by 8.
        """
        return _core_.Window_ConvertDialogSizeToPixels(*args, **kwargs)

    def DLG_PNT(*args, **kwargs):
        """
        DLG_PNT(self, Point pt) -> Point

        Converts a point or size from dialog units to pixels.  Dialog units
        are used for maintaining a dialog's proportions even if the font
        changes. For the x dimension, the dialog units are multiplied by the
        average character width and then divided by 4. For the y dimension,
        the dialog units are multiplied by the average character height and
        then divided by 8.
        """
        return _core_.Window_DLG_PNT(*args, **kwargs)

    def DLG_SZE(*args, **kwargs):
        """
        DLG_SZE(self, Size sz) -> Size

        Converts a point or size from dialog units to pixels.  Dialog units
        are used for maintaining a dialog's proportions even if the font
        changes. For the x dimension, the dialog units are multiplied by the
        average character width and then divided by 4. For the y dimension,
        the dialog units are multiplied by the average character height and
        then divided by 8.
        """
        return _core_.Window_DLG_SZE(*args, **kwargs)

    def ConvertPixelPointToDialog(*args, **kwargs):
        """ConvertPixelPointToDialog(self, Point pt) -> Point"""
        return _core_.Window_ConvertPixelPointToDialog(*args, **kwargs)

    def ConvertPixelSizeToDialog(*args, **kwargs):
        """ConvertPixelSizeToDialog(self, Size sz) -> Size"""
        return _core_.Window_ConvertPixelSizeToDialog(*args, **kwargs)

    def WarpPointer(*args, **kwargs):
        """
        WarpPointer(self, int x, int y)

        Moves the pointer to the given position on the window.

        NOTE: This function is not supported under Mac because Apple Human
        Interface Guidelines forbid moving the mouse cursor programmatically.
        """
        return _core_.Window_WarpPointer(*args, **kwargs)

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
        return _core_.Window_CaptureMouse(*args, **kwargs)

    def ReleaseMouse(*args, **kwargs):
        """
        ReleaseMouse(self)

        Releases mouse input captured with wx.Window.CaptureMouse.
        """
        return _core_.Window_ReleaseMouse(*args, **kwargs)

    def GetCapture(*args, **kwargs):
        """
        GetCapture() -> Window

        Returns the window which currently captures the mouse or None
        """
        return _core_.Window_GetCapture(*args, **kwargs)

    GetCapture = staticmethod(GetCapture)
    def HasCapture(*args, **kwargs):
        """
        HasCapture(self) -> bool

        Returns true if this window has the current mouse capture.
        """
        return _core_.Window_HasCapture(*args, **kwargs)

    def Refresh(*args, **kwargs):
        """
        Refresh(self, bool eraseBackground=True, Rect rect=None)

        Mark the specified rectangle (or the whole window) as "dirty" so it
        will be repainted.  Causes an EVT_PAINT event to be generated and sent
        to the window.
        """
        return _core_.Window_Refresh(*args, **kwargs)

    def RefreshRect(*args, **kwargs):
        """
        RefreshRect(self, Rect rect)

        Redraws the contents of the given rectangle: the area inside it will
        be repainted.  This is the same as Refresh but has a nicer syntax.
        """
        return _core_.Window_RefreshRect(*args, **kwargs)

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
        return _core_.Window_Update(*args, **kwargs)

    def ClearBackground(*args, **kwargs):
        """
        ClearBackground(self)

        Clears the window by filling it with the current background
        colour. Does not cause an erase background event to be generated.
        """
        return _core_.Window_ClearBackground(*args, **kwargs)

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
        return _core_.Window_Freeze(*args, **kwargs)

    def Thaw(*args, **kwargs):
        """
        Thaw(self)

        Reenables window updating after a previous call to Freeze.  Calls to
        Freeze/Thaw may be nested, so Thaw must be called the same number of
        times that Freeze was before the window will be updated.
        """
        return _core_.Window_Thaw(*args, **kwargs)

    def PrepareDC(*args, **kwargs):
        """
        PrepareDC(self, DC dc)

        Call this function to prepare the device context for drawing a
        scrolled image. It sets the device origin according to the current
        scroll position.
        """
        return _core_.Window_PrepareDC(*args, **kwargs)

    def GetUpdateRegion(*args, **kwargs):
        """
        GetUpdateRegion(self) -> Region

        Returns the region specifying which parts of the window have been
        damaged. Should only be called within an EVT_PAINT handler.
        """
        return _core_.Window_GetUpdateRegion(*args, **kwargs)

    def GetUpdateClientRect(*args, **kwargs):
        """
        GetUpdateClientRect(self) -> Rect

        Get the update rectangle region bounding box in client coords.
        """
        return _core_.Window_GetUpdateClientRect(*args, **kwargs)

    def IsExposed(*args, **kwargs):
        """
        IsExposed(self, int x, int y, int w=1, int h=1) -> bool

        Returns true if the given point or rectangle area has been exposed
        since the last repaint. Call this in an paint event handler to
        optimize redrawing by only redrawing those areas, which have been
        exposed.
        """
        return _core_.Window_IsExposed(*args, **kwargs)

    def IsExposedPoint(*args, **kwargs):
        """
        IsExposedPoint(self, Point pt) -> bool

        Returns true if the given point or rectangle area has been exposed
        since the last repaint. Call this in an paint event handler to
        optimize redrawing by only redrawing those areas, which have been
        exposed.
        """
        return _core_.Window_IsExposedPoint(*args, **kwargs)

    def IsExposedRect(*args, **kwargs):
        """
        IsExposedRect(self, Rect rect) -> bool

        Returns true if the given point or rectangle area has been exposed
        since the last repaint. Call this in an paint event handler to
        optimize redrawing by only redrawing those areas, which have been
        exposed.
        """
        return _core_.Window_IsExposedRect(*args, **kwargs)

    def GetDefaultAttributes(*args, **kwargs):
        """
        GetDefaultAttributes(self) -> VisualAttributes

        Get the default attributes for an instance of this class.  This is
        useful if you want to use the same font or colour in your own control
        as in a standard control -- which is a much better idea than hard
        coding specific colours or fonts which might look completely out of
        place on the user's system, especially if it uses themes.
        """
        return _core_.Window_GetDefaultAttributes(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
        return _core_.Window_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    def SetBackgroundColour(*args, **kwargs):
        """
        SetBackgroundColour(self, Colour colour) -> bool

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
        return _core_.Window_SetBackgroundColour(*args, **kwargs)

    def SetOwnBackgroundColour(*args, **kwargs):
        """SetOwnBackgroundColour(self, Colour colour)"""
        return _core_.Window_SetOwnBackgroundColour(*args, **kwargs)

    def SetForegroundColour(*args, **kwargs):
        """
        SetForegroundColour(self, Colour colour) -> bool

        Sets the foreground colour of the window.  Returns True is the colour
        was changed.  The interpretation of foreground colour is dependent on
        the window class; it may be the text colour or other colour, or it may
        not be used at all.
        """
        return _core_.Window_SetForegroundColour(*args, **kwargs)

    def SetOwnForegroundColour(*args, **kwargs):
        """SetOwnForegroundColour(self, Colour colour)"""
        return _core_.Window_SetOwnForegroundColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """
        GetBackgroundColour(self) -> Colour

        Returns the background colour of the window.
        """
        return _core_.Window_GetBackgroundColour(*args, **kwargs)

    def GetForegroundColour(*args, **kwargs):
        """
        GetForegroundColour(self) -> Colour

        Returns the foreground colour of the window.  The interpretation of
        foreground colour is dependent on the window class; it may be the text
        colour or other colour, or it may not be used at all.
        """
        return _core_.Window_GetForegroundColour(*args, **kwargs)

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
        return _core_.Window_SetBackgroundStyle(*args, **kwargs)

    def GetBackgroundStyle(*args, **kwargs):
        """
        GetBackgroundStyle(self) -> int

        Returns the background style of the window.

        :see: `SetBackgroundStyle`
        """
        return _core_.Window_GetBackgroundStyle(*args, **kwargs)

    def SetCursor(*args, **kwargs):
        """
        SetCursor(self, Cursor cursor) -> bool

        Sets the window's cursor. Notice that the window cursor also sets it
        for the children of the window implicitly.

        The cursor may be wx.NullCursor in which case the window cursor will
        be reset back to default.
        """
        return _core_.Window_SetCursor(*args, **kwargs)

    def GetCursor(*args, **kwargs):
        """
        GetCursor(self) -> Cursor

        Return the cursor associated with this window.
        """
        return _core_.Window_GetCursor(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """
        SetFont(self, Font font) -> bool

        Sets the font for this window.
        """
        return _core_.Window_SetFont(*args, **kwargs)

    def SetOwnFont(*args, **kwargs):
        """SetOwnFont(self, Font font)"""
        return _core_.Window_SetOwnFont(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """
        GetFont(self) -> Font

        Returns the default font used for this window.
        """
        return _core_.Window_GetFont(*args, **kwargs)

    def SetCaret(*args, **kwargs):
        """
        SetCaret(self, Caret caret)

        Sets the caret associated with the window.
        """
        return _core_.Window_SetCaret(*args, **kwargs)

    def GetCaret(*args, **kwargs):
        """
        GetCaret(self) -> Caret

        Returns the caret associated with the window.
        """
        return _core_.Window_GetCaret(*args, **kwargs)

    def GetCharHeight(*args, **kwargs):
        """
        GetCharHeight(self) -> int

        Get the (average) character size for the current font.
        """
        return _core_.Window_GetCharHeight(*args, **kwargs)

    def GetCharWidth(*args, **kwargs):
        """
        GetCharWidth(self) -> int

        Get the (average) character size for the current font.
        """
        return _core_.Window_GetCharWidth(*args, **kwargs)

    def GetTextExtent(*args, **kwargs):
        """
        GetTextExtent(String string) -> (width, height)

        Get the width and height of the text using the current font.
        """
        return _core_.Window_GetTextExtent(*args, **kwargs)

    def GetFullTextExtent(*args, **kwargs):
        """
        GetFullTextExtent(String string, Font font=None) ->
           (width, height, descent, externalLeading)

        Get the width, height, decent and leading of the text using the
        current or specified font.
        """
        return _core_.Window_GetFullTextExtent(*args, **kwargs)

    def ClientToScreenXY(*args, **kwargs):
        """
        ClientToScreenXY(int x, int y) -> (x,y)

        Converts to screen coordinates from coordinates relative to this window.
        """
        return _core_.Window_ClientToScreenXY(*args, **kwargs)

    def ScreenToClientXY(*args, **kwargs):
        """
        ScreenToClientXY(int x, int y) -> (x,y)

        Converts from screen to client window coordinates.
        """
        return _core_.Window_ScreenToClientXY(*args, **kwargs)

    def ClientToScreen(*args, **kwargs):
        """
        ClientToScreen(self, Point pt) -> Point

        Converts to screen coordinates from coordinates relative to this window.
        """
        return _core_.Window_ClientToScreen(*args, **kwargs)

    def ScreenToClient(*args, **kwargs):
        """
        ScreenToClient(self, Point pt) -> Point

        Converts from screen to client window coordinates.
        """
        return _core_.Window_ScreenToClient(*args, **kwargs)

    def HitTestXY(*args, **kwargs):
        """
        HitTestXY(self, int x, int y) -> int

        Test where the given (in client coords) point lies
        """
        return _core_.Window_HitTestXY(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(self, Point pt) -> int

        Test where the given (in client coords) point lies
        """
        return _core_.Window_HitTest(*args, **kwargs)

    def GetBorder(*args):
        """
        GetBorder(self, long flags) -> int
        GetBorder(self) -> int

        Get border for the flags of this window
        """
        return _core_.Window_GetBorder(*args)

    def UpdateWindowUI(*args, **kwargs):
        """
        UpdateWindowUI(self, long flags=UPDATE_UI_NONE)

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
        return _core_.Window_UpdateWindowUI(*args, **kwargs)

    def PopupMenuXY(*args, **kwargs):
        """
        PopupMenuXY(self, Menu menu, int x=-1, int y=-1) -> bool

        Pops up the given menu at the specified coordinates, relative to this window,
        and returns control when the user has dismissed the menu. If a menu item is
        selected, the corresponding menu event is generated and will be processed as
        usual.  If the default position is given then the current position of the
        mouse cursor will be used.
        """
        return _core_.Window_PopupMenuXY(*args, **kwargs)

    def PopupMenu(*args, **kwargs):
        """
        PopupMenu(self, Menu menu, Point pos=DefaultPosition) -> bool

        Pops up the given menu at the specified coordinates, relative to this window,
        and returns control when the user has dismissed the menu. If a menu item is
        selected, the corresponding menu event is generated and will be processed as
        usual.  If the default position is given then the current position of the
        mouse cursor will be used.
        """
        return _core_.Window_PopupMenu(*args, **kwargs)

    def GetHandle(*args, **kwargs):
        """
        GetHandle(self) -> long

        Returns the platform-specific handle (as a long integer) of the
        physical window.  Currently on wxMac it returns the handle of the
        toplevel parent of the window.
        """
        return _core_.Window_GetHandle(*args, **kwargs)

    def AssociateHandle(*args, **kwargs):
        """
        AssociateHandle(self, long handle)

        Associate the window with a new native handle
        """
        return _core_.Window_AssociateHandle(*args, **kwargs)

    def DissociateHandle(*args, **kwargs):
        """
        DissociateHandle(self)

        Dissociate the current native handle from the window
        """
        return _core_.Window_DissociateHandle(*args, **kwargs)

    def HasScrollbar(*args, **kwargs):
        """
        HasScrollbar(self, int orient) -> bool

        Does the window have the scrollbar for this orientation?
        """
        return _core_.Window_HasScrollbar(*args, **kwargs)

    def SetScrollbar(*args, **kwargs):
        """
        SetScrollbar(self, int orientation, int position, int thumbSize, int range, 
            bool refresh=True)

        Sets the scrollbar properties of a built-in scrollbar.
        """
        return _core_.Window_SetScrollbar(*args, **kwargs)

    def SetScrollPos(*args, **kwargs):
        """
        SetScrollPos(self, int orientation, int pos, bool refresh=True)

        Sets the position of one of the built-in scrollbars.
        """
        return _core_.Window_SetScrollPos(*args, **kwargs)

    def GetScrollPos(*args, **kwargs):
        """
        GetScrollPos(self, int orientation) -> int

        Returns the built-in scrollbar position.
        """
        return _core_.Window_GetScrollPos(*args, **kwargs)

    def GetScrollThumb(*args, **kwargs):
        """
        GetScrollThumb(self, int orientation) -> int

        Returns the built-in scrollbar thumb size.
        """
        return _core_.Window_GetScrollThumb(*args, **kwargs)

    def GetScrollRange(*args, **kwargs):
        """
        GetScrollRange(self, int orientation) -> int

        Returns the built-in scrollbar range.
        """
        return _core_.Window_GetScrollRange(*args, **kwargs)

    def ScrollWindow(*args, **kwargs):
        """
        ScrollWindow(self, int dx, int dy, Rect rect=None)

        Physically scrolls the pixels in the window and move child windows
        accordingly.  Use this function to optimise your scrolling
        implementations, to minimise the area that must be redrawn. Note that
        it is rarely required to call this function from a user program.
        """
        return _core_.Window_ScrollWindow(*args, **kwargs)

    def ScrollLines(*args, **kwargs):
        """
        ScrollLines(self, int lines) -> bool

        If the platform and window class supports it, scrolls the window by
        the given number of lines down, if lines is positive, or up if lines
        is negative.  Returns True if the window was scrolled, False if it was
        already on top/bottom and nothing was done.
        """
        return _core_.Window_ScrollLines(*args, **kwargs)

    def ScrollPages(*args, **kwargs):
        """
        ScrollPages(self, int pages) -> bool

        If the platform and window class supports it, scrolls the window by
        the given number of pages down, if pages is positive, or up if pages
        is negative.  Returns True if the window was scrolled, False if it was
        already on top/bottom and nothing was done.
        """
        return _core_.Window_ScrollPages(*args, **kwargs)

    def LineUp(*args, **kwargs):
        """
        LineUp(self) -> bool

        This is just a wrapper for ScrollLines(-1).
        """
        return _core_.Window_LineUp(*args, **kwargs)

    def LineDown(*args, **kwargs):
        """
        LineDown(self) -> bool

        This is just a wrapper for ScrollLines(1).
        """
        return _core_.Window_LineDown(*args, **kwargs)

    def PageUp(*args, **kwargs):
        """
        PageUp(self) -> bool

        This is just a wrapper for ScrollPages(-1).
        """
        return _core_.Window_PageUp(*args, **kwargs)

    def PageDown(*args, **kwargs):
        """
        PageDown(self) -> bool

        This is just a wrapper for ScrollPages(1).
        """
        return _core_.Window_PageDown(*args, **kwargs)

    def SetHelpText(*args, **kwargs):
        """
        SetHelpText(self, String text)

        Sets the help text to be used as context-sensitive help for this
        window.  Note that the text is actually stored by the current
        wxHelpProvider implementation, and not in the window object itself.
        """
        return _core_.Window_SetHelpText(*args, **kwargs)

    def SetHelpTextForId(*args, **kwargs):
        """
        SetHelpTextForId(self, String text)

        Associate this help text with all windows with the same id as this
        one.
        """
        return _core_.Window_SetHelpTextForId(*args, **kwargs)

    def GetHelpText(*args, **kwargs):
        """
        GetHelpText(self) -> String

        Gets the help text to be used as context-sensitive help for this
        window.  Note that the text is actually stored by the current
        wxHelpProvider implementation, and not in the window object itself.
        """
        return _core_.Window_GetHelpText(*args, **kwargs)

    def SetToolTipString(*args, **kwargs):
        """
        SetToolTipString(self, String tip)

        Attach a tooltip to the window.
        """
        return _core_.Window_SetToolTipString(*args, **kwargs)

    def SetToolTip(*args, **kwargs):
        """
        SetToolTip(self, ToolTip tip)

        Attach a tooltip to the window.
        """
        return _core_.Window_SetToolTip(*args, **kwargs)

    def GetToolTip(*args, **kwargs):
        """
        GetToolTip(self) -> ToolTip

        get the associated tooltip or None if none
        """
        return _core_.Window_GetToolTip(*args, **kwargs)

    def SetDropTarget(*args, **kwargs):
        """
        SetDropTarget(self, DropTarget dropTarget)

        Associates a drop target with this window.  If the window already has
        a drop target, it is deleted.
        """
        return _core_.Window_SetDropTarget(*args, **kwargs)

    def GetDropTarget(*args, **kwargs):
        """
        GetDropTarget(self) -> DropTarget

        Returns the associated drop target, which may be None.
        """
        return _core_.Window_GetDropTarget(*args, **kwargs)

    def SetConstraints(*args, **kwargs):
        """
        SetConstraints(self, LayoutConstraints constraints)

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
        return _core_.Window_SetConstraints(*args, **kwargs)

    def GetConstraints(*args, **kwargs):
        """
        GetConstraints(self) -> LayoutConstraints

        Returns a pointer to the window's layout constraints, or None if there
        are none.
        """
        return _core_.Window_GetConstraints(*args, **kwargs)

    def SetAutoLayout(*args, **kwargs):
        """
        SetAutoLayout(self, bool autoLayout)

        Determines whether the Layout function will be called automatically
        when the window is resized.  It is called implicitly by SetSizer but
        if you use SetConstraints you should call it manually or otherwise the
        window layout won't be correctly updated when its size changes.
        """
        return _core_.Window_SetAutoLayout(*args, **kwargs)

    def GetAutoLayout(*args, **kwargs):
        """
        GetAutoLayout(self) -> bool

        Returns the current autoLayout setting
        """
        return _core_.Window_GetAutoLayout(*args, **kwargs)

    def Layout(*args, **kwargs):
        """
        Layout(self) -> bool

        Invokes the constraint-based layout algorithm or the sizer-based
        algorithm for this window.  See SetAutoLayout: when auto layout is on,
        this function gets called automatically by the default EVT_SIZE
        handler when the window is resized.
        """
        return _core_.Window_Layout(*args, **kwargs)

    def SetSizer(*args, **kwargs):
        """
        SetSizer(self, Sizer sizer, bool deleteOld=True)

        Sets the window to have the given layout sizer. The window will then
        own the object, and will take care of its deletion. If an existing
        layout sizer object is already owned by the window, it will be deleted
        if the deleteOld parameter is true. Note that this function will also
        call SetAutoLayout implicitly with a True parameter if the sizer is
        non-None, and False otherwise.
        """
        return _core_.Window_SetSizer(*args, **kwargs)

    def SetSizerAndFit(*args, **kwargs):
        """
        SetSizerAndFit(self, Sizer sizer, bool deleteOld=True)

        The same as SetSizer, except it also sets the size hints for the
        window based on the sizer's minimum size.
        """
        return _core_.Window_SetSizerAndFit(*args, **kwargs)

    def GetSizer(*args, **kwargs):
        """
        GetSizer(self) -> Sizer

        Return the sizer associated with the window by a previous call to
        SetSizer or None if there isn't one.
        """
        return _core_.Window_GetSizer(*args, **kwargs)

    def SetContainingSizer(*args, **kwargs):
        """
        SetContainingSizer(self, Sizer sizer)

        This normally does not need to be called by application code. It is
        called internally when a window is added to a sizer, and is used so
        the window can remove itself from the sizer when it is destroyed.
        """
        return _core_.Window_SetContainingSizer(*args, **kwargs)

    def GetContainingSizer(*args, **kwargs):
        """
        GetContainingSizer(self) -> Sizer

        Return the sizer that this window is a member of, if any, otherwise None.
        """
        return _core_.Window_GetContainingSizer(*args, **kwargs)

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
        return _core_.Window_InheritAttributes(*args, **kwargs)

    def ShouldInheritColours(*args, **kwargs):
        """
        ShouldInheritColours(self) -> bool

        Return true from here to allow the colours of this window to be
        changed by InheritAttributes, returning false forbids inheriting them
        from the parent window.

        The base class version returns false, but this method is overridden in
        wxControl where it returns true.
        """
        return _core_.Window_ShouldInheritColours(*args, **kwargs)

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


class WindowPtr(Window):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Window
_core_.Window_swigregister(WindowPtr)

def PreWindow(*args, **kwargs):
    """
    PreWindow() -> Window

    Precreate a Window for 2-phase creation.
    """
    val = _core_.new_PreWindow(*args, **kwargs)
    val.thisown = 1
    return val

def Window_NewControlId(*args, **kwargs):
    """
    Window_NewControlId() -> int

    Generate a control id for the controls which were not given one.
    """
    return _core_.Window_NewControlId(*args, **kwargs)

def Window_NextControlId(*args, **kwargs):
    """
    Window_NextControlId(int winid) -> int

    Get the id of the control following the one with the given
    autogenerated) id
    """
    return _core_.Window_NextControlId(*args, **kwargs)

def Window_PrevControlId(*args, **kwargs):
    """
    Window_PrevControlId(int winid) -> int

    Get the id of the control preceding the one with the given
    autogenerated) id
    """
    return _core_.Window_PrevControlId(*args, **kwargs)

def Window_FindFocus(*args, **kwargs):
    """
    Window_FindFocus() -> Window

    Returns the window or control that currently has the keyboard focus,
    or None.
    """
    return _core_.Window_FindFocus(*args, **kwargs)

def Window_GetCapture(*args, **kwargs):
    """
    Window_GetCapture() -> Window

    Returns the window which currently captures the mouse or None
    """
    return _core_.Window_GetCapture(*args, **kwargs)

def Window_GetClassDefaultAttributes(*args, **kwargs):
    """
    Window_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
    return _core_.Window_GetClassDefaultAttributes(*args, **kwargs)

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


def FindWindowById(*args, **kwargs):
    """
    FindWindowById(long id, Window parent=None) -> Window

    Find the first window in the application with the given id. If parent
    is None, the search will start from all top-level frames and dialog
    boxes; if non-None, the search will be limited to the given window
    hierarchy. The search is recursive in both cases.
    """
    return _core_.FindWindowById(*args, **kwargs)

def FindWindowByName(*args, **kwargs):
    """
    FindWindowByName(String name, Window parent=None) -> Window

    Find a window by its name (as given in a window constructor or Create
    function call). If parent is None, the search will start from all
    top-level frames and dialog boxes; if non-None, the search will be
    limited to the given window hierarchy. The search is recursive in both
    cases.

    If no window with such name is found, wx.FindWindowByLabel is called.
    """
    return _core_.FindWindowByName(*args, **kwargs)

def FindWindowByLabel(*args, **kwargs):
    """
    FindWindowByLabel(String label, Window parent=None) -> Window

    Find a window by its label. Depending on the type of window, the label
    may be a window title or panel item label. If parent is None, the
    search will start from all top-level frames and dialog boxes; if
    non-None, the search will be limited to the given window
    hierarchy. The search is recursive in both cases.
    """
    return _core_.FindWindowByLabel(*args, **kwargs)

def Window_FromHWND(*args, **kwargs):
    """Window_FromHWND(Window parent, unsigned long _hWnd) -> Window"""
    return _core_.Window_FromHWND(*args, **kwargs)
#---------------------------------------------------------------------------

class Validator(EvtHandler):
    """Proxy of C++ Validator class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxValidator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> Validator"""
        newobj = _core_.new_Validator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Clone(*args, **kwargs):
        """Clone(self) -> Validator"""
        return _core_.Validator_Clone(*args, **kwargs)

    def Validate(*args, **kwargs):
        """Validate(self, Window parent) -> bool"""
        return _core_.Validator_Validate(*args, **kwargs)

    def TransferToWindow(*args, **kwargs):
        """TransferToWindow(self) -> bool"""
        return _core_.Validator_TransferToWindow(*args, **kwargs)

    def TransferFromWindow(*args, **kwargs):
        """TransferFromWindow(self) -> bool"""
        return _core_.Validator_TransferFromWindow(*args, **kwargs)

    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> Window"""
        return _core_.Validator_GetWindow(*args, **kwargs)

    def SetWindow(*args, **kwargs):
        """SetWindow(self, Window window)"""
        return _core_.Validator_SetWindow(*args, **kwargs)

    def IsSilent(*args, **kwargs):
        """IsSilent() -> bool"""
        return _core_.Validator_IsSilent(*args, **kwargs)

    IsSilent = staticmethod(IsSilent)
    def SetBellOnError(*args, **kwargs):
        """SetBellOnError(int doIt=True)"""
        return _core_.Validator_SetBellOnError(*args, **kwargs)

    SetBellOnError = staticmethod(SetBellOnError)

class ValidatorPtr(Validator):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Validator
_core_.Validator_swigregister(ValidatorPtr)

def Validator_IsSilent(*args, **kwargs):
    """Validator_IsSilent() -> bool"""
    return _core_.Validator_IsSilent(*args, **kwargs)

def Validator_SetBellOnError(*args, **kwargs):
    """Validator_SetBellOnError(int doIt=True)"""
    return _core_.Validator_SetBellOnError(*args, **kwargs)

class PyValidator(Validator):
    """Proxy of C++ PyValidator class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyValidator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> PyValidator"""
        newobj = _core_.new_PyValidator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        
        self._setCallbackInfo(self, PyValidator, 1)
        self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class, int incref=True)"""
        return _core_.PyValidator__setCallbackInfo(*args, **kwargs)


class PyValidatorPtr(PyValidator):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyValidator
_core_.PyValidator_swigregister(PyValidatorPtr)

#---------------------------------------------------------------------------

class Menu(EvtHandler):
    """Proxy of C++ Menu class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMenu instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String title=EmptyString, long style=0) -> Menu"""
        newobj = _core_.new_Menu(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Append(*args, **kwargs):
        """Append(self, int id, String text, String help=EmptyString, int kind=ITEM_NORMAL) -> MenuItem"""
        return _core_.Menu_Append(*args, **kwargs)

    def AppendSeparator(*args, **kwargs):
        """AppendSeparator(self) -> MenuItem"""
        return _core_.Menu_AppendSeparator(*args, **kwargs)

    def AppendCheckItem(*args, **kwargs):
        """AppendCheckItem(self, int id, String text, String help=EmptyString) -> MenuItem"""
        return _core_.Menu_AppendCheckItem(*args, **kwargs)

    def AppendRadioItem(*args, **kwargs):
        """AppendRadioItem(self, int id, String text, String help=EmptyString) -> MenuItem"""
        return _core_.Menu_AppendRadioItem(*args, **kwargs)

    def AppendMenu(*args, **kwargs):
        """AppendMenu(self, int id, String text, Menu submenu, String help=EmptyString) -> MenuItem"""
        return _core_.Menu_AppendMenu(*args, **kwargs)

    def AppendItem(*args, **kwargs):
        """AppendItem(self, MenuItem item) -> MenuItem"""
        return _core_.Menu_AppendItem(*args, **kwargs)

    def Break(*args, **kwargs):
        """Break(self)"""
        return _core_.Menu_Break(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """InsertItem(self, size_t pos, MenuItem item) -> MenuItem"""
        return _core_.Menu_InsertItem(*args, **kwargs)

    def Insert(*args, **kwargs):
        """
        Insert(self, size_t pos, int id, String text, String help=EmptyString, 
            int kind=ITEM_NORMAL) -> MenuItem
        """
        return _core_.Menu_Insert(*args, **kwargs)

    def InsertSeparator(*args, **kwargs):
        """InsertSeparator(self, size_t pos) -> MenuItem"""
        return _core_.Menu_InsertSeparator(*args, **kwargs)

    def InsertCheckItem(*args, **kwargs):
        """InsertCheckItem(self, size_t pos, int id, String text, String help=EmptyString) -> MenuItem"""
        return _core_.Menu_InsertCheckItem(*args, **kwargs)

    def InsertRadioItem(*args, **kwargs):
        """InsertRadioItem(self, size_t pos, int id, String text, String help=EmptyString) -> MenuItem"""
        return _core_.Menu_InsertRadioItem(*args, **kwargs)

    def InsertMenu(*args, **kwargs):
        """InsertMenu(self, size_t pos, int id, String text, Menu submenu, String help=EmptyString) -> MenuItem"""
        return _core_.Menu_InsertMenu(*args, **kwargs)

    def PrependItem(*args, **kwargs):
        """PrependItem(self, MenuItem item) -> MenuItem"""
        return _core_.Menu_PrependItem(*args, **kwargs)

    def Prepend(*args, **kwargs):
        """Prepend(self, int id, String text, String help=EmptyString, int kind=ITEM_NORMAL) -> MenuItem"""
        return _core_.Menu_Prepend(*args, **kwargs)

    def PrependSeparator(*args, **kwargs):
        """PrependSeparator(self) -> MenuItem"""
        return _core_.Menu_PrependSeparator(*args, **kwargs)

    def PrependCheckItem(*args, **kwargs):
        """PrependCheckItem(self, int id, String text, String help=EmptyString) -> MenuItem"""
        return _core_.Menu_PrependCheckItem(*args, **kwargs)

    def PrependRadioItem(*args, **kwargs):
        """PrependRadioItem(self, int id, String text, String help=EmptyString) -> MenuItem"""
        return _core_.Menu_PrependRadioItem(*args, **kwargs)

    def PrependMenu(*args, **kwargs):
        """PrependMenu(self, int id, String text, Menu submenu, String help=EmptyString) -> MenuItem"""
        return _core_.Menu_PrependMenu(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(self, int id) -> MenuItem"""
        return _core_.Menu_Remove(*args, **kwargs)

    def RemoveItem(*args, **kwargs):
        """RemoveItem(self, MenuItem item) -> MenuItem"""
        return _core_.Menu_RemoveItem(*args, **kwargs)

    def Delete(*args, **kwargs):
        """Delete(self, int id) -> bool"""
        return _core_.Menu_Delete(*args, **kwargs)

    def DeleteItem(*args, **kwargs):
        """DeleteItem(self, MenuItem item) -> bool"""
        return _core_.Menu_DeleteItem(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """
        Destroy(self)

        Deletes the C++ object this Python object is a proxy for.
        """
        return _core_.Menu_Destroy(*args, **kwargs)

    def DestroyId(*args, **kwargs):
        """
        DestroyId(self, int id) -> bool

        Deletes the C++ object this Python object is a proxy for.
        """
        return _core_.Menu_DestroyId(*args, **kwargs)

    def DestroyItem(*args, **kwargs):
        """
        DestroyItem(self, MenuItem item) -> bool

        Deletes the C++ object this Python object is a proxy for.
        """
        return _core_.Menu_DestroyItem(*args, **kwargs)

    def GetMenuItemCount(*args, **kwargs):
        """GetMenuItemCount(self) -> size_t"""
        return _core_.Menu_GetMenuItemCount(*args, **kwargs)

    def GetMenuItems(*args, **kwargs):
        """GetMenuItems(self) -> PyObject"""
        return _core_.Menu_GetMenuItems(*args, **kwargs)

    def FindItem(*args, **kwargs):
        """FindItem(self, String item) -> int"""
        return _core_.Menu_FindItem(*args, **kwargs)

    def FindItemById(*args, **kwargs):
        """FindItemById(self, int id) -> MenuItem"""
        return _core_.Menu_FindItemById(*args, **kwargs)

    def FindItemByPosition(*args, **kwargs):
        """FindItemByPosition(self, size_t position) -> MenuItem"""
        return _core_.Menu_FindItemByPosition(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(self, int id, bool enable)"""
        return _core_.Menu_Enable(*args, **kwargs)

    def IsEnabled(*args, **kwargs):
        """IsEnabled(self, int id) -> bool"""
        return _core_.Menu_IsEnabled(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(self, int id, bool check)"""
        return _core_.Menu_Check(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked(self, int id) -> bool"""
        return _core_.Menu_IsChecked(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """SetLabel(self, int id, String label)"""
        return _core_.Menu_SetLabel(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel(self, int id) -> String"""
        return _core_.Menu_GetLabel(*args, **kwargs)

    def SetHelpString(*args, **kwargs):
        """SetHelpString(self, int id, String helpString)"""
        return _core_.Menu_SetHelpString(*args, **kwargs)

    def GetHelpString(*args, **kwargs):
        """GetHelpString(self, int id) -> String"""
        return _core_.Menu_GetHelpString(*args, **kwargs)

    def SetTitle(*args, **kwargs):
        """SetTitle(self, String title)"""
        return _core_.Menu_SetTitle(*args, **kwargs)

    def GetTitle(*args, **kwargs):
        """GetTitle(self) -> String"""
        return _core_.Menu_GetTitle(*args, **kwargs)

    def SetEventHandler(*args, **kwargs):
        """SetEventHandler(self, EvtHandler handler)"""
        return _core_.Menu_SetEventHandler(*args, **kwargs)

    def GetEventHandler(*args, **kwargs):
        """GetEventHandler(self) -> EvtHandler"""
        return _core_.Menu_GetEventHandler(*args, **kwargs)

    def SetInvokingWindow(*args, **kwargs):
        """SetInvokingWindow(self, Window win)"""
        return _core_.Menu_SetInvokingWindow(*args, **kwargs)

    def GetInvokingWindow(*args, **kwargs):
        """GetInvokingWindow(self) -> Window"""
        return _core_.Menu_GetInvokingWindow(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self) -> long"""
        return _core_.Menu_GetStyle(*args, **kwargs)

    def UpdateUI(*args, **kwargs):
        """UpdateUI(self, EvtHandler source=None)"""
        return _core_.Menu_UpdateUI(*args, **kwargs)

    def GetMenuBar(*args, **kwargs):
        """GetMenuBar(self) -> MenuBar"""
        return _core_.Menu_GetMenuBar(*args, **kwargs)

    def Attach(*args, **kwargs):
        """Attach(self, wxMenuBarBase menubar)"""
        return _core_.Menu_Attach(*args, **kwargs)

    def Detach(*args, **kwargs):
        """Detach(self)"""
        return _core_.Menu_Detach(*args, **kwargs)

    def IsAttached(*args, **kwargs):
        """IsAttached(self) -> bool"""
        return _core_.Menu_IsAttached(*args, **kwargs)

    def SetParent(*args, **kwargs):
        """SetParent(self, Menu parent)"""
        return _core_.Menu_SetParent(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent(self) -> Menu"""
        return _core_.Menu_GetParent(*args, **kwargs)


class MenuPtr(Menu):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Menu
_core_.Menu_swigregister(MenuPtr)
DefaultValidator = cvar.DefaultValidator

#---------------------------------------------------------------------------

class MenuBar(Window):
    """Proxy of C++ MenuBar class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMenuBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, long style=0) -> MenuBar"""
        newobj = _core_.new_MenuBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Append(*args, **kwargs):
        """Append(self, Menu menu, String title) -> bool"""
        return _core_.MenuBar_Append(*args, **kwargs)

    def Insert(*args, **kwargs):
        """Insert(self, size_t pos, Menu menu, String title) -> bool"""
        return _core_.MenuBar_Insert(*args, **kwargs)

    def GetMenuCount(*args, **kwargs):
        """GetMenuCount(self) -> size_t"""
        return _core_.MenuBar_GetMenuCount(*args, **kwargs)

    def GetMenu(*args, **kwargs):
        """GetMenu(self, size_t pos) -> Menu"""
        return _core_.MenuBar_GetMenu(*args, **kwargs)

    def Replace(*args, **kwargs):
        """Replace(self, size_t pos, Menu menu, String title) -> Menu"""
        return _core_.MenuBar_Replace(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(self, size_t pos) -> Menu"""
        return _core_.MenuBar_Remove(*args, **kwargs)

    def EnableTop(*args, **kwargs):
        """EnableTop(self, size_t pos, bool enable)"""
        return _core_.MenuBar_EnableTop(*args, **kwargs)

    def IsEnabledTop(*args, **kwargs):
        """IsEnabledTop(self, size_t pos) -> bool"""
        return _core_.MenuBar_IsEnabledTop(*args, **kwargs)

    def SetLabelTop(*args, **kwargs):
        """SetLabelTop(self, size_t pos, String label)"""
        return _core_.MenuBar_SetLabelTop(*args, **kwargs)

    def GetLabelTop(*args, **kwargs):
        """GetLabelTop(self, size_t pos) -> String"""
        return _core_.MenuBar_GetLabelTop(*args, **kwargs)

    def FindMenuItem(*args, **kwargs):
        """FindMenuItem(self, String menu, String item) -> int"""
        return _core_.MenuBar_FindMenuItem(*args, **kwargs)

    def FindItemById(*args, **kwargs):
        """FindItemById(self, int id) -> MenuItem"""
        return _core_.MenuBar_FindItemById(*args, **kwargs)

    def FindMenu(*args, **kwargs):
        """FindMenu(self, String title) -> int"""
        return _core_.MenuBar_FindMenu(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(self, int id, bool enable)"""
        return _core_.MenuBar_Enable(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(self, int id, bool check)"""
        return _core_.MenuBar_Check(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked(self, int id) -> bool"""
        return _core_.MenuBar_IsChecked(*args, **kwargs)

    def IsEnabled(*args, **kwargs):
        """IsEnabled(self, int id) -> bool"""
        return _core_.MenuBar_IsEnabled(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """SetLabel(self, int id, String label)"""
        return _core_.MenuBar_SetLabel(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel(self, int id) -> String"""
        return _core_.MenuBar_GetLabel(*args, **kwargs)

    def SetHelpString(*args, **kwargs):
        """SetHelpString(self, int id, String helpString)"""
        return _core_.MenuBar_SetHelpString(*args, **kwargs)

    def GetHelpString(*args, **kwargs):
        """GetHelpString(self, int id) -> String"""
        return _core_.MenuBar_GetHelpString(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame(self) -> wxFrame"""
        return _core_.MenuBar_GetFrame(*args, **kwargs)

    def IsAttached(*args, **kwargs):
        """IsAttached(self) -> bool"""
        return _core_.MenuBar_IsAttached(*args, **kwargs)

    def Attach(*args, **kwargs):
        """Attach(self, wxFrame frame)"""
        return _core_.MenuBar_Attach(*args, **kwargs)

    def Detach(*args, **kwargs):
        """Detach(self)"""
        return _core_.MenuBar_Detach(*args, **kwargs)


class MenuBarPtr(MenuBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MenuBar
_core_.MenuBar_swigregister(MenuBarPtr)

#---------------------------------------------------------------------------

class MenuItem(Object):
    """Proxy of C++ MenuItem class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMenuItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Menu parentMenu=None, int id=ID_ANY, String text=EmptyString, 
            String help=EmptyString, int kind=ITEM_NORMAL, 
            Menu subMenu=None) -> MenuItem
        """
        newobj = _core_.new_MenuItem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetMenu(*args, **kwargs):
        """GetMenu(self) -> Menu"""
        return _core_.MenuItem_GetMenu(*args, **kwargs)

    def SetMenu(*args, **kwargs):
        """SetMenu(self, Menu menu)"""
        return _core_.MenuItem_SetMenu(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(self, int id)"""
        return _core_.MenuItem_SetId(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> int"""
        return _core_.MenuItem_GetId(*args, **kwargs)

    def IsSeparator(*args, **kwargs):
        """IsSeparator(self) -> bool"""
        return _core_.MenuItem_IsSeparator(*args, **kwargs)

    def SetText(*args, **kwargs):
        """SetText(self, String str)"""
        return _core_.MenuItem_SetText(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel(self) -> String"""
        return _core_.MenuItem_GetLabel(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText(self) -> String"""
        return _core_.MenuItem_GetText(*args, **kwargs)

    def GetLabelFromText(*args, **kwargs):
        """GetLabelFromText(String text) -> String"""
        return _core_.MenuItem_GetLabelFromText(*args, **kwargs)

    GetLabelFromText = staticmethod(GetLabelFromText)
    def GetKind(*args, **kwargs):
        """GetKind(self) -> int"""
        return _core_.MenuItem_GetKind(*args, **kwargs)

    def SetKind(*args, **kwargs):
        """SetKind(self, int kind)"""
        return _core_.MenuItem_SetKind(*args, **kwargs)

    def SetCheckable(*args, **kwargs):
        """SetCheckable(self, bool checkable)"""
        return _core_.MenuItem_SetCheckable(*args, **kwargs)

    def IsCheckable(*args, **kwargs):
        """IsCheckable(self) -> bool"""
        return _core_.MenuItem_IsCheckable(*args, **kwargs)

    def IsSubMenu(*args, **kwargs):
        """IsSubMenu(self) -> bool"""
        return _core_.MenuItem_IsSubMenu(*args, **kwargs)

    def SetSubMenu(*args, **kwargs):
        """SetSubMenu(self, Menu menu)"""
        return _core_.MenuItem_SetSubMenu(*args, **kwargs)

    def GetSubMenu(*args, **kwargs):
        """GetSubMenu(self) -> Menu"""
        return _core_.MenuItem_GetSubMenu(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(self, bool enable=True)"""
        return _core_.MenuItem_Enable(*args, **kwargs)

    def IsEnabled(*args, **kwargs):
        """IsEnabled(self) -> bool"""
        return _core_.MenuItem_IsEnabled(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(self, bool check=True)"""
        return _core_.MenuItem_Check(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked(self) -> bool"""
        return _core_.MenuItem_IsChecked(*args, **kwargs)

    def Toggle(*args, **kwargs):
        """Toggle(self)"""
        return _core_.MenuItem_Toggle(*args, **kwargs)

    def SetHelp(*args, **kwargs):
        """SetHelp(self, String str)"""
        return _core_.MenuItem_SetHelp(*args, **kwargs)

    def GetHelp(*args, **kwargs):
        """GetHelp(self) -> String"""
        return _core_.MenuItem_GetHelp(*args, **kwargs)

    def GetAccel(*args, **kwargs):
        """GetAccel(self) -> AcceleratorEntry"""
        return _core_.MenuItem_GetAccel(*args, **kwargs)

    def SetAccel(*args, **kwargs):
        """SetAccel(self, AcceleratorEntry accel)"""
        return _core_.MenuItem_SetAccel(*args, **kwargs)

    def GetDefaultMarginWidth(*args, **kwargs):
        """GetDefaultMarginWidth() -> int"""
        return _core_.MenuItem_GetDefaultMarginWidth(*args, **kwargs)

    GetDefaultMarginWidth = staticmethod(GetDefaultMarginWidth)
    def SetBitmap(*args, **kwargs):
        """SetBitmap(self, Bitmap bitmap)"""
        return _core_.MenuItem_SetBitmap(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap(self) -> Bitmap"""
        return _core_.MenuItem_GetBitmap(*args, **kwargs)


class MenuItemPtr(MenuItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MenuItem
_core_.MenuItem_swigregister(MenuItemPtr)

def MenuItem_GetLabelFromText(*args, **kwargs):
    """MenuItem_GetLabelFromText(String text) -> String"""
    return _core_.MenuItem_GetLabelFromText(*args, **kwargs)

def MenuItem_GetDefaultMarginWidth(*args, **kwargs):
    """MenuItem_GetDefaultMarginWidth() -> int"""
    return _core_.MenuItem_GetDefaultMarginWidth(*args, **kwargs)

#---------------------------------------------------------------------------

class Control(Window):
    """
    This is the base class for a control or 'widget'.

    A control is generally a small window which processes user input
    and/or displays one or more item of data.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxControl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, Validator validator=DefaultValidator, 
            String name=ControlNameStr) -> Control

        Create a Control.  Normally you should only call this from a subclass'
        __init__ as a plain old wx.Control is not very useful.
        """
        newobj = _core_.new_Control(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, Validator validator=DefaultValidator, 
            String name=ControlNameStr) -> bool

        Do the 2nd phase and create the GUI control.
        """
        return _core_.Control_Create(*args, **kwargs)

    def Command(*args, **kwargs):
        """
        Command(self, CommandEvent event)

        Simulates the effect of the user issuing a command to the item.

        :see: `wx.CommandEvent`

        """
        return _core_.Control_Command(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """
        GetLabel(self) -> String

        Return a control's text.
        """
        return _core_.Control_GetLabel(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """
        SetLabel(self, String label)

        Sets the item's text.
        """
        return _core_.Control_SetLabel(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
        return _core_.Control_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)

class ControlPtr(Control):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Control
_core_.Control_swigregister(ControlPtr)
ControlNameStr = cvar.ControlNameStr

def PreControl(*args, **kwargs):
    """
    PreControl() -> Control

    Precreate a Control control for 2-phase creation
    """
    val = _core_.new_PreControl(*args, **kwargs)
    val.thisown = 1
    return val

def Control_GetClassDefaultAttributes(*args, **kwargs):
    """
    Control_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
    return _core_.Control_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class ItemContainer(object):
    """
    wx.ItemContainer defines an interface which is implemented by all
    controls which have string subitems, each of which may be selected,
    such as `wx.ListBox`, `wx.CheckListBox`, `wx.Choice` as well as
    `wx.ComboBox` which implements an extended interface deriving from
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
        Append(self, String item, PyObject clientData=None) -> int

        Adds the item to the control, associating the given data with the item
        if not None.  The return value is the index of the newly added item
        which may be different from the last one if the control is sorted (e.g.
        has wx.LB_SORT or wx.CB_SORT style).
        """
        return _core_.ItemContainer_Append(*args, **kwargs)

    def AppendItems(*args, **kwargs):
        """
        AppendItems(self, List strings)

        Apend several items at once to the control.  Notice that calling this
        method may be much faster than appending the items one by one if you
        need to add a lot of items.
        """
        return _core_.ItemContainer_AppendItems(*args, **kwargs)

    def Insert(*args, **kwargs):
        """
        Insert(self, String item, int pos, PyObject clientData=None) -> int

        Insert an item into the control before the item at the ``pos`` index,
        optionally associating some data object with the item.
        """
        return _core_.ItemContainer_Insert(*args, **kwargs)

    def Clear(*args, **kwargs):
        """
        Clear(self)

        Removes all items from the control.
        """
        return _core_.ItemContainer_Clear(*args, **kwargs)

    def Delete(*args, **kwargs):
        """
        Delete(self, int n)

        Deletes the item at the zero-based index 'n' from the control. Note
        that it is an error (signalled by a `wx.PyAssertionError` exception if
        enabled) to remove an item with the index negative or greater or equal
        than the number of items in the control.
        """
        return _core_.ItemContainer_Delete(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """
        GetCount(self) -> int

        Returns the number of items in the control.
        """
        return _core_.ItemContainer_GetCount(*args, **kwargs)

    def IsEmpty(*args, **kwargs):
        """
        IsEmpty(self) -> bool

        Returns True if the control is empty or False if it has some items.
        """
        return _core_.ItemContainer_IsEmpty(*args, **kwargs)

    def GetString(*args, **kwargs):
        """
        GetString(self, int n) -> String

        Returns the label of the item with the given index.
        """
        return _core_.ItemContainer_GetString(*args, **kwargs)

    def GetStrings(*args, **kwargs):
        """GetStrings(self) -> wxArrayString"""
        return _core_.ItemContainer_GetStrings(*args, **kwargs)

    def SetString(*args, **kwargs):
        """
        SetString(self, int n, String s)

        Sets the label for the given item.
        """
        return _core_.ItemContainer_SetString(*args, **kwargs)

    def FindString(*args, **kwargs):
        """
        FindString(self, String s) -> int

        Finds an item whose label matches the given string.  Returns the
        zero-based position of the item, or ``wx.NOT_FOUND`` if the string was not
        found.
        """
        return _core_.ItemContainer_FindString(*args, **kwargs)

    def Select(*args, **kwargs):
        """
        Select(self, int n)

        Sets the item at index 'n' to be the selected item.
        """
        return _core_.ItemContainer_Select(*args, **kwargs)

    SetSelection = Select 
    def GetSelection(*args, **kwargs):
        """
        GetSelection(self) -> int

        Returns the index of the selected item or ``wx.NOT_FOUND`` if no item
        is selected.
        """
        return _core_.ItemContainer_GetSelection(*args, **kwargs)

    def GetStringSelection(*args, **kwargs):
        """
        GetStringSelection(self) -> String

        Returns the label of the selected item or an empty string if no item
        is selected.
        """
        return _core_.ItemContainer_GetStringSelection(*args, **kwargs)

    def GetClientData(*args, **kwargs):
        """
        GetClientData(self, int n) -> PyObject

        Returns the client data associated with the given item, (if any.)
        """
        return _core_.ItemContainer_GetClientData(*args, **kwargs)

    def SetClientData(*args, **kwargs):
        """
        SetClientData(self, int n, PyObject clientData)

        Associate the given client data with the item at position n.
        """
        return _core_.ItemContainer_SetClientData(*args, **kwargs)


class ItemContainerPtr(ItemContainer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ItemContainer
_core_.ItemContainer_swigregister(ItemContainerPtr)

#---------------------------------------------------------------------------

class ControlWithItems(Control,ItemContainer):
    """
    wx.ControlWithItems combines the ``wx.ItemContainer`` class with the
    wx.Control class, and is used for the base class of various controls
    that have items.
    """
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxControlWithItems instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ControlWithItemsPtr(ControlWithItems):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ControlWithItems
_core_.ControlWithItems_swigregister(ControlWithItemsPtr)

#---------------------------------------------------------------------------

class SizerItem(Object):
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
        __init__(self) -> SizerItem

        Constructs an empty wx.SizerItem.  Either a window, sizer or spacer
        size will need to be set before this item can be used in a Sizer.

        You will probably never need to create a wx.SizerItem directly as they
        are created automatically when the sizer's Add, Insert or Prepend
        methods are called.

        :see: `wx.SizerItemSpacer`, `wx.SizerItemWindow`, `wx.SizerItemSizer`
        """
        newobj = _core_.new_SizerItem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def DeleteWindows(*args, **kwargs):
        """
        DeleteWindows(self)

        Destroy the window or the windows in a subsizer, depending on the type
        of item.
        """
        return _core_.SizerItem_DeleteWindows(*args, **kwargs)

    def DetachSizer(*args, **kwargs):
        """
        DetachSizer(self)

        Enable deleting the SizerItem without destroying the contained sizer.
        """
        return _core_.SizerItem_DetachSizer(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> Size

        Get the current size of the item, as set in the last Layout.
        """
        return _core_.SizerItem_GetSize(*args, **kwargs)

    def CalcMin(*args, **kwargs):
        """
        CalcMin(self) -> Size

        Calculates the minimum desired size for the item, including any space
        needed by borders.
        """
        return _core_.SizerItem_CalcMin(*args, **kwargs)

    def SetDimension(*args, **kwargs):
        """
        SetDimension(self, Point pos, Size size)

        Set the position and size of the space allocated for this item by the
        sizer, and adjust the position and size of the item (window or
        subsizer) to be within that space taking alignment and borders into
        account.
        """
        return _core_.SizerItem_SetDimension(*args, **kwargs)

    def GetMinSize(*args, **kwargs):
        """
        GetMinSize(self) -> Size

        Get the minimum size needed for the item.
        """
        return _core_.SizerItem_GetMinSize(*args, **kwargs)

    def GetMinSizeWithBorder(*args, **kwargs):
        """
        GetMinSizeWithBorder(self) -> Size

        Get the minimum size needed for the item with space for the borders
        added, if needed.
        """
        return _core_.SizerItem_GetMinSizeWithBorder(*args, **kwargs)

    def SetInitSize(*args, **kwargs):
        """SetInitSize(self, int x, int y)"""
        return _core_.SizerItem_SetInitSize(*args, **kwargs)

    def SetRatioWH(*args, **kwargs):
        """
        SetRatioWH(self, int width, int height)

        Set the ratio item attribute.
        """
        return _core_.SizerItem_SetRatioWH(*args, **kwargs)

    def SetRatioSize(*args, **kwargs):
        """
        SetRatioSize(self, Size size)

        Set the ratio item attribute.
        """
        return _core_.SizerItem_SetRatioSize(*args, **kwargs)

    def SetRatio(*args, **kwargs):
        """
        SetRatio(self, float ratio)

        Set the ratio item attribute.
        """
        return _core_.SizerItem_SetRatio(*args, **kwargs)

    def GetRatio(*args, **kwargs):
        """
        GetRatio(self) -> float

        Set the ratio item attribute.
        """
        return _core_.SizerItem_GetRatio(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """
        GetRect(self) -> Rect

        Returns the rectangle that the sizer item should occupy
        """
        return _core_.SizerItem_GetRect(*args, **kwargs)

    def IsWindow(*args, **kwargs):
        """
        IsWindow(self) -> bool

        Is this sizer item a window?
        """
        return _core_.SizerItem_IsWindow(*args, **kwargs)

    def IsSizer(*args, **kwargs):
        """
        IsSizer(self) -> bool

        Is this sizer item a subsizer?
        """
        return _core_.SizerItem_IsSizer(*args, **kwargs)

    def IsSpacer(*args, **kwargs):
        """
        IsSpacer(self) -> bool

        Is this sizer item a spacer?
        """
        return _core_.SizerItem_IsSpacer(*args, **kwargs)

    def SetProportion(*args, **kwargs):
        """
        SetProportion(self, int proportion)

        Set the proportion value for this item.
        """
        return _core_.SizerItem_SetProportion(*args, **kwargs)

    def GetProportion(*args, **kwargs):
        """
        GetProportion(self) -> int

        Get the proportion value for this item.
        """
        return _core_.SizerItem_GetProportion(*args, **kwargs)

    SetOption = wx._deprecated(SetProportion, "Please use `SetProportion` instead.") 
    GetOption = wx._deprecated(GetProportion, "Please use `GetProportion` instead.") 
    def SetFlag(*args, **kwargs):
        """
        SetFlag(self, int flag)

        Set the flag value for this item.
        """
        return _core_.SizerItem_SetFlag(*args, **kwargs)

    def GetFlag(*args, **kwargs):
        """
        GetFlag(self) -> int

        Get the flag value for this item.
        """
        return _core_.SizerItem_GetFlag(*args, **kwargs)

    def SetBorder(*args, **kwargs):
        """
        SetBorder(self, int border)

        Set the border value for this item.
        """
        return _core_.SizerItem_SetBorder(*args, **kwargs)

    def GetBorder(*args, **kwargs):
        """
        GetBorder(self) -> int

        Get the border value for this item.
        """
        return _core_.SizerItem_GetBorder(*args, **kwargs)

    def GetWindow(*args, **kwargs):
        """
        GetWindow(self) -> Window

        Get the window (if any) that is managed by this sizer item.
        """
        return _core_.SizerItem_GetWindow(*args, **kwargs)

    def SetWindow(*args, **kwargs):
        """
        SetWindow(self, Window window)

        Set the window to be managed by this sizer item.
        """
        return _core_.SizerItem_SetWindow(*args, **kwargs)

    def GetSizer(*args, **kwargs):
        """
        GetSizer(self) -> Sizer

        Get the subsizer (if any) that is managed by this sizer item.
        """
        return _core_.SizerItem_GetSizer(*args, **kwargs)

    def SetSizer(*args, **kwargs):
        """
        SetSizer(self, Sizer sizer)

        Set the subsizer to be managed by this sizer item.
        """
        return _core_.SizerItem_SetSizer(*args, **kwargs)

    def GetSpacer(*args, **kwargs):
        """
        GetSpacer(self) -> Size

        Get the size of the spacer managed by this sizer item.
        """
        return _core_.SizerItem_GetSpacer(*args, **kwargs)

    def SetSpacer(*args, **kwargs):
        """
        SetSpacer(self, Size size)

        Set the size of the spacer to be managed by this sizer item.
        """
        return _core_.SizerItem_SetSpacer(*args, **kwargs)

    def Show(*args, **kwargs):
        """
        Show(self, bool show)

        Set the show item attribute, which sizers use to determine if the item
        is to be made part of the layout or not. If the item is tracking a
        window then it is shown or hidden as needed.
        """
        return _core_.SizerItem_Show(*args, **kwargs)

    def IsShown(*args, **kwargs):
        """
        IsShown(self) -> bool

        Is the item to be shown in the layout?
        """
        return _core_.SizerItem_IsShown(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Returns the current position of the item, as set in the last Layout.
        """
        return _core_.SizerItem_GetPosition(*args, **kwargs)

    def GetUserData(*args, **kwargs):
        """
        GetUserData(self) -> PyObject

        Returns the userData associated with this sizer item, or None if there
        isn't any.
        """
        return _core_.SizerItem_GetUserData(*args, **kwargs)


class SizerItemPtr(SizerItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SizerItem
_core_.SizerItem_swigregister(SizerItemPtr)

def SizerItemWindow(*args, **kwargs):
    """
    SizerItemWindow(Window window, int proportion, int flag, int border, 
        PyObject userData=None) -> SizerItem

    Constructs a `wx.SizerItem` for tracking a window.
    """
    val = _core_.new_SizerItemWindow(*args, **kwargs)
    val.thisown = 1
    return val

def SizerItemSpacer(*args, **kwargs):
    """
    SizerItemSpacer(int width, int height, int proportion, int flag, int border, 
        PyObject userData=None) -> SizerItem

    Constructs a `wx.SizerItem` for tracking a spacer.
    """
    val = _core_.new_SizerItemSpacer(*args, **kwargs)
    val.thisown = 1
    return val

def SizerItemSizer(*args, **kwargs):
    """
    SizerItemSizer(Sizer sizer, int proportion, int flag, int border, 
        PyObject userData=None) -> SizerItem

    Constructs a `wx.SizerItem` for tracking a subsizer
    """
    val = _core_.new_SizerItemSizer(*args, **kwargs)
    val.thisown = 1
    return val

class Sizer(Object):
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
        return _core_.Sizer__setOORInfo(*args, **kwargs)

    def Add(*args, **kwargs):
        """
        Add(self, item, int proportion=0, int flag=0, int border=0,
            PyObject userData=None) -> wx.SizerItem

        Appends a child item to the sizer.
        """
        return _core_.Sizer_Add(*args, **kwargs)

    def Insert(*args, **kwargs):
        """
        Insert(self, int before, item, int proportion=0, int flag=0, int border=0,
            PyObject userData=None) -> wx.SizerItem

        Inserts a new item into the list of items managed by this sizer before
        the item at index *before*.  See `Add` for a description of the parameters.
        """
        return _core_.Sizer_Insert(*args, **kwargs)

    def Prepend(*args, **kwargs):
        """
        Prepend(self, item, int proportion=0, int flag=0, int border=0,
            PyObject userData=None) -> wx.SizerItem

        Adds a new item to the begining of the list of sizer items managed by
        this sizer.  See `Add` for a description of the parameters.
        """
        return _core_.Sizer_Prepend(*args, **kwargs)

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
        return _core_.Sizer_Remove(*args, **kwargs)

    def Detach(*args, **kwargs):
        """
        Detach(self, item) -> bool

        Detaches an item from the sizer without destroying it.  This method
        does not cause any layout or resizing to take place, call `Layout` to
        do so.  The *item* parameter can be either a window, a sizer, or the
        zero-based index of the item to be detached.  Returns True if the child item
        was found and detached.
        """
        return _core_.Sizer_Detach(*args, **kwargs)

    def GetItem(*args, **kwargs):
        """
        GetItem(self, item) -> wx.SizerItem

        Returns the `wx.SizerItem` which holds the *item* given.  The *item*
        parameter can be either a window, a sizer, or the zero-based index of
        the item to be detached.
        """
        return _core_.Sizer_GetItem(*args, **kwargs)

    def _SetItemMinSize(*args, **kwargs):
        """_SetItemMinSize(self, PyObject item, Size size)"""
        return _core_.Sizer__SetItemMinSize(*args, **kwargs)

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
        return _core_.Sizer_AddItem(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """
        InsertItem(self, int index, SizerItem item)

        Inserts a `wx.SizerItem` to the sizer at the position given by *index*.
        """
        return _core_.Sizer_InsertItem(*args, **kwargs)

    def PrependItem(*args, **kwargs):
        """
        PrependItem(self, SizerItem item)

        Prepends a `wx.SizerItem` to the sizer.
        """
        return _core_.Sizer_PrependItem(*args, **kwargs)

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
    AddWindow     = wx._deprecated(Add, "AddWindow is deprecated, use `Add` instead.")
    AddSizer      = wx._deprecated(Add, "AddSizer is deprecated, use `Add` instead.")
    AddSpacer     = wx._deprecated(Add, "AddSpacer is deprecated, use `Add` instead.")
    PrependWindow = wx._deprecated(Prepend, "PrependWindow is deprecated, use `Prepend` instead.")
    PrependSizer  = wx._deprecated(Prepend, "PrependSizer is deprecated, use `Prepend` instead.")
    PrependSpacer = wx._deprecated(Prepend, "PrependSpacer is deprecated, use `Prepend` instead.")
    InsertWindow  = wx._deprecated(Insert, "InsertWindow is deprecated, use `Insert` instead.")
    InsertSizer   = wx._deprecated(Insert, "InsertSizer is deprecated, use `Insert` instead.")
    InsertSpacer  = wx._deprecated(Insert, "InsertSpacer is deprecated, use `Insert` instead.")
    RemoveWindow  = wx._deprecated(Remove, "RemoveWindow is deprecated, use `Remove` instead.")
    RemoveSizer   = wx._deprecated(Remove, "RemoveSizer is deprecated, use `Remove` instead.")
    RemovePos     = wx._deprecated(Remove, "RemovePos is deprecated, use `Remove` instead.")


    def SetDimension(*args, **kwargs):
        """
        SetDimension(self, int x, int y, int width, int height)

        Call this to force the sizer to take the given dimension and thus
        force the items owned by the sizer to resize themselves according to
        the rules defined by the parameter in the `Add`, `Insert` or `Prepend`
        methods.
        """
        return _core_.Sizer_SetDimension(*args, **kwargs)

    def SetMinSize(*args, **kwargs):
        """
        SetMinSize(self, Size size)

        Call this to give the sizer a minimal size. Normally, the sizer will
        calculate its minimal size based purely on how much space its children
        need. After calling this method `GetMinSize` will return either the
        minimal size as requested by its children or the minimal size set
        here, depending on which is bigger.
        """
        return _core_.Sizer_SetMinSize(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> Size

        Returns the current size of the space managed by the sizer.
        """
        return _core_.Sizer_GetSize(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Returns the current position of the sizer's managed space.
        """
        return _core_.Sizer_GetPosition(*args, **kwargs)

    def GetMinSize(*args, **kwargs):
        """
        GetMinSize(self) -> Size

        Returns the minimal size of the sizer. This is either the combined
        minimal size of all the children and their borders or the minimal size
        set by SetMinSize, depending on which is bigger.
        """
        return _core_.Sizer_GetMinSize(*args, **kwargs)

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
        return _core_.Sizer_RecalcSizes(*args, **kwargs)

    def CalcMin(*args, **kwargs):
        """
        CalcMin(self) -> Size

        This method is where the sizer will do the actual calculation of its
        children's minimal sizes.  You should not need to call this directly as
        it is called by `Layout`.
        """
        return _core_.Sizer_CalcMin(*args, **kwargs)

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
        return _core_.Sizer_Layout(*args, **kwargs)

    def Fit(*args, **kwargs):
        """
        Fit(self, Window window) -> Size

        Tell the sizer to resize the *window* to match the sizer's minimal
        size. This is commonly done in the constructor of the window itself in
        order to set its initial size to match the needs of the children as
        determined by the sizer.  Returns the new size.

        For a top level window this is the total window size, not the client size.
        """
        return _core_.Sizer_Fit(*args, **kwargs)

    def FitInside(*args, **kwargs):
        """
        FitInside(self, Window window)

        Tell the sizer to resize the *virtual size* of the *window* to match the
        sizer's minimal size. This will not alter the on screen size of the
        window, but may cause the addition/removal/alteration of scrollbars
        required to view the virtual area in windows which manage it.

        :see: `wx.ScrolledWindow.SetScrollbars`, `SetVirtualSizeHints`

        """
        return _core_.Sizer_FitInside(*args, **kwargs)

    def SetSizeHints(*args, **kwargs):
        """
        SetSizeHints(self, Window window)

        Tell the sizer to set (and `Fit`) the minimal size of the *window* to
        match the sizer's minimal size. This is commonly done in the
        constructor of the window itself if the window is resizable (as are
        many dialogs under Unix and frames on probably all platforms) in order
        to prevent the window from being sized smaller than the minimal size
        required by the sizer.
        """
        return _core_.Sizer_SetSizeHints(*args, **kwargs)

    def SetVirtualSizeHints(*args, **kwargs):
        """
        SetVirtualSizeHints(self, Window window)

        Tell the sizer to set the minimal size of the window virtual area to
        match the sizer's minimal size. For windows with managed scrollbars
        this will set them appropriately.

        :see: `wx.ScrolledWindow.SetScrollbars`

        """
        return _core_.Sizer_SetVirtualSizeHints(*args, **kwargs)

    def Clear(*args, **kwargs):
        """
        Clear(self, bool deleteWindows=False)

        Clear all items from the sizer, optionally destroying the window items
        as well.
        """
        return _core_.Sizer_Clear(*args, **kwargs)

    def DeleteWindows(*args, **kwargs):
        """
        DeleteWindows(self)

        Destroy all windows managed by the sizer.
        """
        return _core_.Sizer_DeleteWindows(*args, **kwargs)

    def GetChildren(*args, **kwargs):
        """
        GetChildren(sefl) -> list

        Returns a list of all the `wx.SizerItem` objects managed by the sizer.
        """
        return _core_.Sizer_GetChildren(*args, **kwargs)

    def Show(*args, **kwargs):
        """
        Show(self, item, bool show=True, bool recursive=false) -> bool

        Shows or hides an item managed by the sizer.  To make a sizer item
        disappear or reappear, use Show followed by `Layout`.  The *item*
        parameter can be either a window, a sizer, or the zero-based index of
        the item.  Use the recursive parameter to show or hide an item in a
        subsizer.  Returns True if the item was found.
        """
        return _core_.Sizer_Show(*args, **kwargs)

    def IsShown(*args, **kwargs):
        """
        IsShown(self, item)

        Determines if the item is currently shown. sizer.  To make a sizer
        item disappear or reappear, use Show followed by `Layout`.  The *item*
        parameter can be either a window, a sizer, or the zero-based index of
        the item.
        """
        return _core_.Sizer_IsShown(*args, **kwargs)

    def Hide(self, item, recursive=False):
        """
        A convenience method for Show(item, False, recursive).
        """
        return self.Show(item, False, recursive)

    def ShowItems(*args, **kwargs):
        """
        ShowItems(self, bool show)

        Recursively call `wx.SizerItem.Show` on all sizer items.
        """
        return _core_.Sizer_ShowItems(*args, **kwargs)


class SizerPtr(Sizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Sizer
_core_.Sizer_swigregister(SizerPtr)

class PySizer(Sizer):
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
    `CalcMin` and resuing them in `RecalcSizes`.

    :see: `wx.SizerItem`, `wx.Sizer.GetChildren`


    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPySizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> PySizer

        Creates a wx.PySizer.  Must be called from the __init__ in the derived
        class.
        """
        newobj = _core_.new_PySizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PySizer);self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _core_.PySizer__setCallbackInfo(*args, **kwargs)


class PySizerPtr(PySizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PySizer
_core_.PySizer_swigregister(PySizerPtr)

#---------------------------------------------------------------------------

class BoxSizer(Sizer):
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
        __init__(self, int orient=HORIZONTAL) -> BoxSizer

        Constructor for a wx.BoxSizer. *orient* may be one of ``wx.VERTICAL``
        or ``wx.HORIZONTAL`` for creating either a column sizer or a row
        sizer.
        """
        newobj = _core_.new_BoxSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetOrientation(*args, **kwargs):
        """
        GetOrientation(self) -> int

        Returns the current orientation of the sizer.
        """
        return _core_.BoxSizer_GetOrientation(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """
        SetOrientation(self, int orient)

        Resets the orientation of the sizer.
        """
        return _core_.BoxSizer_SetOrientation(*args, **kwargs)


class BoxSizerPtr(BoxSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BoxSizer
_core_.BoxSizer_swigregister(BoxSizerPtr)

#---------------------------------------------------------------------------

class StaticBoxSizer(BoxSizer):
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
        __init__(self, StaticBox box, int orient=HORIZONTAL) -> StaticBoxSizer

        Constructor. It takes an associated static box and the orientation
        *orient* as parameters - orient can be either of ``wx.VERTICAL`` or
        ``wx.HORIZONTAL``.
        """
        newobj = _core_.new_StaticBoxSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetStaticBox(*args, **kwargs):
        """
        GetStaticBox(self) -> StaticBox

        Returns the static box associated with this sizer.
        """
        return _core_.StaticBoxSizer_GetStaticBox(*args, **kwargs)


class StaticBoxSizerPtr(StaticBoxSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticBoxSizer
_core_.StaticBoxSizer_swigregister(StaticBoxSizerPtr)

#---------------------------------------------------------------------------

class GridSizer(Sizer):
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
        __init__(self, int rows=1, int cols=0, int vgap=0, int hgap=0) -> GridSizer

        Constructor for a wx.GridSizer. *rows* and *cols* determine the number
        of columns and rows in the sizer - if either of the parameters is
        zero, it will be calculated to from the total number of children in
        the sizer, thus making the sizer grow dynamically. *vgap* and *hgap*
        define extra space between all children.
        """
        newobj = _core_.new_GridSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def SetCols(*args, **kwargs):
        """
        SetCols(self, int cols)

        Sets the number of columns in the sizer.
        """
        return _core_.GridSizer_SetCols(*args, **kwargs)

    def SetRows(*args, **kwargs):
        """
        SetRows(self, int rows)

        Sets the number of rows in the sizer.
        """
        return _core_.GridSizer_SetRows(*args, **kwargs)

    def SetVGap(*args, **kwargs):
        """
        SetVGap(self, int gap)

        Sets the vertical gap (in pixels) between the cells in the sizer.
        """
        return _core_.GridSizer_SetVGap(*args, **kwargs)

    def SetHGap(*args, **kwargs):
        """
        SetHGap(self, int gap)

        Sets the horizontal gap (in pixels) between cells in the sizer
        """
        return _core_.GridSizer_SetHGap(*args, **kwargs)

    def GetCols(*args, **kwargs):
        """
        GetCols(self) -> int

        Returns the number of columns in the sizer.
        """
        return _core_.GridSizer_GetCols(*args, **kwargs)

    def GetRows(*args, **kwargs):
        """
        GetRows(self) -> int

        Returns the number of rows in the sizer.
        """
        return _core_.GridSizer_GetRows(*args, **kwargs)

    def GetVGap(*args, **kwargs):
        """
        GetVGap(self) -> int

        Returns the vertical gap (in pixels) between the cells in the sizer.
        """
        return _core_.GridSizer_GetVGap(*args, **kwargs)

    def GetHGap(*args, **kwargs):
        """
        GetHGap(self) -> int

        Returns the horizontal gap (in pixels) between cells in the sizer.
        """
        return _core_.GridSizer_GetHGap(*args, **kwargs)


class GridSizerPtr(GridSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridSizer
_core_.GridSizer_swigregister(GridSizerPtr)

#---------------------------------------------------------------------------

FLEX_GROWMODE_NONE = _core_.FLEX_GROWMODE_NONE
FLEX_GROWMODE_SPECIFIED = _core_.FLEX_GROWMODE_SPECIFIED
FLEX_GROWMODE_ALL = _core_.FLEX_GROWMODE_ALL
class FlexGridSizer(GridSizer):
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
        __init__(self, int rows=1, int cols=0, int vgap=0, int hgap=0) -> FlexGridSizer

        Constructor for a wx.FlexGridSizer. *rows* and *cols* determine the
        number of columns and rows in the sizer - if either of the parameters
        is zero, it will be calculated to from the total number of children in
        the sizer, thus making the sizer grow dynamically. *vgap* and *hgap*
        define extra space between all children.
        """
        newobj = _core_.new_FlexGridSizer(*args, **kwargs)
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
        return _core_.FlexGridSizer_AddGrowableRow(*args, **kwargs)

    def RemoveGrowableRow(*args, **kwargs):
        """
        RemoveGrowableRow(self, size_t idx)

        Specifies that row *idx* is no longer growable.
        """
        return _core_.FlexGridSizer_RemoveGrowableRow(*args, **kwargs)

    def AddGrowableCol(*args, **kwargs):
        """
        AddGrowableCol(self, size_t idx, int proportion=0)

        Specifies that column *idx* (starting from zero) should be grown if
        there is extra space available to the sizer.

        The *proportion* parameter has the same meaning as the stretch factor
        for the box sizers except that if all proportions are 0, then all
        columns are resized equally (instead of not being resized at all).
        """
        return _core_.FlexGridSizer_AddGrowableCol(*args, **kwargs)

    def RemoveGrowableCol(*args, **kwargs):
        """
        RemoveGrowableCol(self, size_t idx)

        Specifies that column *idx* is no longer growable.
        """
        return _core_.FlexGridSizer_RemoveGrowableCol(*args, **kwargs)

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
        return _core_.FlexGridSizer_SetFlexibleDirection(*args, **kwargs)

    def GetFlexibleDirection(*args, **kwargs):
        """
        GetFlexibleDirection(self) -> int

        Returns a value that specifies whether the sizer
        flexibly resizes its columns, rows, or both (default).

        :see: `SetFlexibleDirection`
        """
        return _core_.FlexGridSizer_GetFlexibleDirection(*args, **kwargs)

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
        return _core_.FlexGridSizer_SetNonFlexibleGrowMode(*args, **kwargs)

    def GetNonFlexibleGrowMode(*args, **kwargs):
        """
        GetNonFlexibleGrowMode(self) -> int

        Returns the value that specifies how the sizer grows in the
        non-flexible direction if there is one.

        :see: `SetNonFlexibleGrowMode`
        """
        return _core_.FlexGridSizer_GetNonFlexibleGrowMode(*args, **kwargs)

    def GetRowHeights(*args, **kwargs):
        """
        GetRowHeights(self) -> list

        Returns a list of integers representing the heights of each of the
        rows in the sizer.
        """
        return _core_.FlexGridSizer_GetRowHeights(*args, **kwargs)

    def GetColWidths(*args, **kwargs):
        """
        GetColWidths(self) -> list

        Returns a list of integers representing the widths of each of the
        columns in the sizer.
        """
        return _core_.FlexGridSizer_GetColWidths(*args, **kwargs)


class FlexGridSizerPtr(FlexGridSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FlexGridSizer
_core_.FlexGridSizer_swigregister(FlexGridSizerPtr)

#---------------------------------------------------------------------------

class GBPosition(object):
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
        __init__(self, int row=0, int col=0) -> GBPosition

        This class represents the position of an item in a virtual grid of
        rows and columns managed by a `wx.GridBagSizer`.  wxPython has
        typemaps that will automatically convert from a 2-element sequence of
        integers to a wx.GBPosition, so you can use the more pythonic
        representation of the position nearly transparently in Python code.
        """
        newobj = _core_.new_GBPosition(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRow(*args, **kwargs):
        """GetRow(self) -> int"""
        return _core_.GBPosition_GetRow(*args, **kwargs)

    def GetCol(*args, **kwargs):
        """GetCol(self) -> int"""
        return _core_.GBPosition_GetCol(*args, **kwargs)

    def SetRow(*args, **kwargs):
        """SetRow(self, int row)"""
        return _core_.GBPosition_SetRow(*args, **kwargs)

    def SetCol(*args, **kwargs):
        """SetCol(self, int col)"""
        return _core_.GBPosition_SetCol(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, GBPosition other) -> bool"""
        return _core_.GBPosition___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, GBPosition other) -> bool"""
        return _core_.GBPosition___ne__(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(self, int row=0, int col=0)"""
        return _core_.GBPosition_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """Get(self) -> PyObject"""
        return _core_.GBPosition_Get(*args, **kwargs)

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


class GBPositionPtr(GBPosition):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GBPosition
_core_.GBPosition_swigregister(GBPositionPtr)

class GBSpan(object):
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
        __init__(self, int rowspan=1, int colspan=1) -> GBSpan

        Construct a new wxGBSpan, optionally setting the rowspan and
        colspan. The default is (1,1). (Meaning that the item occupies one
        cell in each direction.
        """
        newobj = _core_.new_GBSpan(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRowspan(*args, **kwargs):
        """GetRowspan(self) -> int"""
        return _core_.GBSpan_GetRowspan(*args, **kwargs)

    def GetColspan(*args, **kwargs):
        """GetColspan(self) -> int"""
        return _core_.GBSpan_GetColspan(*args, **kwargs)

    def SetRowspan(*args, **kwargs):
        """SetRowspan(self, int rowspan)"""
        return _core_.GBSpan_SetRowspan(*args, **kwargs)

    def SetColspan(*args, **kwargs):
        """SetColspan(self, int colspan)"""
        return _core_.GBSpan_SetColspan(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, GBSpan other) -> bool"""
        return _core_.GBSpan___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, GBSpan other) -> bool"""
        return _core_.GBSpan___ne__(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(self, int rowspan=1, int colspan=1)"""
        return _core_.GBSpan_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """Get(self) -> PyObject"""
        return _core_.GBSpan_Get(*args, **kwargs)

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


class GBSpanPtr(GBSpan):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GBSpan
_core_.GBSpan_swigregister(GBSpanPtr)

class GBSizerItem(SizerItem):
    """
    The wx.GBSizerItem class is used to track the additional data about
    items in a `wx.GridBagSizer` such as the item's position in the grid
    and how many rows or columns it spans.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGBSizerItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> GBSizerItem

        Constructs an empty wx.GBSizerItem.  Either a window, sizer or spacer
        size will need to be set, as well as a position and span before this
        item can be used in a Sizer.

        You will probably never need to create a wx.GBSizerItem directly as they
        are created automatically when the sizer's Add method is called.
        """
        newobj = _core_.new_GBSizerItem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPos(*args, **kwargs):
        """
        GetPos(self) -> GBPosition

        Get the grid position of the item
        """
        return _core_.GBSizerItem_GetPos(*args, **kwargs)

    def GetPosTuple(self): return self.GetPos().Get() 
    def GetSpan(*args, **kwargs):
        """
        GetSpan(self) -> GBSpan

        Get the row and column spanning of the item
        """
        return _core_.GBSizerItem_GetSpan(*args, **kwargs)

    def GetSpanTuple(self): return self.GetSpan().Get() 
    def SetPos(*args, **kwargs):
        """
        SetPos(self, GBPosition pos) -> bool

        If the item is already a member of a sizer then first ensure that
        there is no other item that would intersect with this one at the new
        position, then set the new position.  Returns True if the change is
        successful and after the next Layout() the item will be moved.
        """
        return _core_.GBSizerItem_SetPos(*args, **kwargs)

    def SetSpan(*args, **kwargs):
        """
        SetSpan(self, GBSpan span) -> bool

        If the item is already a member of a sizer then first ensure that
        there is no other item that would intersect with this one with its new
        spanning size, then set the new spanning.  Returns True if the change
        is successful and after the next Layout() the item will be resized.

        """
        return _core_.GBSizerItem_SetSpan(*args, **kwargs)

    def Intersects(*args, **kwargs):
        """
        Intersects(self, GBSizerItem other) -> bool

        Returns True if this item and the other item instersect.
        """
        return _core_.GBSizerItem_Intersects(*args, **kwargs)

    def IntersectsPos(*args, **kwargs):
        """
        IntersectsPos(self, GBPosition pos, GBSpan span) -> bool

        Returns True if the given pos/span would intersect with this item.
        """
        return _core_.GBSizerItem_IntersectsPos(*args, **kwargs)

    def GetEndPos(*args, **kwargs):
        """
        GetEndPos(self) -> GBPosition

        Get the row and column of the endpoint of this item.
        """
        return _core_.GBSizerItem_GetEndPos(*args, **kwargs)

    def GetGBSizer(*args, **kwargs):
        """
        GetGBSizer(self) -> GridBagSizer

        Get the sizer this item is a member of.
        """
        return _core_.GBSizerItem_GetGBSizer(*args, **kwargs)

    def SetGBSizer(*args, **kwargs):
        """
        SetGBSizer(self, GridBagSizer sizer)

        Set the sizer this item is a member of.
        """
        return _core_.GBSizerItem_SetGBSizer(*args, **kwargs)


class GBSizerItemPtr(GBSizerItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GBSizerItem
_core_.GBSizerItem_swigregister(GBSizerItemPtr)
DefaultSpan = cvar.DefaultSpan

def GBSizerItemWindow(*args, **kwargs):
    """
    GBSizerItemWindow(Window window, GBPosition pos, GBSpan span, int flag, 
        int border, PyObject userData=None) -> GBSizerItem

    Construct a `wx.GBSizerItem` for a window.
    """
    val = _core_.new_GBSizerItemWindow(*args, **kwargs)
    val.thisown = 1
    return val

def GBSizerItemSizer(*args, **kwargs):
    """
    GBSizerItemSizer(Sizer sizer, GBPosition pos, GBSpan span, int flag, 
        int border, PyObject userData=None) -> GBSizerItem

    Construct a `wx.GBSizerItem` for a sizer
    """
    val = _core_.new_GBSizerItemSizer(*args, **kwargs)
    val.thisown = 1
    return val

def GBSizerItemSpacer(*args, **kwargs):
    """
    GBSizerItemSpacer(int width, int height, GBPosition pos, GBSpan span, 
        int flag, int border, PyObject userData=None) -> GBSizerItem

    Construct a `wx.GBSizerItem` for a spacer.
    """
    val = _core_.new_GBSizerItemSpacer(*args, **kwargs)
    val.thisown = 1
    return val

class GridBagSizer(FlexGridSizer):
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
        __init__(self, int vgap=0, int hgap=0) -> GridBagSizer

        Constructor, with optional parameters to specify the gap between the
        rows and columns.
        """
        newobj = _core_.new_GridBagSizer(*args, **kwargs)
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
        return _core_.GridBagSizer_Add(*args, **kwargs)

    def AddItem(*args, **kwargs):
        """
        Add(self, GBSizerItem item) -> wx.GBSizerItem

        Add an item to the sizer using a `wx.GBSizerItem`.  Returns True if
        the item was successfully placed at its given cell position, False if
        something was already there.
        """
        return _core_.GridBagSizer_AddItem(*args, **kwargs)

    def GetCellSize(*args, **kwargs):
        """
        GetCellSize(self, int row, int col) -> Size

        Get the size of the specified cell, including hgap and
        vgap.  Only valid after a Layout.
        """
        return _core_.GridBagSizer_GetCellSize(*args, **kwargs)

    def GetEmptyCellSize(*args, **kwargs):
        """
        GetEmptyCellSize(self) -> Size

        Get the size used for cells in the grid with no item.
        """
        return _core_.GridBagSizer_GetEmptyCellSize(*args, **kwargs)

    def SetEmptyCellSize(*args, **kwargs):
        """
        SetEmptyCellSize(self, Size sz)

        Set the size used for cells in the grid with no item.
        """
        return _core_.GridBagSizer_SetEmptyCellSize(*args, **kwargs)

    def GetItemPosition(*args):
        """
        GetItemPosition(self, item) -> GBPosition

        Get the grid position of the specified *item* where *item* is either a
        window or subsizer that is a member of this sizer, or a zero-based
        index of an item.
        """
        return _core_.GridBagSizer_GetItemPosition(*args)

    def SetItemPosition(*args):
        """
        SetItemPosition(self, item, GBPosition pos) -> bool

        Set the grid position of the specified *item* where *item* is either a
        window or subsizer that is a member of this sizer, or a zero-based
        index of an item.  Returns True on success.  If the move is not
        allowed (because an item is already there) then False is returned.

        """
        return _core_.GridBagSizer_SetItemPosition(*args)

    def GetItemSpan(*args):
        """
        GetItemSpan(self, item) -> GBSpan

        Get the row/col spanning of the specified *item* where *item* is
        either a window or subsizer that is a member of this sizer, or a
        zero-based index of an item.
        """
        return _core_.GridBagSizer_GetItemSpan(*args)

    def SetItemSpan(*args):
        """
        SetItemSpan(self, item, GBSpan span) -> bool

        Set the row/col spanning of the specified *item* where *item* is
        either a window or subsizer that is a member of this sizer, or a
        zero-based index of an item.  Returns True on success.  If the move is
        not allowed (because an item is already there) then False is returned.
        """
        return _core_.GridBagSizer_SetItemSpan(*args)

    def FindItem(*args):
        """
        FindItem(self, item) -> GBSizerItem

        Find the sizer item for the given window or subsizer, returns None if
        not found. (non-recursive)
        """
        return _core_.GridBagSizer_FindItem(*args)

    def FindItemAtPosition(*args, **kwargs):
        """
        FindItemAtPosition(self, GBPosition pos) -> GBSizerItem

        Return the sizer item for the given grid cell, or None if there is no
        item at that position. (non-recursive)
        """
        return _core_.GridBagSizer_FindItemAtPosition(*args, **kwargs)

    def FindItemAtPoint(*args, **kwargs):
        """
        FindItemAtPoint(self, Point pt) -> GBSizerItem

        Return the sizer item located at the point given in *pt*, or None if
        there is no item at that point. The (x,y) coordinates in pt correspond
        to the client coordinates of the window using the sizer for
        layout. (non-recursive)
        """
        return _core_.GridBagSizer_FindItemAtPoint(*args, **kwargs)

    def CheckForIntersection(*args, **kwargs):
        """
        CheckForIntersection(self, GBSizerItem item, GBSizerItem excludeItem=None) -> bool

        Look at all items and see if any intersect (or would overlap) the
        given *item*.  Returns True if so, False if there would be no overlap.
        If an *excludeItem* is given then it will not be checked for
        intersection, for example it may be the item we are checking the
        position of.

        """
        return _core_.GridBagSizer_CheckForIntersection(*args, **kwargs)

    def CheckForIntersectionPos(*args, **kwargs):
        """
        CheckForIntersectionPos(self, GBPosition pos, GBSpan span, GBSizerItem excludeItem=None) -> bool

        Look at all items and see if any intersect (or would overlap) the
        given position and span.  Returns True if so, False if there would be
        no overlap.  If an *excludeItem* is given then it will not be checked
        for intersection, for example it may be the item we are checking the
        position of.
        """
        return _core_.GridBagSizer_CheckForIntersectionPos(*args, **kwargs)


class GridBagSizerPtr(GridBagSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridBagSizer
_core_.GridBagSizer_swigregister(GridBagSizerPtr)

#---------------------------------------------------------------------------

Left = _core_.Left
Top = _core_.Top
Right = _core_.Right
Bottom = _core_.Bottom
Width = _core_.Width
Height = _core_.Height
Centre = _core_.Centre
Center = _core_.Center
CentreX = _core_.CentreX
CentreY = _core_.CentreY
Unconstrained = _core_.Unconstrained
AsIs = _core_.AsIs
PercentOf = _core_.PercentOf
Above = _core_.Above
Below = _core_.Below
LeftOf = _core_.LeftOf
RightOf = _core_.RightOf
SameAs = _core_.SameAs
Absolute = _core_.Absolute
class IndividualLayoutConstraint(Object):
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
        Set(self, int rel, Window otherW, int otherE, int val=0, int marg=wxLAYOUT_DEFAULT_MARGIN)

        Sets the properties of the constraint. Normally called by one of the
        convenience functions such as Above, RightOf, SameAs.
        """
        return _core_.IndividualLayoutConstraint_Set(*args, **kwargs)

    def LeftOf(*args, **kwargs):
        """
        LeftOf(self, Window sibling, int marg=0)

        Constrains this edge to be to the left of the given window, with an
        optional margin. Implicitly, this is relative to the left edge of the
        other window.
        """
        return _core_.IndividualLayoutConstraint_LeftOf(*args, **kwargs)

    def RightOf(*args, **kwargs):
        """
        RightOf(self, Window sibling, int marg=0)

        Constrains this edge to be to the right of the given window, with an
        optional margin. Implicitly, this is relative to the right edge of the
        other window.
        """
        return _core_.IndividualLayoutConstraint_RightOf(*args, **kwargs)

    def Above(*args, **kwargs):
        """
        Above(self, Window sibling, int marg=0)

        Constrains this edge to be above the given window, with an optional
        margin. Implicitly, this is relative to the top edge of the other
        window.
        """
        return _core_.IndividualLayoutConstraint_Above(*args, **kwargs)

    def Below(*args, **kwargs):
        """
        Below(self, Window sibling, int marg=0)

        Constrains this edge to be below the given window, with an optional
        margin. Implicitly, this is relative to the bottom edge of the other
        window.
        """
        return _core_.IndividualLayoutConstraint_Below(*args, **kwargs)

    def SameAs(*args, **kwargs):
        """
        SameAs(self, Window otherW, int edge, int marg=0)

        Constrains this edge or dimension to be to the same as the edge of the
        given window, with an optional margin.
        """
        return _core_.IndividualLayoutConstraint_SameAs(*args, **kwargs)

    def PercentOf(*args, **kwargs):
        """
        PercentOf(self, Window otherW, int wh, int per)

        Constrains this edge or dimension to be to a percentage of the given
        window, with an optional margin.
        """
        return _core_.IndividualLayoutConstraint_PercentOf(*args, **kwargs)

    def Absolute(*args, **kwargs):
        """
        Absolute(self, int val)

        Constrains this edge or dimension to be the given absolute value.
        """
        return _core_.IndividualLayoutConstraint_Absolute(*args, **kwargs)

    def Unconstrained(*args, **kwargs):
        """
        Unconstrained(self)

        Sets this edge or dimension to be unconstrained, that is, dependent on
        other edges and dimensions from which this value can be deduced.
        """
        return _core_.IndividualLayoutConstraint_Unconstrained(*args, **kwargs)

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
        return _core_.IndividualLayoutConstraint_AsIs(*args, **kwargs)

    def GetOtherWindow(*args, **kwargs):
        """GetOtherWindow(self) -> Window"""
        return _core_.IndividualLayoutConstraint_GetOtherWindow(*args, **kwargs)

    def GetMyEdge(*args, **kwargs):
        """GetMyEdge(self) -> int"""
        return _core_.IndividualLayoutConstraint_GetMyEdge(*args, **kwargs)

    def SetEdge(*args, **kwargs):
        """SetEdge(self, int which)"""
        return _core_.IndividualLayoutConstraint_SetEdge(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, int v)"""
        return _core_.IndividualLayoutConstraint_SetValue(*args, **kwargs)

    def GetMargin(*args, **kwargs):
        """GetMargin(self) -> int"""
        return _core_.IndividualLayoutConstraint_GetMargin(*args, **kwargs)

    def SetMargin(*args, **kwargs):
        """SetMargin(self, int m)"""
        return _core_.IndividualLayoutConstraint_SetMargin(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> int"""
        return _core_.IndividualLayoutConstraint_GetValue(*args, **kwargs)

    def GetPercent(*args, **kwargs):
        """GetPercent(self) -> int"""
        return _core_.IndividualLayoutConstraint_GetPercent(*args, **kwargs)

    def GetOtherEdge(*args, **kwargs):
        """GetOtherEdge(self) -> int"""
        return _core_.IndividualLayoutConstraint_GetOtherEdge(*args, **kwargs)

    def GetDone(*args, **kwargs):
        """GetDone(self) -> bool"""
        return _core_.IndividualLayoutConstraint_GetDone(*args, **kwargs)

    def SetDone(*args, **kwargs):
        """SetDone(self, bool d)"""
        return _core_.IndividualLayoutConstraint_SetDone(*args, **kwargs)

    def GetRelationship(*args, **kwargs):
        """GetRelationship(self) -> int"""
        return _core_.IndividualLayoutConstraint_GetRelationship(*args, **kwargs)

    def SetRelationship(*args, **kwargs):
        """SetRelationship(self, int r)"""
        return _core_.IndividualLayoutConstraint_SetRelationship(*args, **kwargs)

    def ResetIfWin(*args, **kwargs):
        """
        ResetIfWin(self, Window otherW) -> bool

        Reset constraint if it mentions otherWin
        """
        return _core_.IndividualLayoutConstraint_ResetIfWin(*args, **kwargs)

    def SatisfyConstraint(*args, **kwargs):
        """
        SatisfyConstraint(self, LayoutConstraints constraints, Window win) -> bool

        Try to satisfy constraint
        """
        return _core_.IndividualLayoutConstraint_SatisfyConstraint(*args, **kwargs)

    def GetEdge(*args, **kwargs):
        """
        GetEdge(self, int which, Window thisWin, Window other) -> int

        Get the value of this edge or dimension, or if this
        is not determinable, -1.
        """
        return _core_.IndividualLayoutConstraint_GetEdge(*args, **kwargs)


class IndividualLayoutConstraintPtr(IndividualLayoutConstraint):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IndividualLayoutConstraint
_core_.IndividualLayoutConstraint_swigregister(IndividualLayoutConstraintPtr)

class LayoutConstraints(Object):
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
    left = property(_core_.LayoutConstraints_left_get)
    top = property(_core_.LayoutConstraints_top_get)
    right = property(_core_.LayoutConstraints_right_get)
    bottom = property(_core_.LayoutConstraints_bottom_get)
    width = property(_core_.LayoutConstraints_width_get)
    height = property(_core_.LayoutConstraints_height_get)
    centreX = property(_core_.LayoutConstraints_centreX_get)
    centreY = property(_core_.LayoutConstraints_centreY_get)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> LayoutConstraints"""
        newobj = _core_.new_LayoutConstraints(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SatisfyConstraints(*args, **kwargs):
        """SatisfyConstraints(Window win) -> (areSatisfied, noChanges)"""
        return _core_.LayoutConstraints_SatisfyConstraints(*args, **kwargs)

    def AreSatisfied(*args, **kwargs):
        """AreSatisfied(self) -> bool"""
        return _core_.LayoutConstraints_AreSatisfied(*args, **kwargs)


class LayoutConstraintsPtr(LayoutConstraints):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = LayoutConstraints
_core_.LayoutConstraints_swigregister(LayoutConstraintsPtr)

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

# Set the default string conversion encoding from the locale
import locale
default = locale.getdefaultlocale()[1]
if default:
    wx.SetDefaultPyEncoding(default)
del default
del locale

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


