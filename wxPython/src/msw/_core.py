# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

import _core_
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
DEFAULT_CONTROL_BORDER = _core_.DEFAULT_CONTROL_BORDER
DEFAULT_STATUSBAR_STYLE = _core_.DEFAULT_STATUSBAR_STYLE
TAB_TRAVERSAL = _core_.TAB_TRAVERSAL
WANTS_CHARS = _core_.WANTS_CHARS
POPUP_WINDOW = _core_.POPUP_WINDOW
CENTER_FRAME = _core_.CENTER_FRAME
CENTRE_ON_SCREEN = _core_.CENTRE_ON_SCREEN
CENTER_ON_SCREEN = _core_.CENTER_ON_SCREEN
CLIP_CHILDREN = _core_.CLIP_CHILDREN
CLIP_SIBLINGS = _core_.CLIP_SIBLINGS
WINDOW_STYLE_MASK = _core_.WINDOW_STYLE_MASK
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
RA_USE_CHECKBOX = _core_.RA_USE_CHECKBOX
RB_GROUP = _core_.RB_GROUP
RB_SINGLE = _core_.RB_SINGLE
SB_HORIZONTAL = _core_.SB_HORIZONTAL
SB_VERTICAL = _core_.SB_VERTICAL
RB_USE_CHECKBOX = _core_.RB_USE_CHECKBOX
ST_SIZEGRIP = _core_.ST_SIZEGRIP
ST_NO_AUTORESIZE = _core_.ST_NO_AUTORESIZE
ST_DOTS_MIDDLE = _core_.ST_DOTS_MIDDLE
ST_DOTS_END = _core_.ST_DOTS_END
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
SIZE_FORCE = _core_.SIZE_FORCE
PORTRAIT = _core_.PORTRAIT
LANDSCAPE = _core_.LANDSCAPE
PRINT_QUALITY_HIGH = _core_.PRINT_QUALITY_HIGH
PRINT_QUALITY_MEDIUM = _core_.PRINT_QUALITY_MEDIUM
PRINT_QUALITY_LOW = _core_.PRINT_QUALITY_LOW
PRINT_QUALITY_DRAFT = _core_.PRINT_QUALITY_DRAFT
ID_ANY = _core_.ID_ANY
ID_SEPARATOR = _core_.ID_SEPARATOR
ID_NONE = _core_.ID_NONE
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
ID_PAGE_SETUP = _core_.ID_PAGE_SETUP
ID_PREVIEW = _core_.ID_PREVIEW
ID_ABOUT = _core_.ID_ABOUT
ID_HELP_CONTENTS = _core_.ID_HELP_CONTENTS
ID_HELP_COMMANDS = _core_.ID_HELP_COMMANDS
ID_HELP_PROCEDURES = _core_.ID_HELP_PROCEDURES
ID_HELP_CONTEXT = _core_.ID_HELP_CONTEXT
ID_HELP_INDEX = _core_.ID_HELP_INDEX
ID_HELP_SEARCH = _core_.ID_HELP_SEARCH
ID_CLOSE_ALL = _core_.ID_CLOSE_ALL
ID_PREFERENCES = _core_.ID_PREFERENCES
ID_EDIT = _core_.ID_EDIT
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
ID_FILE = _core_.ID_FILE
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
STIPPLE_MASK = _core_.STIPPLE_MASK
STIPPLE_MASK_OPAQUE = _core_.STIPPLE_MASK_OPAQUE
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
WXK_COMMAND = _core_.WXK_COMMAND
WXK_SPECIAL1 = _core_.WXK_SPECIAL1
WXK_SPECIAL2 = _core_.WXK_SPECIAL2
WXK_SPECIAL3 = _core_.WXK_SPECIAL3
WXK_SPECIAL4 = _core_.WXK_SPECIAL4
WXK_SPECIAL5 = _core_.WXK_SPECIAL5
WXK_SPECIAL6 = _core_.WXK_SPECIAL6
WXK_SPECIAL7 = _core_.WXK_SPECIAL7
WXK_SPECIAL8 = _core_.WXK_SPECIAL8
WXK_SPECIAL9 = _core_.WXK_SPECIAL9
WXK_SPECIAL10 = _core_.WXK_SPECIAL10
WXK_SPECIAL11 = _core_.WXK_SPECIAL11
WXK_SPECIAL12 = _core_.WXK_SPECIAL12
WXK_SPECIAL13 = _core_.WXK_SPECIAL13
WXK_SPECIAL14 = _core_.WXK_SPECIAL14
WXK_SPECIAL15 = _core_.WXK_SPECIAL15
WXK_SPECIAL16 = _core_.WXK_SPECIAL16
WXK_SPECIAL17 = _core_.WXK_SPECIAL17
WXK_SPECIAL18 = _core_.WXK_SPECIAL18
WXK_SPECIAL19 = _core_.WXK_SPECIAL19
WXK_SPECIAL20 = _core_.WXK_SPECIAL20
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
PAPER_DBL_JAPANESE_POSTCARD = _core_.PAPER_DBL_JAPANESE_POSTCARD
PAPER_A6 = _core_.PAPER_A6
PAPER_JENV_KAKU2 = _core_.PAPER_JENV_KAKU2
PAPER_JENV_KAKU3 = _core_.PAPER_JENV_KAKU3
PAPER_JENV_CHOU3 = _core_.PAPER_JENV_CHOU3
PAPER_JENV_CHOU4 = _core_.PAPER_JENV_CHOU4
PAPER_LETTER_ROTATED = _core_.PAPER_LETTER_ROTATED
PAPER_A3_ROTATED = _core_.PAPER_A3_ROTATED
PAPER_A4_ROTATED = _core_.PAPER_A4_ROTATED
PAPER_A5_ROTATED = _core_.PAPER_A5_ROTATED
PAPER_B4_JIS_ROTATED = _core_.PAPER_B4_JIS_ROTATED
PAPER_B5_JIS_ROTATED = _core_.PAPER_B5_JIS_ROTATED
PAPER_JAPANESE_POSTCARD_ROTATED = _core_.PAPER_JAPANESE_POSTCARD_ROTATED
PAPER_DBL_JAPANESE_POSTCARD_ROTATED = _core_.PAPER_DBL_JAPANESE_POSTCARD_ROTATED
PAPER_A6_ROTATED = _core_.PAPER_A6_ROTATED
PAPER_JENV_KAKU2_ROTATED = _core_.PAPER_JENV_KAKU2_ROTATED
PAPER_JENV_KAKU3_ROTATED = _core_.PAPER_JENV_KAKU3_ROTATED
PAPER_JENV_CHOU3_ROTATED = _core_.PAPER_JENV_CHOU3_ROTATED
PAPER_JENV_CHOU4_ROTATED = _core_.PAPER_JENV_CHOU4_ROTATED
PAPER_B6_JIS = _core_.PAPER_B6_JIS
PAPER_B6_JIS_ROTATED = _core_.PAPER_B6_JIS_ROTATED
PAPER_12X11 = _core_.PAPER_12X11
PAPER_JENV_YOU4 = _core_.PAPER_JENV_YOU4
PAPER_JENV_YOU4_ROTATED = _core_.PAPER_JENV_YOU4_ROTATED
PAPER_P16K = _core_.PAPER_P16K
PAPER_P32K = _core_.PAPER_P32K
PAPER_P32KBIG = _core_.PAPER_P32KBIG
PAPER_PENV_1 = _core_.PAPER_PENV_1
PAPER_PENV_2 = _core_.PAPER_PENV_2
PAPER_PENV_3 = _core_.PAPER_PENV_3
PAPER_PENV_4 = _core_.PAPER_PENV_4
PAPER_PENV_5 = _core_.PAPER_PENV_5
PAPER_PENV_6 = _core_.PAPER_PENV_6
PAPER_PENV_7 = _core_.PAPER_PENV_7
PAPER_PENV_8 = _core_.PAPER_PENV_8
PAPER_PENV_9 = _core_.PAPER_PENV_9
PAPER_PENV_10 = _core_.PAPER_PENV_10
PAPER_P16K_ROTATED = _core_.PAPER_P16K_ROTATED
PAPER_P32K_ROTATED = _core_.PAPER_P32K_ROTATED
PAPER_P32KBIG_ROTATED = _core_.PAPER_P32KBIG_ROTATED
PAPER_PENV_1_ROTATED = _core_.PAPER_PENV_1_ROTATED
PAPER_PENV_2_ROTATED = _core_.PAPER_PENV_2_ROTATED
PAPER_PENV_3_ROTATED = _core_.PAPER_PENV_3_ROTATED
PAPER_PENV_4_ROTATED = _core_.PAPER_PENV_4_ROTATED
PAPER_PENV_5_ROTATED = _core_.PAPER_PENV_5_ROTATED
PAPER_PENV_6_ROTATED = _core_.PAPER_PENV_6_ROTATED
PAPER_PENV_7_ROTATED = _core_.PAPER_PENV_7_ROTATED
PAPER_PENV_8_ROTATED = _core_.PAPER_PENV_8_ROTATED
PAPER_PENV_9_ROTATED = _core_.PAPER_PENV_9_ROTATED
PAPER_PENV_10_ROTATED = _core_.PAPER_PENV_10_ROTATED
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
MOD_ALTGR = _core_.MOD_ALTGR
MOD_SHIFT = _core_.MOD_SHIFT
MOD_META = _core_.MOD_META
MOD_WIN = _core_.MOD_WIN
MOD_CMD = _core_.MOD_CMD
MOD_ALL = _core_.MOD_ALL
UPDATE_UI_NONE = _core_.UPDATE_UI_NONE
UPDATE_UI_RECURSE = _core_.UPDATE_UI_RECURSE
UPDATE_UI_FROMIDLE = _core_.UPDATE_UI_FROMIDLE
Layout_Default = _core_.Layout_Default
Layout_LeftToRight = _core_.Layout_LeftToRight
Layout_RightToLeft = _core_.Layout_RightToLeft
#---------------------------------------------------------------------------

class Object(object):
    """
    The base class for most wx objects, although in wxPython not
    much functionality is needed nor exposed.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
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
        val = _core_.Object_Destroy(*args, **kwargs)
        args[0].thisown = 0
        return val

    ClassName = property(GetClassName,doc="See `GetClassName`") 
_core_.Object_swigregister(Object)
_wxPySetDictionary = _core_._wxPySetDictionary
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
    something.  It simply contains integer width and height
    properties.  In most places in wxPython where a wx.Size is
    expected a (width, height) tuple can be used instead.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    width = property(_core_.Size_width_get, _core_.Size_width_set)
    height = property(_core_.Size_height_get, _core_.Size_height_set)
    x = width; y = height 
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int w=0, int h=0) -> Size

        Creates a size object.
        """
        _core_.Size_swiginit(self,_core_.new_Size(*args, **kwargs))
    __swig_destroy__ = _core_.delete_Size
    __del__ = lambda self : None;
    def __eq__(*args, **kwargs):
        """
        __eq__(self, PyObject other) -> bool

        Test for equality of wx.Size objects.
        """
        return _core_.Size___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, PyObject other) -> bool

        Test for inequality of wx.Size objects.
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

    def IncBy(*args, **kwargs):
        """IncBy(self, int dx, int dy)"""
        return _core_.Size_IncBy(*args, **kwargs)

    def DecBy(*args, **kwargs):
        """DecBy(self, int dx, int dy)"""
        return _core_.Size_DecBy(*args, **kwargs)

    def Scale(*args, **kwargs):
        """
        Scale(self, float xscale, float yscale)

        Scales the dimensions of this object by the given factors.
        """
        return _core_.Size_Scale(*args, **kwargs)

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

_core_.Size_swigregister(Size)

#---------------------------------------------------------------------------

class RealPoint(object):
    """
    A data structure for representing a point or position with floating
    point x and y properties.  In wxPython most places that expect a
    wx.RealPoint can also accept a (x,y) tuple.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    x = property(_core_.RealPoint_x_get, _core_.RealPoint_x_set)
    y = property(_core_.RealPoint_y_get, _core_.RealPoint_y_set)
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, double x=0.0, double y=0.0) -> RealPoint

        Create a wx.RealPoint object
        """
        _core_.RealPoint_swiginit(self,_core_.new_RealPoint(*args, **kwargs))
    __swig_destroy__ = _core_.delete_RealPoint
    __del__ = lambda self : None;
    def __eq__(*args, **kwargs):
        """
        __eq__(self, PyObject other) -> bool

        Test for equality of wx.RealPoint objects.
        """
        return _core_.RealPoint___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, PyObject other) -> bool

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

_core_.RealPoint_swigregister(RealPoint)

#---------------------------------------------------------------------------

class Point(object):
    """
    A data structure for representing a point or position with integer x
    and y properties.  Most places in wxPython that expect a wx.Point can
    also accept a (x,y) tuple.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    x = property(_core_.Point_x_get, _core_.Point_x_set)
    y = property(_core_.Point_y_get, _core_.Point_y_set)
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int x=0, int y=0) -> Point

        Create a wx.Point object
        """
        _core_.Point_swiginit(self,_core_.new_Point(*args, **kwargs))
    __swig_destroy__ = _core_.delete_Point
    __del__ = lambda self : None;
    def __eq__(*args, **kwargs):
        """
        __eq__(self, PyObject other) -> bool

        Test for equality of wx.Point objects.
        """
        return _core_.Point___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, PyObject other) -> bool

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

_core_.Point_swigregister(Point)

#---------------------------------------------------------------------------

class Rect(object):
    """
    A class for representing and manipulating rectangles.  It has x, y,
    width and height properties.  In wxPython most palces that expect a
    wx.Rect can also accept a (x,y,width,height) tuple.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int x=0, int y=0, int width=0, int height=0) -> Rect

        Create a new Rect object.
        """
        _core_.Rect_swiginit(self,_core_.new_Rect(*args, **kwargs))
    __swig_destroy__ = _core_.delete_Rect
    __del__ = lambda self : None;
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

    def IsEmpty(*args, **kwargs):
        """IsEmpty(self) -> bool"""
        return _core_.Rect_IsEmpty(*args, **kwargs)

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

    def GetTopRight(*args, **kwargs):
        """GetTopRight(self) -> Point"""
        return _core_.Rect_GetTopRight(*args, **kwargs)

    def SetTopRight(*args, **kwargs):
        """SetTopRight(self, Point p)"""
        return _core_.Rect_SetTopRight(*args, **kwargs)

    def GetBottomLeft(*args, **kwargs):
        """GetBottomLeft(self) -> Point"""
        return _core_.Rect_GetBottomLeft(*args, **kwargs)

    def SetBottomLeft(*args, **kwargs):
        """SetBottomLeft(self, Point p)"""
        return _core_.Rect_SetBottomLeft(*args, **kwargs)

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
        return _core_.Rect_Inflate(*args, **kwargs)

    def Deflate(*args, **kwargs):
        """
        Deflate(self, int dx, int dy) -> Rect

        Decrease the rectangle size. This method is the opposite of `Inflate`
        in that Deflate(a,b) is equivalent to Inflate(-a,-b).  Please refer to
        `Inflate` for a full description.
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

        Same as `OffsetXY` but uses dx,dy from Point
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
        __eq__(self, PyObject other) -> bool

        Test for equality of wx.Rect objects.
        """
        return _core_.Rect___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, PyObject other) -> bool

        Test for inequality of wx.Rect objects.
        """
        return _core_.Rect___ne__(*args, **kwargs)

    def ContainsXY(*args, **kwargs):
        """
        ContainsXY(self, int x, int y) -> bool

        Return True if the point is inside the rect.
        """
        return _core_.Rect_ContainsXY(*args, **kwargs)

    def Contains(*args, **kwargs):
        """
        Contains(self, Point pt) -> bool

        Return True if the point is inside the rect.
        """
        return _core_.Rect_Contains(*args, **kwargs)

    def ContainsRect(*args, **kwargs):
        """
        ContainsRect(self, Rect rect) -> bool

        Returns ``True`` if the given rectangle is completely inside this
        rectangle or touches its boundary.
        """
        return _core_.Rect_ContainsRect(*args, **kwargs)

    Inside = wx._deprecated(Contains, "Use `Contains` instead.")
    InsideXY = wx._deprecated(ContainsXY, "Use `ContainsXY` instead.")
    InsideRect = wx._deprecated(ContainsRect, "Use `ContainsRect` instead.")

    def Intersects(*args, **kwargs):
        """
        Intersects(self, Rect rect) -> bool

        Returns True if the rectangles have a non empty intersection.
        """
        return _core_.Rect_Intersects(*args, **kwargs)

    def CenterIn(*args, **kwargs):
        """
        CenterIn(self, Rect r, int dir=BOTH) -> Rect

        Center this rectangle within the one passed to the method, which is
        usually, but not necessarily, the larger one.
        """
        return _core_.Rect_CenterIn(*args, **kwargs)

    CentreIn = CenterIn 
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

    Bottom = property(GetBottom,SetBottom,doc="See `GetBottom` and `SetBottom`") 
    BottomRight = property(GetBottomRight,SetBottomRight,doc="See `GetBottomRight` and `SetBottomRight`") 
    BottomLeft = property(GetBottomLeft,SetBottomLeft,doc="See `GetBottomLeft` and `SetBottomLeft`") 
    Height = property(GetHeight,SetHeight,doc="See `GetHeight` and `SetHeight`") 
    Left = property(GetLeft,SetLeft,doc="See `GetLeft` and `SetLeft`") 
    Position = property(GetPosition,SetPosition,doc="See `GetPosition` and `SetPosition`") 
    Right = property(GetRight,SetRight,doc="See `GetRight` and `SetRight`") 
    Size = property(GetSize,SetSize,doc="See `GetSize` and `SetSize`") 
    Top = property(GetTop,SetTop,doc="See `GetTop` and `SetTop`") 
    TopLeft = property(GetTopLeft,SetTopLeft,doc="See `GetTopLeft` and `SetTopLeft`") 
    TopRight = property(GetTopRight,SetTopRight,doc="See `GetTopRight` and `SetTopRight`") 
    Width = property(GetWidth,SetWidth,doc="See `GetWidth` and `SetWidth`") 
    X = property(GetX,SetX,doc="See `GetX` and `SetX`") 
    Y = property(GetY,SetY,doc="See `GetY` and `SetY`") 
    Empty = property(IsEmpty,doc="See `IsEmpty`") 
_core_.Rect_swigregister(Rect)

def RectPP(*args, **kwargs):
    """
    RectPP(Point topLeft, Point bottomRight) -> Rect

    Create a new Rect object from Points representing two corners.
    """
    val = _core_.new_RectPP(*args, **kwargs)
    return val

def RectPS(*args, **kwargs):
    """
    RectPS(Point pos, Size size) -> Rect

    Create a new Rect from a position and size.
    """
    val = _core_.new_RectPS(*args, **kwargs)
    return val

def RectS(*args, **kwargs):
    """
    RectS(Size size) -> Rect

    Create a new Rect from a size only.
    """
    val = _core_.new_RectS(*args, **kwargs)
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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, double x=0.0, double y=0.0) -> Point2D

        Create a w.Point2D object.
        """
        _core_.Point2D_swiginit(self,_core_.new_Point2D(*args, **kwargs))
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
        __eq__(self, PyObject other) -> bool

        Test for equality of wx.Point2D objects.
        """
        return _core_.Point2D___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, PyObject other) -> bool

        Test for inequality of wx.Point2D objects.
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

    Floor = property(GetFloor,doc="See `GetFloor`") 
    Rounded = property(GetRounded,doc="See `GetRounded`") 
    VectorAngle = property(GetVectorAngle,SetVectorAngle,doc="See `GetVectorAngle` and `SetVectorAngle`") 
    VectorLength = property(GetVectorLength,SetVectorLength,doc="See `GetVectorLength` and `SetVectorLength`") 
_core_.Point2D_swigregister(Point2D)

def Point2DCopy(*args, **kwargs):
    """
    Point2DCopy(Point2D pt) -> Point2D

    Create a w.Point2D object.
    """
    val = _core_.new_Point2DCopy(*args, **kwargs)
    return val

def Point2DFromPoint(*args, **kwargs):
    """
    Point2DFromPoint(Point pt) -> Point2D

    Create a w.Point2D object.
    """
    val = _core_.new_Point2DFromPoint(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

FromStart = _core_.FromStart
FromCurrent = _core_.FromCurrent
FromEnd = _core_.FromEnd
class InputStream(object):
    """Proxy of C++ InputStream class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, PyObject p) -> InputStream"""
        _core_.InputStream_swiginit(self,_core_.new_InputStream(*args, **kwargs))
    __swig_destroy__ = _core_.delete_InputStream
    __del__ = lambda self : None;
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

_core_.InputStream_swigregister(InputStream)
DefaultPosition = cvar.DefaultPosition
DefaultSize = cvar.DefaultSize

class OutputStream(object):
    """Proxy of C++ OutputStream class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def write(*args, **kwargs):
        """write(self, PyObject obj)"""
        return _core_.OutputStream_write(*args, **kwargs)

    def LastWrite(*args, **kwargs):
        """LastWrite(self) -> size_t"""
        return _core_.OutputStream_LastWrite(*args, **kwargs)

_core_.OutputStream_swigregister(OutputStream)

#---------------------------------------------------------------------------

class FSFile(Object):
    """Proxy of C++ FSFile class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, InputStream stream, String loc, String mimetype, String anchor, 
            DateTime modif) -> FSFile
        """
        _core_.FSFile_swiginit(self,_core_.new_FSFile(*args, **kwargs))
    __swig_destroy__ = _core_.delete_FSFile
    __del__ = lambda self : None;
    def GetStream(*args, **kwargs):
        """GetStream(self) -> InputStream"""
        return _core_.FSFile_GetStream(*args, **kwargs)

    def DetachStream(*args, **kwargs):
        """DetachStream(self)"""
        return _core_.FSFile_DetachStream(*args, **kwargs)

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

    Anchor = property(GetAnchor,doc="See `GetAnchor`") 
    Location = property(GetLocation,doc="See `GetLocation`") 
    MimeType = property(GetMimeType,doc="See `GetMimeType`") 
    ModificationTime = property(GetModificationTime,doc="See `GetModificationTime`") 
    Stream = property(GetStream,doc="See `GetStream`") 
_core_.FSFile_swigregister(FSFile)

class CPPFileSystemHandler(object):
    """Proxy of C++ CPPFileSystemHandler class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _core_.delete_CPPFileSystemHandler
    __del__ = lambda self : None;
_core_.CPPFileSystemHandler_swigregister(CPPFileSystemHandler)

class FileSystemHandler(CPPFileSystemHandler):
    """Proxy of C++ FileSystemHandler class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> FileSystemHandler"""
        _core_.FileSystemHandler_swiginit(self,_core_.new_FileSystemHandler(*args, **kwargs))
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

    Anchor = property(GetAnchor,doc="See `GetAnchor`") 
    LeftLocation = property(GetLeftLocation,doc="See `GetLeftLocation`") 
    MimeTypeFromExt = property(GetMimeTypeFromExt,doc="See `GetMimeTypeFromExt`") 
    Protocol = property(GetProtocol,doc="See `GetProtocol`") 
    RightLocation = property(GetRightLocation,doc="See `GetRightLocation`") 
_core_.FileSystemHandler_swigregister(FileSystemHandler)

class FileSystem(Object):
    """Proxy of C++ FileSystem class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> FileSystem"""
        _core_.FileSystem_swiginit(self,_core_.new_FileSystem(*args, **kwargs))
    __swig_destroy__ = _core_.delete_FileSystem
    __del__ = lambda self : None;
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
    def RemoveHandler(*args, **kwargs):
        """RemoveHandler(CPPFileSystemHandler handler) -> CPPFileSystemHandler"""
        return _core_.FileSystem_RemoveHandler(*args, **kwargs)

    RemoveHandler = staticmethod(RemoveHandler)
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
    Path = property(GetPath,doc="See `GetPath`") 
_core_.FileSystem_swigregister(FileSystem)

def FileSystem_AddHandler(*args, **kwargs):
  """FileSystem_AddHandler(CPPFileSystemHandler handler)"""
  return _core_.FileSystem_AddHandler(*args, **kwargs)

def FileSystem_RemoveHandler(*args, **kwargs):
  """FileSystem_RemoveHandler(CPPFileSystemHandler handler) -> CPPFileSystemHandler"""
  return _core_.FileSystem_RemoveHandler(*args, **kwargs)

def FileSystem_CleanUpHandlers(*args):
  """FileSystem_CleanUpHandlers()"""
  return _core_.FileSystem_CleanUpHandlers(*args)

def FileSystem_FileNameToURL(*args, **kwargs):
  """FileSystem_FileNameToURL(String filename) -> String"""
  return _core_.FileSystem_FileNameToURL(*args, **kwargs)

def FileSystem_URLToFileName(*args, **kwargs):
  """FileSystem_URLToFileName(String url) -> String"""
  return _core_.FileSystem_URLToFileName(*args, **kwargs)

class InternetFSHandler(CPPFileSystemHandler):
    """Proxy of C++ InternetFSHandler class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> InternetFSHandler"""
        _core_.InternetFSHandler_swiginit(self,_core_.new_InternetFSHandler(*args, **kwargs))
    def CanOpen(*args, **kwargs):
        """CanOpen(self, String location) -> bool"""
        return _core_.InternetFSHandler_CanOpen(*args, **kwargs)

    def OpenFile(*args, **kwargs):
        """OpenFile(self, FileSystem fs, String location) -> FSFile"""
        return _core_.InternetFSHandler_OpenFile(*args, **kwargs)

_core_.InternetFSHandler_swigregister(InternetFSHandler)

class ZipFSHandler(CPPFileSystemHandler):
    """Proxy of C++ ZipFSHandler class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> ZipFSHandler"""
        _core_.ZipFSHandler_swiginit(self,_core_.new_ZipFSHandler(*args, **kwargs))
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

_core_.ZipFSHandler_swigregister(ZipFSHandler)


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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> MemoryFSHandler"""
        _core_.MemoryFSHandler_swiginit(self,_core_.new_MemoryFSHandler(*args, **kwargs))
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

_core_.MemoryFSHandler_swigregister(MemoryFSHandler)

def MemoryFSHandler_RemoveFile(*args, **kwargs):
  """MemoryFSHandler_RemoveFile(String filename)"""
  return _core_.MemoryFSHandler_RemoveFile(*args, **kwargs)

IMAGE_ALPHA_TRANSPARENT = _core_.IMAGE_ALPHA_TRANSPARENT
IMAGE_ALPHA_THRESHOLD = _core_.IMAGE_ALPHA_THRESHOLD
IMAGE_ALPHA_OPAQUE = _core_.IMAGE_ALPHA_OPAQUE
IMAGE_QUALITY_NORMAL = _core_.IMAGE_QUALITY_NORMAL
IMAGE_QUALITY_HIGH = _core_.IMAGE_QUALITY_HIGH
#---------------------------------------------------------------------------

class ImageHandler(Object):
    """
    This is the base class for implementing image file loading/saving, and
    image creation from data. It is used within `wx.Image` and is not
    normally seen by the application.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
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

    def CanReadStream(*args, **kwargs):
        """CanReadStream(self, InputStream stream) -> bool"""
        return _core_.ImageHandler_CanReadStream(*args, **kwargs)

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

    Extension = property(GetExtension,SetExtension,doc="See `GetExtension` and `SetExtension`") 
    MimeType = property(GetMimeType,SetMimeType,doc="See `GetMimeType` and `SetMimeType`") 
    Name = property(GetName,SetName,doc="See `GetName` and `SetName`") 
    Type = property(GetType,SetType,doc="See `GetType` and `SetType`") 
_core_.ImageHandler_swigregister(ImageHandler)

class PyImageHandler(ImageHandler):
    """
    This is the base class for implementing image file loading/saving, and
    image creation from data, all written in Python.  To create a custom
    image handler derive a new class from wx.PyImageHandler and provide
    the following methods::

        def DoCanRead(self, stream) --> bool
            '''Check if this handler can read the image on the stream'''

        def LoadFile(self, image, stream, verbose, index) --> bool
            '''Load image data from the stream and load it into image.'''

        def SaveFile(self, image, stream, verbose) --> bool
            '''Save the iamge data in image to the stream using
               this handler's image file format.'''

        def GetImageCount(self, stream) --> int
            '''If this image format can hold more than one image,
               how many does the image on the stream have?'''

    To activate your handler create an instance of it and pass it to
    `wx.Image_AddHandler`.  Be sure to call `SetName`, `SetType`, and
    `SetExtension` from your constructor.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> PyImageHandler

        This is the base class for implementing image file loading/saving, and
        image creation from data, all written in Python.  To create a custom
        image handler derive a new class from wx.PyImageHandler and provide
        the following methods::

            def DoCanRead(self, stream) --> bool
                '''Check if this handler can read the image on the stream'''

            def LoadFile(self, image, stream, verbose, index) --> bool
                '''Load image data from the stream and load it into image.'''

            def SaveFile(self, image, stream, verbose) --> bool
                '''Save the iamge data in image to the stream using
                   this handler's image file format.'''

            def GetImageCount(self, stream) --> int
                '''If this image format can hold more than one image,
                   how many does the image on the stream have?'''

        To activate your handler create an instance of it and pass it to
        `wx.Image_AddHandler`.  Be sure to call `SetName`, `SetType`, and
        `SetExtension` from your constructor.

        """
        _core_.PyImageHandler_swiginit(self,_core_.new_PyImageHandler(*args, **kwargs))
        self._SetSelf(self)

    def _SetSelf(*args, **kwargs):
        """_SetSelf(self, PyObject self)"""
        return _core_.PyImageHandler__SetSelf(*args, **kwargs)

_core_.PyImageHandler_swigregister(PyImageHandler)

class ImageHistogram(object):
    """Proxy of C++ ImageHistogram class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> ImageHistogram"""
        _core_.ImageHistogram_swiginit(self,_core_.new_ImageHistogram(*args, **kwargs))
    def MakeKey(*args, **kwargs):
        """
        MakeKey(byte r, byte g, byte b) -> unsigned long

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

    def GetCount(*args, **kwargs):
        """
        GetCount(self, unsigned long key) -> unsigned long

        Returns the pixel count for the given key.  Use `MakeKey` to create a
        key value from a RGB tripple.
        """
        return _core_.ImageHistogram_GetCount(*args, **kwargs)

    def GetCountRGB(*args, **kwargs):
        """
        GetCountRGB(self, byte r, byte g, byte b) -> unsigned long

        Returns the pixel count for the given RGB values.
        """
        return _core_.ImageHistogram_GetCountRGB(*args, **kwargs)

    def GetCountColour(*args, **kwargs):
        """
        GetCountColour(self, Colour colour) -> unsigned long

        Returns the pixel count for the given `wx.Colour` value.
        """
        return _core_.ImageHistogram_GetCountColour(*args, **kwargs)

_core_.ImageHistogram_swigregister(ImageHistogram)

def ImageHistogram_MakeKey(*args, **kwargs):
  """
    ImageHistogram_MakeKey(byte r, byte g, byte b) -> unsigned long

    Get the key in the histogram for the given RGB values
    """
  return _core_.ImageHistogram_MakeKey(*args, **kwargs)

class Image_RGBValue(object):
    """
    An object that contains values for red, green and blue which represent
    the value of a color. It is used by `wx.Image.HSVtoRGB` and
    `wx.Image.RGBtoHSV`, which converts between HSV color space and RGB
    color space.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, byte r=0, byte g=0, byte b=0) -> Image_RGBValue

        Constructor.
        """
        _core_.Image_RGBValue_swiginit(self,_core_.new_Image_RGBValue(*args, **kwargs))
    red = property(_core_.Image_RGBValue_red_get, _core_.Image_RGBValue_red_set)
    green = property(_core_.Image_RGBValue_green_get, _core_.Image_RGBValue_green_set)
    blue = property(_core_.Image_RGBValue_blue_get, _core_.Image_RGBValue_blue_set)
_core_.Image_RGBValue_swigregister(Image_RGBValue)

class Image_HSVValue(object):
    """
    An object that contains values for hue, saturation and value which
    represent the value of a color.  It is used by `wx.Image.HSVtoRGB` and
    `wx.Image.RGBtoHSV`, which +converts between HSV color space and RGB
    color space.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, double h=0.0, double s=0.0, double v=0.0) -> Image_HSVValue

        Constructor.
        """
        _core_.Image_HSVValue_swiginit(self,_core_.new_Image_HSVValue(*args, **kwargs))
    hue = property(_core_.Image_HSVValue_hue_get, _core_.Image_HSVValue_hue_set)
    saturation = property(_core_.Image_HSVValue_saturation_get, _core_.Image_HSVValue_saturation_set)
    value = property(_core_.Image_HSVValue_value_get, _core_.Image_HSVValue_value_set)
_core_.Image_HSVValue_swigregister(Image_HSVValue)

class Image(Object):
    """
    A platform-independent image class.  An image can be created from
    data, or using `wx.Bitmap.ConvertToImage`, or loaded from a file in a
    variety of formats.  Functions are available to set and get image
    bits, so it can be used for basic image manipulation.

    A wx.Image cannot be drawn directly to a `wx.DC`.  Instead, a
    platform-specific `wx.Bitmap` object must be created from it using the
    `wx.BitmapFromImage` constructor. This bitmap can then be drawn in a
    device context, using `wx.DC.DrawBitmap`.

    One colour value of the image may be used as a mask colour which will
    lead to the automatic creation of a `wx.Mask` object associated to the
    bitmap object.

    wx.Image supports alpha channel data, that is in addition to a byte
    for the red, green and blue colour components for each pixel it also
    stores a byte representing the pixel opacity. An alpha value of 0
    corresponds to a transparent pixel (null opacity) while a value of 255
    means that the pixel is 100% opaque.

    Unlike RGB data, not all images have an alpha channel and before using
    `GetAlpha` you should check if this image contains an alpha channel
    with `HasAlpha`. Note that currently only images loaded from PNG files
    with transparency information will have an alpha channel.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String name, long type=BITMAP_TYPE_ANY, int index=-1) -> Image

        Loads an image from a file.
        """
        _core_.Image_swiginit(self,_core_.new_Image(*args, **kwargs))
    __swig_destroy__ = _core_.delete_Image
    __del__ = lambda self : None;
    def Create(*args, **kwargs):
        """
        Create(self, int width, int height, bool clear=True)

        Creates a fresh image.  If clear is ``True``, the new image will be
        initialized to black. Otherwise, the image data will be uninitialized.
        """
        return _core_.Image_Create(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """
        Destroy(self)

        Destroys the image data.
        """
        val = _core_.Image_Destroy(*args, **kwargs)
        args[0].thisown = 0
        return val

    def Scale(*args, **kwargs):
        """
        Scale(self, int width, int height, int quality=IMAGE_QUALITY_NORMAL) -> Image

        Returns a scaled version of the image. This is also useful for scaling
        bitmaps in general as the only other way to scale bitmaps is to blit a
        `wx.MemoryDC` into another `wx.MemoryDC`.  The ``quality`` parameter
        specifies what method to use for resampling the image.  It can be
        either wx.IMAGE_QUALITY_NORMAL, which uses the normal default scaling
        method of pixel replication, or wx.IMAGE_QUALITY_HIGH which uses
        bicubic and box averaging resampling methods for upsampling and
        downsampling respectively.
        """
        return _core_.Image_Scale(*args, **kwargs)

    def ResampleBox(*args, **kwargs):
        """ResampleBox(self, int width, int height) -> Image"""
        return _core_.Image_ResampleBox(*args, **kwargs)

    def ResampleBicubic(*args, **kwargs):
        """ResampleBicubic(self, int width, int height) -> Image"""
        return _core_.Image_ResampleBicubic(*args, **kwargs)

    def Blur(*args, **kwargs):
        """
        Blur(self, int radius) -> Image

        Blurs the image in both horizontal and vertical directions by the
        specified pixel ``radius``. This should not be used when using a
        single mask colour for transparency.
        """
        return _core_.Image_Blur(*args, **kwargs)

    def BlurHorizontal(*args, **kwargs):
        """
        BlurHorizontal(self, int radius) -> Image

        Blurs the image in the horizontal direction only. This should not be
        used when using a single mask colour for transparency.

        """
        return _core_.Image_BlurHorizontal(*args, **kwargs)

    def BlurVertical(*args, **kwargs):
        """
        BlurVertical(self, int radius) -> Image

        Blurs the image in the vertical direction only. This should not be
        used when using a single mask colour for transparency.
        """
        return _core_.Image_BlurVertical(*args, **kwargs)

    def ShrinkBy(*args, **kwargs):
        """
        ShrinkBy(self, int xFactor, int yFactor) -> Image

        Return a version of the image scaled smaller by the given factors.
        """
        return _core_.Image_ShrinkBy(*args, **kwargs)

    def Rescale(*args, **kwargs):
        """
        Rescale(self, int width, int height, int quality=IMAGE_QUALITY_NORMAL) -> Image

        Changes the size of the image in-place by scaling it: after a call to
        this function, the image will have the given width and height.

        Returns the (modified) image itself.
        """
        return _core_.Image_Rescale(*args, **kwargs)

    def Resize(*args, **kwargs):
        """
        Resize(self, Size size, Point pos, int r=-1, int g=-1, int b=-1) -> Image

        Changes the size of the image in-place without scaling it, by adding
        either a border with the given colour or cropping as necessary. The
        image is pasted into a new image with the given size and background
        colour at the position pos relative to the upper left of the new
        image. If red = green = blue = -1 then use either the current mask
        colour if set or find, use, and set a suitable mask colour for any
        newly exposed areas.

        Returns the (modified) image itself.
        """
        return _core_.Image_Resize(*args, **kwargs)

    def SetRGB(*args, **kwargs):
        """
        SetRGB(self, int x, int y, byte r, byte g, byte b)

        Sets the pixel at the given coordinate. This routine performs
        bounds-checks for the coordinate so it can be considered a safe way to
        manipulate the data, but in some cases this might be too slow so that
        the data will have to be set directly. In that case you will have to
        get access to the image data using the `GetData` method.
        """
        return _core_.Image_SetRGB(*args, **kwargs)

    def SetRGBRect(*args, **kwargs):
        """
        SetRGBRect(self, Rect rect, byte r, byte g, byte b)

        Sets the colour of the pixels within the given rectangle. This routine
        performs bounds-checks for the rectangle so it can be considered a
        safe way to manipulate the data.
        """
        return _core_.Image_SetRGBRect(*args, **kwargs)

    def GetRed(*args, **kwargs):
        """
        GetRed(self, int x, int y) -> byte

        Returns the red intensity at the given coordinate.
        """
        return _core_.Image_GetRed(*args, **kwargs)

    def GetGreen(*args, **kwargs):
        """
        GetGreen(self, int x, int y) -> byte

        Returns the green intensity at the given coordinate.
        """
        return _core_.Image_GetGreen(*args, **kwargs)

    def GetBlue(*args, **kwargs):
        """
        GetBlue(self, int x, int y) -> byte

        Returns the blue intensity at the given coordinate.
        """
        return _core_.Image_GetBlue(*args, **kwargs)

    def SetAlpha(*args, **kwargs):
        """
        SetAlpha(self, int x, int y, byte alpha)

        Sets the alpha value for the given pixel. This function should only be
        called if the image has alpha channel data, use `HasAlpha` to check
        for this.
        """
        return _core_.Image_SetAlpha(*args, **kwargs)

    def GetAlpha(*args, **kwargs):
        """
        GetAlpha(self, int x, int y) -> byte

        Returns the alpha value for the given pixel. This function may only be
        called for the images with alpha channel, use `HasAlpha` to check for
        this.

        The returned value is the *opacity* of the image, i.e. the value of 0
        corresponds to the fully transparent pixels while the value of 255 to
        the fully opaque pixels.
        """
        return _core_.Image_GetAlpha(*args, **kwargs)

    def HasAlpha(*args, **kwargs):
        """
        HasAlpha(self) -> bool

        Returns true if this image has alpha channel, false otherwise.
        """
        return _core_.Image_HasAlpha(*args, **kwargs)

    def InitAlpha(*args, **kwargs):
        """
        InitAlpha(self)

        Initializes the image alpha channel data. It is an error to call it if
        the image already has alpha data. If it doesn't, alpha data will be by
        default initialized to all pixels being fully opaque. But if the image
        has a a mask colour, all mask pixels will be completely transparent.
        """
        return _core_.Image_InitAlpha(*args, **kwargs)

    def IsTransparent(*args, **kwargs):
        """
        IsTransparent(self, int x, int y, byte threshold=IMAGE_ALPHA_THRESHOLD) -> bool

        Returns ``True`` if this pixel is masked or has an alpha value less
        than the spcified threshold.
        """
        return _core_.Image_IsTransparent(*args, **kwargs)

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
        ConvertAlphaToMask(self, byte threshold=IMAGE_ALPHA_THRESHOLD) -> bool

        If the image has alpha channel, this method converts it to mask. All
        pixels with alpha value less than ``threshold`` are replaced with the
        mask colour and the alpha channel is removed. The mask colour is
        chosen automatically using `FindFirstUnusedColour`.

        If the image image doesn't have alpha channel, ConvertAlphaToMask does
        nothing.
        """
        return _core_.Image_ConvertAlphaToMask(*args, **kwargs)

    def ConvertColourToAlpha(*args, **kwargs):
        """
        ConvertColourToAlpha(self, byte r, byte g, byte b) -> bool

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
        """
        SetMaskFromImage(self, Image mask, byte mr, byte mg, byte mb) -> bool

        Sets the image's mask so that the pixels that have RGB value of
        ``(mr,mg,mb)`` in ``mask`` will be masked in this image. This is done
        by first finding an unused colour in the image, setting this colour as
        the mask colour and then using this colour to draw all pixels in the
        image who corresponding pixel in mask has given RGB value.

        Returns ``False`` if ``mask`` does not have same dimensions as the
        image or if there is no unused colour left. Returns ``True`` if the
        mask was successfully applied.

        Note that this method involves computing the histogram, which is
        computationally intensive operation.
        """
        return _core_.Image_SetMaskFromImage(*args, **kwargs)

    def CanRead(*args, **kwargs):
        """
        CanRead(String filename) -> bool

        Returns True if the image handlers can read this file.
        """
        return _core_.Image_CanRead(*args, **kwargs)

    CanRead = staticmethod(CanRead)
    def GetImageCount(*args, **kwargs):
        """
        GetImageCount(String filename, long type=BITMAP_TYPE_ANY) -> int

        If the image file contains more than one image and the image handler
        is capable of retrieving these individually, this function will return
        the number of available images.
        """
        return _core_.Image_GetImageCount(*args, **kwargs)

    GetImageCount = staticmethod(GetImageCount)
    def LoadFile(*args, **kwargs):
        """
        LoadFile(self, String name, long type=BITMAP_TYPE_ANY, int index=-1) -> bool

        Loads an image from a file. If no handler type is provided, the
        library will try to autodetect the format.
        """
        return _core_.Image_LoadFile(*args, **kwargs)

    def LoadMimeFile(*args, **kwargs):
        """
        LoadMimeFile(self, String name, String mimetype, int index=-1) -> bool

        Loads an image from a file, specifying the image type with a MIME type
        string.
        """
        return _core_.Image_LoadMimeFile(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """
        SaveFile(self, String name, int type) -> bool

        Saves an image in the named file.
        """
        return _core_.Image_SaveFile(*args, **kwargs)

    def SaveMimeFile(*args, **kwargs):
        """
        SaveMimeFile(self, String name, String mimetype) -> bool

        Saves an image in the named file.
        """
        return _core_.Image_SaveMimeFile(*args, **kwargs)

    def CanReadStream(*args, **kwargs):
        """
        CanReadStream(InputStream stream) -> bool

        Returns True if the image handlers can read an image file from the
        data currently on the input stream, or a readable Python file-like
        object.
        """
        return _core_.Image_CanReadStream(*args, **kwargs)

    CanReadStream = staticmethod(CanReadStream)
    def LoadStream(*args, **kwargs):
        """
        LoadStream(self, InputStream stream, long type=BITMAP_TYPE_ANY, int index=-1) -> bool

        Loads an image from an input stream or a readable Python file-like
        object. If no handler type is provided, the library will try to
        autodetect the format.
        """
        return _core_.Image_LoadStream(*args, **kwargs)

    def LoadMimeStream(*args, **kwargs):
        """
        LoadMimeStream(self, InputStream stream, String mimetype, int index=-1) -> bool

        Loads an image from an input stream or a readable Python file-like
        object, using a MIME type string to specify the image file format.
        """
        return _core_.Image_LoadMimeStream(*args, **kwargs)

    def Ok(*args, **kwargs):
        """
        Ok(self) -> bool

        Returns true if image data is present.
        """
        return _core_.Image_Ok(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """
        GetWidth(self) -> int

        Gets the width of the image in pixels.
        """
        return _core_.Image_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """
        GetHeight(self) -> int

        Gets the height of the image in pixels.
        """
        return _core_.Image_GetHeight(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> Size

        Returns the size of the image in pixels.
        """
        return _core_.Image_GetSize(*args, **kwargs)

    def GetSubImage(*args, **kwargs):
        """
        GetSubImage(self, Rect rect) -> Image

        Returns a sub image of the current one as long as the rect belongs
        entirely to the image.
        """
        return _core_.Image_GetSubImage(*args, **kwargs)

    def Size(*args, **kwargs):
        """
        Size(self, Size size, Point pos, int r=-1, int g=-1, int b=-1) -> Image

        Returns a resized version of this image without scaling it by adding
        either a border with the given colour or cropping as necessary. The
        image is pasted into a new image with the given size and background
        colour at the position ``pos`` relative to the upper left of the new
        image. If red = green = blue = -1 then use either the current mask
        colour if set or find, use, and set a suitable mask colour for any
        newly exposed areas.
        """
        return _core_.Image_Size(*args, **kwargs)

    def Copy(*args, **kwargs):
        """
        Copy(self) -> Image

        Returns an identical copy of the image.
        """
        return _core_.Image_Copy(*args, **kwargs)

    def Paste(*args, **kwargs):
        """
        Paste(self, Image image, int x, int y)

        Pastes ``image`` into this instance and takes care of the mask colour
        and any out of bounds problems.
        """
        return _core_.Image_Paste(*args, **kwargs)

    def GetData(*args, **kwargs):
        """
        GetData(self) -> PyObject

        Returns a string containing a copy of the RGB bytes of the image.
        """
        return _core_.Image_GetData(*args, **kwargs)

    def SetData(*args, **kwargs):
        """
        SetData(self, buffer data)

        Resets the Image's RGB data from a buffer of RGB bytes.  Accepts
        either a string or a buffer object holding the data and the length of
        the data must be width*height*3.
        """
        return _core_.Image_SetData(*args, **kwargs)

    def GetDataBuffer(*args, **kwargs):
        """
        GetDataBuffer(self) -> PyObject

        Returns a writable Python buffer object that is pointing at the RGB
        image data buffer inside the wx.Image. You need to ensure that you do
        not use this buffer object after the image has been destroyed.
        """
        return _core_.Image_GetDataBuffer(*args, **kwargs)

    def SetDataBuffer(*args, **kwargs):
        """
        SetDataBuffer(self, buffer data)

        Sets the internal image data pointer to point at a Python buffer
        object.  This can save making an extra copy of the data but you must
        ensure that the buffer object lives longer than the wx.Image does.
        """
        return _core_.Image_SetDataBuffer(*args, **kwargs)

    def GetAlphaData(*args, **kwargs):
        """
        GetAlphaData(self) -> PyObject

        Returns a string containing a copy of the alpha bytes of the image.
        """
        return _core_.Image_GetAlphaData(*args, **kwargs)

    def SetAlphaData(*args, **kwargs):
        """
        SetAlphaData(self, buffer alpha)

        Resets the Image's alpha data from a buffer of bytes.  Accepts either
        a string or a buffer object holding the data and the length of the
        data must be width*height.
        """
        return _core_.Image_SetAlphaData(*args, **kwargs)

    def GetAlphaBuffer(*args, **kwargs):
        """
        GetAlphaBuffer(self) -> PyObject

        Returns a writable Python buffer object that is pointing at the Alpha
        data buffer inside the wx.Image. You need to ensure that you do not
        use this buffer object after the image has been destroyed.
        """
        return _core_.Image_GetAlphaBuffer(*args, **kwargs)

    def SetAlphaBuffer(*args, **kwargs):
        """
        SetAlphaBuffer(self, buffer alpha)

        Sets the internal image alpha pointer to point at a Python buffer
        object.  This can save making an extra copy of the data but you must
        ensure that the buffer object lives as long as the wx.Image does.
        """
        return _core_.Image_SetAlphaBuffer(*args, **kwargs)

    def SetMaskColour(*args, **kwargs):
        """
        SetMaskColour(self, byte r, byte g, byte b)

        Sets the mask colour for this image (and tells the image to use the
        mask).
        """
        return _core_.Image_SetMaskColour(*args, **kwargs)

    def GetOrFindMaskColour(*args, **kwargs):
        """
        GetOrFindMaskColour() -> (r,g,b)

        Get the current mask colour or find a suitable colour.
        """
        return _core_.Image_GetOrFindMaskColour(*args, **kwargs)

    def GetMaskRed(*args, **kwargs):
        """
        GetMaskRed(self) -> byte

        Gets the red component of the mask colour.
        """
        return _core_.Image_GetMaskRed(*args, **kwargs)

    def GetMaskGreen(*args, **kwargs):
        """
        GetMaskGreen(self) -> byte

        Gets the green component of the mask colour.
        """
        return _core_.Image_GetMaskGreen(*args, **kwargs)

    def GetMaskBlue(*args, **kwargs):
        """
        GetMaskBlue(self) -> byte

        Gets the blue component of the mask colour.
        """
        return _core_.Image_GetMaskBlue(*args, **kwargs)

    def SetMask(*args, **kwargs):
        """
        SetMask(self, bool mask=True)

        Specifies whether there is a mask or not. The area of the mask is
        determined by the current mask colour.
        """
        return _core_.Image_SetMask(*args, **kwargs)

    def HasMask(*args, **kwargs):
        """
        HasMask(self) -> bool

        Returns ``True`` if there is a mask active, ``False`` otherwise.
        """
        return _core_.Image_HasMask(*args, **kwargs)

    def Rotate(*args, **kwargs):
        """
        Rotate(self, double angle, Point centre_of_rotation, bool interpolating=True, 
            Point offset_after_rotation=None) -> Image

        Rotates the image about the given point, by ``angle`` radians. Passing
        ``True`` to ``interpolating`` results in better image quality, but is
        slower. If the image has a mask, then the mask colour is used for the
        uncovered pixels in the rotated image background. Otherwise, black
        will be used as the fill colour.

        Returns the rotated image, leaving this image intact.
        """
        return _core_.Image_Rotate(*args, **kwargs)

    def Rotate90(*args, **kwargs):
        """
        Rotate90(self, bool clockwise=True) -> Image

        Returns a copy of the image rotated 90 degrees in the direction
        indicated by ``clockwise``.
        """
        return _core_.Image_Rotate90(*args, **kwargs)

    def Mirror(*args, **kwargs):
        """
        Mirror(self, bool horizontally=True) -> Image

        Returns a mirrored copy of the image. The parameter ``horizontally``
        indicates the orientation.
        """
        return _core_.Image_Mirror(*args, **kwargs)

    def Replace(*args, **kwargs):
        """
        Replace(self, byte r1, byte g1, byte b1, byte r2, byte g2, byte b2)

        Replaces the colour specified by ``(r1,g1,b1)`` by the colour
        ``(r2,g2,b2)``.
        """
        return _core_.Image_Replace(*args, **kwargs)

    def ConvertToGreyscale(*args, **kwargs):
        """
        ConvertToGreyscale(self, double lr=0.299, double lg=0.587, double lb=0.114) -> Image

        Convert to greyscale image. Uses the luminance component (Y) of the
        image.  The luma value (YUV) is calculated using (R * lr) + (G * lg) + (B * lb),
        defaults to ITU-T BT.601
        """
        return _core_.Image_ConvertToGreyscale(*args, **kwargs)

    def ConvertToMono(*args, **kwargs):
        """
        ConvertToMono(self, byte r, byte g, byte b) -> Image

        Returns monochromatic version of the image. The returned image has
        white colour where the original has ``(r,g,b)`` colour and black
        colour everywhere else.
        """
        return _core_.Image_ConvertToMono(*args, **kwargs)

    def SetOption(*args, **kwargs):
        """
        SetOption(self, String name, String value)

        Sets an image handler defined option.  For example, when saving as a
        JPEG file, the option ``wx.IMAGE_OPTION_QUALITY`` is used, which is a
        number between 0 and 100 (0 is terrible, 100 is very good).
        """
        return _core_.Image_SetOption(*args, **kwargs)

    def SetOptionInt(*args, **kwargs):
        """
        SetOptionInt(self, String name, int value)

        Sets an image option as an integer.
        """
        return _core_.Image_SetOptionInt(*args, **kwargs)

    def GetOption(*args, **kwargs):
        """
        GetOption(self, String name) -> String

        Gets the value of an image handler option.
        """
        return _core_.Image_GetOption(*args, **kwargs)

    def GetOptionInt(*args, **kwargs):
        """
        GetOptionInt(self, String name) -> int

        Gets the value of an image handler option as an integer.  If the given
        option is not present, the function returns 0.
        """
        return _core_.Image_GetOptionInt(*args, **kwargs)

    def HasOption(*args, **kwargs):
        """
        HasOption(self, String name) -> bool

        Returns true if the given option is present.
        """
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
    def GetHandlers(*args, **kwargs):
        """GetHandlers() -> PyObject"""
        return _core_.Image_GetHandlers(*args, **kwargs)

    GetHandlers = staticmethod(GetHandlers)
    def GetImageExtWildcard(*args, **kwargs):
        """
        GetImageExtWildcard() -> String

        Iterates all registered wxImageHandler objects, and returns a string
        containing file extension masks suitable for passing to file open/save
        dialog boxes.
        """
        return _core_.Image_GetImageExtWildcard(*args, **kwargs)

    GetImageExtWildcard = staticmethod(GetImageExtWildcard)
    def ConvertToBitmap(*args, **kwargs):
        """ConvertToBitmap(self, int depth=-1) -> Bitmap"""
        return _core_.Image_ConvertToBitmap(*args, **kwargs)

    def ConvertToMonoBitmap(*args, **kwargs):
        """ConvertToMonoBitmap(self, byte red, byte green, byte blue) -> Bitmap"""
        return _core_.Image_ConvertToMonoBitmap(*args, **kwargs)

    def RotateHue(*args, **kwargs):
        """
        RotateHue(self, double angle)

        Rotates the hue of each pixel of the image. Hue is a double in the
        range -1.0..1.0 where -1.0 is -360 degrees and 1.0 is 360 degrees
        """
        return _core_.Image_RotateHue(*args, **kwargs)

    def RGBtoHSV(*args, **kwargs):
        """
        RGBtoHSV(Image_RGBValue rgb) -> Image_HSVValue

        Converts a color in RGB color space to HSV color space.
        """
        return _core_.Image_RGBtoHSV(*args, **kwargs)

    RGBtoHSV = staticmethod(RGBtoHSV)
    def HSVtoRGB(*args, **kwargs):
        """
        HSVtoRGB(Image_HSVValue hsv) -> Image_RGBValue

        Converts a color in HSV color space to RGB color space.
        """
        return _core_.Image_HSVtoRGB(*args, **kwargs)

    HSVtoRGB = staticmethod(HSVtoRGB)
    def __nonzero__(self): return self.Ok() 
    AlphaBuffer = property(GetAlphaBuffer,SetAlphaBuffer,doc="See `GetAlphaBuffer` and `SetAlphaBuffer`") 
    AlphaData = property(GetAlphaData,SetAlphaData,doc="See `GetAlphaData` and `SetAlphaData`") 
    Data = property(GetData,SetData,doc="See `GetData` and `SetData`") 
    DataBuffer = property(GetDataBuffer,SetDataBuffer,doc="See `GetDataBuffer` and `SetDataBuffer`") 
    Height = property(GetHeight,doc="See `GetHeight`") 
    MaskBlue = property(GetMaskBlue,doc="See `GetMaskBlue`") 
    MaskGreen = property(GetMaskGreen,doc="See `GetMaskGreen`") 
    MaskRed = property(GetMaskRed,doc="See `GetMaskRed`") 
    Size = property(GetSize,doc="See `GetSize`") 
    Width = property(GetWidth,doc="See `GetWidth`") 
_core_.Image_swigregister(Image)

def ImageFromMime(*args, **kwargs):
    """
    ImageFromMime(String name, String mimetype, int index=-1) -> Image

    Loads an image from a file, using a MIME type string (such as
    'image/jpeg') to specify image type.
    """
    val = _core_.new_ImageFromMime(*args, **kwargs)
    return val

def ImageFromStream(*args, **kwargs):
    """
    ImageFromStream(InputStream stream, long type=BITMAP_TYPE_ANY, int index=-1) -> Image

    Loads an image from an input stream, or any readable Python file-like
    object.
    """
    val = _core_.new_ImageFromStream(*args, **kwargs)
    return val

def ImageFromStreamMime(*args, **kwargs):
    """
    ImageFromStreamMime(InputStream stream, String mimetype, int index=-1) -> Image

    Loads an image from an input stream, or any readable Python file-like
    object, specifying the image format with a MIME type string.
    """
    val = _core_.new_ImageFromStreamMime(*args, **kwargs)
    return val

def EmptyImage(*args, **kwargs):
    """
    EmptyImage(int width=0, int height=0, bool clear=True) -> Image

    Construct an empty image of a given size, optionally setting all
    pixels to black.
    """
    val = _core_.new_EmptyImage(*args, **kwargs)
    return val

def ImageFromBitmap(*args, **kwargs):
    """
    ImageFromBitmap(Bitmap bitmap) -> Image

    Construct an Image from a `wx.Bitmap`.
    """
    val = _core_.new_ImageFromBitmap(*args, **kwargs)
    return val

def ImageFromData(*args, **kwargs):
    """
    ImageFromData(int width, int height, buffer data) -> Image

    Construct an Image from a buffer of RGB bytes.  Accepts either a
    string or a buffer object holding the data and the length of the data
    must be width*height*3.
    """
    val = _core_.new_ImageFromData(*args, **kwargs)
    return val

def ImageFromDataWithAlpha(*args, **kwargs):
    """
    ImageFromDataWithAlpha(int width, int height, buffer data, buffer alpha) -> Image

    Construct an Image from a buffer of RGB bytes with an Alpha channel.
    Accepts either a string or a buffer object holding the data and the
    length of the data must be width*height*3 bytes, and the length of the
    alpha data must be width*height bytes.
    """
    val = _core_.new_ImageFromDataWithAlpha(*args, **kwargs)
    return val

def Image_CanRead(*args, **kwargs):
  """
    Image_CanRead(String filename) -> bool

    Returns True if the image handlers can read this file.
    """
  return _core_.Image_CanRead(*args, **kwargs)

def Image_GetImageCount(*args, **kwargs):
  """
    Image_GetImageCount(String filename, long type=BITMAP_TYPE_ANY) -> int

    If the image file contains more than one image and the image handler
    is capable of retrieving these individually, this function will return
    the number of available images.
    """
  return _core_.Image_GetImageCount(*args, **kwargs)

def Image_CanReadStream(*args, **kwargs):
  """
    Image_CanReadStream(InputStream stream) -> bool

    Returns True if the image handlers can read an image file from the
    data currently on the input stream, or a readable Python file-like
    object.
    """
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

def Image_GetHandlers(*args):
  """Image_GetHandlers() -> PyObject"""
  return _core_.Image_GetHandlers(*args)

def Image_GetImageExtWildcard(*args):
  """
    Image_GetImageExtWildcard() -> String

    Iterates all registered wxImageHandler objects, and returns a string
    containing file extension masks suitable for passing to file open/save
    dialog boxes.
    """
  return _core_.Image_GetImageExtWildcard(*args)

def Image_RGBtoHSV(*args, **kwargs):
  """
    Image_RGBtoHSV(Image_RGBValue rgb) -> Image_HSVValue

    Converts a color in RGB color space to HSV color space.
    """
  return _core_.Image_RGBtoHSV(*args, **kwargs)

def Image_HSVtoRGB(*args, **kwargs):
  """
    Image_HSVtoRGB(Image_HSVValue hsv) -> Image_RGBValue

    Converts a color in HSV color space to RGB color space.
    """
  return _core_.Image_HSVtoRGB(*args, **kwargs)


def _ImageFromBuffer(*args, **kwargs):
  """_ImageFromBuffer(int width, int height, buffer data, buffer alpha=None) -> Image"""
  return _core_._ImageFromBuffer(*args, **kwargs)
def ImageFromBuffer(width, height, dataBuffer, alphaBuffer=None):
    """
    Creates a `wx.Image` from the data in dataBuffer.  The dataBuffer
    parameter must be a Python object that implements the buffer interface,
    such as a string, array, etc.  The dataBuffer object is expected to
    contain a series of RGB bytes and be width*height*3 bytes long.  A buffer
    object can optionally be supplied for the image's alpha channel data, and
    it is expected to be width*height bytes long.

    The wx.Image will be created with its data and alpha pointers initialized
    to the memory address pointed to by the buffer objects, thus saving the
    time needed to copy the image data from the buffer object to the wx.Image.
    While this has advantages, it also has the shoot-yourself-in-the-foot
    risks associated with sharing a C pointer between two objects.

    To help alleviate the risk a reference to the data and alpha buffer
    objects are kept with the wx.Image, so that they won't get deleted until
    after the wx.Image is deleted.  However please be aware that it is not
    guaranteed that an object won't move its memory buffer to a new location
    when it needs to resize its contents.  If that happens then the wx.Image
    will end up referring to an invalid memory location and could cause the
    application to crash.  Therefore care should be taken to not manipulate
    the objects used for the data and alpha buffers in a way that would cause
    them to change size.
    """
    image = _core_._ImageFromBuffer(width, height, dataBuffer, alphaBuffer)
    image._buffer = dataBuffer
    image._alpha = alphaBuffer
    return image

def InitAllImageHandlers():
    """
    The former functionality of InitAllImageHanders is now done internal to
    the _core_ extension module and so this function has become a simple NOP.
    """
    pass

IMAGE_RESOLUTION_INCHES = _core_.IMAGE_RESOLUTION_INCHES
IMAGE_RESOLUTION_CM = _core_.IMAGE_RESOLUTION_CM
PNG_TYPE_COLOUR = _core_.PNG_TYPE_COLOUR
PNG_TYPE_GREY = _core_.PNG_TYPE_GREY
PNG_TYPE_GREY_RED = _core_.PNG_TYPE_GREY_RED
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
    """A `wx.ImageHandler` for \*.bmp bitmap files."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> BMPHandler

        A `wx.ImageHandler` for \*.bmp bitmap files.
        """
        _core_.BMPHandler_swiginit(self,_core_.new_BMPHandler(*args, **kwargs))
_core_.BMPHandler_swigregister(BMPHandler)
NullImage = cvar.NullImage
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

class ICOHandler(BMPHandler):
    """A `wx.ImageHandler` for \*.ico icon files."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> ICOHandler

        A `wx.ImageHandler` for \*.ico icon files.
        """
        _core_.ICOHandler_swiginit(self,_core_.new_ICOHandler(*args, **kwargs))
_core_.ICOHandler_swigregister(ICOHandler)

class CURHandler(ICOHandler):
    """A `wx.ImageHandler` for \*.cur cursor files."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> CURHandler

        A `wx.ImageHandler` for \*.cur cursor files.
        """
        _core_.CURHandler_swiginit(self,_core_.new_CURHandler(*args, **kwargs))
_core_.CURHandler_swigregister(CURHandler)

class ANIHandler(CURHandler):
    """A `wx.ImageHandler` for \*.ani animated cursor files."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> ANIHandler

        A `wx.ImageHandler` for \*.ani animated cursor files.
        """
        _core_.ANIHandler_swiginit(self,_core_.new_ANIHandler(*args, **kwargs))
_core_.ANIHandler_swigregister(ANIHandler)

class PNGHandler(ImageHandler):
    """A `wx.ImageHandler` for PNG image files."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> PNGHandler

        A `wx.ImageHandler` for PNG image files.
        """
        _core_.PNGHandler_swiginit(self,_core_.new_PNGHandler(*args, **kwargs))
_core_.PNGHandler_swigregister(PNGHandler)

class GIFHandler(ImageHandler):
    """A `wx.ImageHandler` for GIF image files."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> GIFHandler

        A `wx.ImageHandler` for GIF image files.
        """
        _core_.GIFHandler_swiginit(self,_core_.new_GIFHandler(*args, **kwargs))
_core_.GIFHandler_swigregister(GIFHandler)

class PCXHandler(ImageHandler):
    """A `wx.ImageHandler` for PCX imager files."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> PCXHandler

        A `wx.ImageHandler` for PCX imager files.
        """
        _core_.PCXHandler_swiginit(self,_core_.new_PCXHandler(*args, **kwargs))
_core_.PCXHandler_swigregister(PCXHandler)

class JPEGHandler(ImageHandler):
    """A `wx.ImageHandler` for JPEG/JPG image files."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> JPEGHandler

        A `wx.ImageHandler` for JPEG/JPG image files.
        """
        _core_.JPEGHandler_swiginit(self,_core_.new_JPEGHandler(*args, **kwargs))
_core_.JPEGHandler_swigregister(JPEGHandler)

class PNMHandler(ImageHandler):
    """A `wx.ImageHandler` for PNM image files."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> PNMHandler

        A `wx.ImageHandler` for PNM image files.
        """
        _core_.PNMHandler_swiginit(self,_core_.new_PNMHandler(*args, **kwargs))
_core_.PNMHandler_swigregister(PNMHandler)

class XPMHandler(ImageHandler):
    """A `wx.ImageHandler` for XPM image."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> XPMHandler

        A `wx.ImageHandler` for XPM image.
        """
        _core_.XPMHandler_swiginit(self,_core_.new_XPMHandler(*args, **kwargs))
_core_.XPMHandler_swigregister(XPMHandler)

class TIFFHandler(ImageHandler):
    """A `wx.ImageHandler` for TIFF image files."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> TIFFHandler

        A `wx.ImageHandler` for TIFF image files.
        """
        _core_.TIFFHandler_swiginit(self,_core_.new_TIFFHandler(*args, **kwargs))
_core_.TIFFHandler_swigregister(TIFFHandler)

QUANTIZE_INCLUDE_WINDOWS_COLOURS = _core_.QUANTIZE_INCLUDE_WINDOWS_COLOURS
QUANTIZE_FILL_DESTINATION_IMAGE = _core_.QUANTIZE_FILL_DESTINATION_IMAGE
class Quantize(object):
    """Performs quantization, or colour reduction, on a wxImage."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Quantize(*args, **kwargs):
        """
        Quantize(Image src, Image dest, int desiredNoColours=236, int flags=wxQUANTIZE_INCLUDE_WINDOWS_COLOURS|wxQUANTIZE_FILL_DESTINATION_IMAGE) -> bool

        Reduce the colours in the source image and put the result into the
        destination image, setting the palette in the destination if
        needed. Both images may be the same, to overwrite the source image.
        """
        return _core_.Quantize_Quantize(*args, **kwargs)

    Quantize = staticmethod(Quantize)
_core_.Quantize_swigregister(Quantize)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> EvtHandler"""
        _core_.EvtHandler_swiginit(self,_core_.new_EvtHandler(*args, **kwargs))
        self._setOORInfo(self)

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
        """Disconnect(self, int id, int lastId=-1, EventType eventType=wxEVT_NULL) -> bool"""
        return _core_.EvtHandler_Disconnect(*args, **kwargs)

    def _setOORInfo(*args, **kwargs):
        """_setOORInfo(self, PyObject _self, bool incref=True)"""
        val = _core_.EvtHandler__setOORInfo(*args, **kwargs)
        args[0].thisown = 0
        return val

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

    EvtHandlerEnabled = property(GetEvtHandlerEnabled,SetEvtHandlerEnabled,doc="See `GetEvtHandlerEnabled` and `SetEvtHandlerEnabled`") 
    NextHandler = property(GetNextHandler,SetNextHandler,doc="See `GetNextHandler` and `SetNextHandler`") 
    PreviousHandler = property(GetPreviousHandler,SetPreviousHandler,doc="See `GetPreviousHandler` and `SetPreviousHandler`") 
_core_.EvtHandler_swigregister(EvtHandler)

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

def NewEventType(*args):
  """NewEventType() -> EventType"""
  return _core_.NewEventType(*args)
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
wxEVT_SCROLL_CHANGED = _core_.wxEVT_SCROLL_CHANGED
wxEVT_SCROLL_ENDSCROLL = wxEVT_SCROLL_CHANGED 
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
wxEVT_ACTIVATE = _core_.wxEVT_ACTIVATE
wxEVT_CREATE = _core_.wxEVT_CREATE
wxEVT_DESTROY = _core_.wxEVT_DESTROY
wxEVT_SHOW = _core_.wxEVT_SHOW
wxEVT_ICONIZE = _core_.wxEVT_ICONIZE
wxEVT_MAXIMIZE = _core_.wxEVT_MAXIMIZE
wxEVT_MOUSE_CAPTURE_CHANGED = _core_.wxEVT_MOUSE_CAPTURE_CHANGED
wxEVT_MOUSE_CAPTURE_LOST = _core_.wxEVT_MOUSE_CAPTURE_LOST
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
wxEVT_HIBERNATE = _core_.wxEVT_HIBERNATE
wxEVT_COMMAND_TEXT_COPY = _core_.wxEVT_COMMAND_TEXT_COPY
wxEVT_COMMAND_TEXT_CUT = _core_.wxEVT_COMMAND_TEXT_CUT
wxEVT_COMMAND_TEXT_PASTE = _core_.wxEVT_COMMAND_TEXT_PASTE
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
EVT_MOUSE_CAPTURE_LOST = wx.PyEventBinder( wxEVT_MOUSE_CAPTURE_LOST )         

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

# Scrolling from wx.Slider and wx.ScrollBar
EVT_SCROLL = wx.PyEventBinder([ wxEVT_SCROLL_TOP,
                               wxEVT_SCROLL_BOTTOM,
                               wxEVT_SCROLL_LINEUP,
                               wxEVT_SCROLL_LINEDOWN,
                               wxEVT_SCROLL_PAGEUP,
                               wxEVT_SCROLL_PAGEDOWN,
                               wxEVT_SCROLL_THUMBTRACK,
                               wxEVT_SCROLL_THUMBRELEASE,
                               wxEVT_SCROLL_CHANGED,
                               ])

EVT_SCROLL_TOP = wx.PyEventBinder( wxEVT_SCROLL_TOP )
EVT_SCROLL_BOTTOM = wx.PyEventBinder( wxEVT_SCROLL_BOTTOM )
EVT_SCROLL_LINEUP = wx.PyEventBinder( wxEVT_SCROLL_LINEUP )
EVT_SCROLL_LINEDOWN = wx.PyEventBinder( wxEVT_SCROLL_LINEDOWN )
EVT_SCROLL_PAGEUP = wx.PyEventBinder( wxEVT_SCROLL_PAGEUP )
EVT_SCROLL_PAGEDOWN = wx.PyEventBinder( wxEVT_SCROLL_PAGEDOWN )
EVT_SCROLL_THUMBTRACK = wx.PyEventBinder( wxEVT_SCROLL_THUMBTRACK )
EVT_SCROLL_THUMBRELEASE = wx.PyEventBinder( wxEVT_SCROLL_THUMBRELEASE )
EVT_SCROLL_CHANGED = wx.PyEventBinder( wxEVT_SCROLL_CHANGED )
EVT_SCROLL_ENDSCROLL = EVT_SCROLL_CHANGED

# Scrolling from wx.Slider and wx.ScrollBar, with an id
EVT_COMMAND_SCROLL = wx.PyEventBinder([ wxEVT_SCROLL_TOP,
                                       wxEVT_SCROLL_BOTTOM,
                                       wxEVT_SCROLL_LINEUP,
                                       wxEVT_SCROLL_LINEDOWN,
                                       wxEVT_SCROLL_PAGEUP,
                                       wxEVT_SCROLL_PAGEDOWN,
                                       wxEVT_SCROLL_THUMBTRACK,
                                       wxEVT_SCROLL_THUMBRELEASE,
                                       wxEVT_SCROLL_CHANGED,
                                       ], 1)

EVT_COMMAND_SCROLL_TOP = wx.PyEventBinder( wxEVT_SCROLL_TOP, 1)
EVT_COMMAND_SCROLL_BOTTOM = wx.PyEventBinder( wxEVT_SCROLL_BOTTOM, 1)
EVT_COMMAND_SCROLL_LINEUP = wx.PyEventBinder( wxEVT_SCROLL_LINEUP, 1)
EVT_COMMAND_SCROLL_LINEDOWN = wx.PyEventBinder( wxEVT_SCROLL_LINEDOWN, 1)
EVT_COMMAND_SCROLL_PAGEUP = wx.PyEventBinder( wxEVT_SCROLL_PAGEUP, 1)
EVT_COMMAND_SCROLL_PAGEDOWN = wx.PyEventBinder( wxEVT_SCROLL_PAGEDOWN, 1)
EVT_COMMAND_SCROLL_THUMBTRACK = wx.PyEventBinder( wxEVT_SCROLL_THUMBTRACK, 1)
EVT_COMMAND_SCROLL_THUMBRELEASE = wx.PyEventBinder( wxEVT_SCROLL_THUMBRELEASE, 1)
EVT_COMMAND_SCROLL_CHANGED = wx.PyEventBinder( wxEVT_SCROLL_CHANGED, 1)
EVT_COMMAND_SCROLL_ENDSCROLL = EVT_COMMAND_SCROLL_CHANGED

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

EVT_TEXT_CUT   =  wx.PyEventBinder( wxEVT_COMMAND_TEXT_CUT )
EVT_TEXT_COPY  =  wx.PyEventBinder( wxEVT_COMMAND_TEXT_COPY )
EVT_TEXT_PASTE =  wx.PyEventBinder( wxEVT_COMMAND_TEXT_PASTE )


#---------------------------------------------------------------------------

class Event(Object):
    """
    An event is a structure holding information about an event passed to a
    callback or member function. wx.Event is an abstract base class for
    other event classes
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _core_.delete_Event
    __del__ = lambda self : None;
    def SetEventType(*args, **kwargs):
        """
        SetEventType(self, EventType typ)

        Sets the specific type of the event.
        """
        return _core_.Event_SetEventType(*args, **kwargs)

    def GetEventType(*args, **kwargs):
        """
        GetEventType(self) -> EventType

        Returns the identifier of the given event type, such as
        ``wxEVT_COMMAND_BUTTON_CLICKED``.
        """
        return _core_.Event_GetEventType(*args, **kwargs)

    def GetEventObject(*args, **kwargs):
        """
        GetEventObject(self) -> Object

        Returns the object (usually a window) associated with the event, if
        any.
        """
        return _core_.Event_GetEventObject(*args, **kwargs)

    def SetEventObject(*args, **kwargs):
        """
        SetEventObject(self, Object obj)

        Sets the originating object, or in other words, obj is normally the
        object that is sending the event.
        """
        return _core_.Event_SetEventObject(*args, **kwargs)

    def GetTimestamp(*args, **kwargs):
        """GetTimestamp(self) -> long"""
        return _core_.Event_GetTimestamp(*args, **kwargs)

    def SetTimestamp(*args, **kwargs):
        """SetTimestamp(self, long ts=0)"""
        return _core_.Event_SetTimestamp(*args, **kwargs)

    def GetId(*args, **kwargs):
        """
        GetId(self) -> int

        Returns the identifier associated with this event, such as a button
        command id.
        """
        return _core_.Event_GetId(*args, **kwargs)

    def SetId(*args, **kwargs):
        """
        SetId(self, int Id)

        Set's the ID for the event.  This is usually the ID of the window that
        is sending the event, but it can also be a command id from a menu
        item, etc.
        """
        return _core_.Event_SetId(*args, **kwargs)

    def IsCommandEvent(*args, **kwargs):
        """
        IsCommandEvent(self) -> bool

        Returns true if the event is or is derived from `wx.CommandEvent` else
        it returns false. Note: Exists only for optimization purposes.
        """
        return _core_.Event_IsCommandEvent(*args, **kwargs)

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
        return _core_.Event_Skip(*args, **kwargs)

    def GetSkipped(*args, **kwargs):
        """
        GetSkipped(self) -> bool

        Returns true if the event handler should be skipped, false otherwise.
        :see: `Skip`
        """
        return _core_.Event_GetSkipped(*args, **kwargs)

    def ShouldPropagate(*args, **kwargs):
        """
        ShouldPropagate(self) -> bool

        Test if this event should be propagated to the parent window or not,
        i.e. if the propagation level is currently greater than 0.
        """
        return _core_.Event_ShouldPropagate(*args, **kwargs)

    def StopPropagation(*args, **kwargs):
        """
        StopPropagation(self) -> int

        Stop the event from propagating to its parent window.  Returns the old
        propagation level value which may be later passed to
        `ResumePropagation` to allow propagating the event again.
        """
        return _core_.Event_StopPropagation(*args, **kwargs)

    def ResumePropagation(*args, **kwargs):
        """
        ResumePropagation(self, int propagationLevel)

        Resume the event propagation by restoring the propagation level.  (For
        example, you can use the value returned by an earlier call to
        `StopPropagation`.)

        """
        return _core_.Event_ResumePropagation(*args, **kwargs)

    def Clone(*args, **kwargs):
        """Clone(self) -> Event"""
        return _core_.Event_Clone(*args, **kwargs)

    EventObject = property(GetEventObject,SetEventObject,doc="See `GetEventObject` and `SetEventObject`") 
    EventType = property(GetEventType,SetEventType,doc="See `GetEventType` and `SetEventType`") 
    Id = property(GetId,SetId,doc="See `GetId` and `SetId`") 
    Skipped = property(GetSkipped,doc="See `GetSkipped`") 
    Timestamp = property(GetTimestamp,SetTimestamp,doc="See `GetTimestamp` and `SetTimestamp`") 
_core_.Event_swigregister(Event)

#---------------------------------------------------------------------------

class PropagationDisabler(object):
    """
    Helper class to temporarily change an event not to propagate.  Simply
    create an instance of this class and then whe it is destroyed the
    propogation of the event will be restored.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Event event) -> PropagationDisabler

        Helper class to temporarily change an event not to propagate.  Simply
        create an instance of this class and then whe it is destroyed the
        propogation of the event will be restored.
        """
        _core_.PropagationDisabler_swiginit(self,_core_.new_PropagationDisabler(*args, **kwargs))
    __swig_destroy__ = _core_.delete_PropagationDisabler
    __del__ = lambda self : None;
_core_.PropagationDisabler_swigregister(PropagationDisabler)

class PropagateOnce(object):
    """
    A helper class that will temporarily lower propagation level of an
    event.  Simply create an instance of this class and then whe it is
    destroyed the propogation of the event will be restored.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Event event) -> PropagateOnce

        A helper class that will temporarily lower propagation level of an
        event.  Simply create an instance of this class and then whe it is
        destroyed the propogation of the event will be restored.
        """
        _core_.PropagateOnce_swiginit(self,_core_.new_PropagateOnce(*args, **kwargs))
    __swig_destroy__ = _core_.delete_PropagateOnce
    __del__ = lambda self : None;
_core_.PropagateOnce_swigregister(PropagateOnce)

#---------------------------------------------------------------------------

class CommandEvent(Event):
    """
    This event class contains information about command events, which
    originate from a variety of simple controls, as well as menus and
    toolbars.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int winid=0) -> CommandEvent

        This event class contains information about command events, which
        originate from a variety of simple controls, as well as menus and
        toolbars.
        """
        _core_.CommandEvent_swiginit(self,_core_.new_CommandEvent(*args, **kwargs))
    def GetSelection(*args, **kwargs):
        """
        GetSelection(self) -> int

        Returns item index for a listbox or choice selection event (not valid
        for a deselection).
        """
        return _core_.CommandEvent_GetSelection(*args, **kwargs)

    def SetString(*args, **kwargs):
        """SetString(self, String s)"""
        return _core_.CommandEvent_SetString(*args, **kwargs)

    def GetString(*args, **kwargs):
        """
        GetString(self) -> String

        Returns item string for a listbox or choice selection event (not valid
        for a deselection).
        """
        return _core_.CommandEvent_GetString(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """
        IsChecked(self) -> bool

        This method can be used with checkbox and menu events: for the
        checkboxes, the method returns true for a selection event and false
        for a deselection one. For the menu events, this method indicates if
        the menu item just has become checked or unchecked (and thus only
        makes sense for checkable menu items).
        """
        return _core_.CommandEvent_IsChecked(*args, **kwargs)

    Checked = IsChecked 
    def IsSelection(*args, **kwargs):
        """
        IsSelection(self) -> bool

        For a listbox or similar event, returns true if it is a selection,
        false if it is a deselection.
        """
        return _core_.CommandEvent_IsSelection(*args, **kwargs)

    def SetExtraLong(*args, **kwargs):
        """SetExtraLong(self, long extraLong)"""
        return _core_.CommandEvent_SetExtraLong(*args, **kwargs)

    def GetExtraLong(*args, **kwargs):
        """
        GetExtraLong(self) -> long

        Returns extra information dependant on the event objects type. If the
        event comes from a listbox selection, it is a boolean determining
        whether the event was a selection (true) or a deselection (false). A
        listbox deselection only occurs for multiple-selection boxes, and in
        this case the index and string values are indeterminate and the
        listbox must be examined by the application.
        """
        return _core_.CommandEvent_GetExtraLong(*args, **kwargs)

    def SetInt(*args, **kwargs):
        """SetInt(self, int i)"""
        return _core_.CommandEvent_SetInt(*args, **kwargs)

    def GetInt(*args, **kwargs):
        """
        GetInt(self) -> int

        Returns the integer identifier corresponding to a listbox, choice or
        radiobox selection (only if the event was a selection, not a
        deselection), or a boolean value representing the value of a checkbox.
        """
        return _core_.CommandEvent_GetInt(*args, **kwargs)

    def GetClientData(*args, **kwargs):
        """
        GetClientData(self) -> PyObject

        Returns the client data object for a listbox or choice selection event, (if any.)
        """
        return _core_.CommandEvent_GetClientData(*args, **kwargs)

    def SetClientData(*args, **kwargs):
        """
        SetClientData(self, PyObject clientData)

        Associate the given client data with the item at position n.
        """
        return _core_.CommandEvent_SetClientData(*args, **kwargs)

    GetClientObject = GetClientData
    SetClientObject = SetClientData

    def Clone(*args, **kwargs):
        """Clone(self) -> Event"""
        return _core_.CommandEvent_Clone(*args, **kwargs)

    ClientData = property(GetClientData,SetClientData,doc="See `GetClientData` and `SetClientData`") 
    ClientObject = property(GetClientObject,SetClientObject,doc="See `GetClientObject` and `SetClientObject`") 
    ExtraLong = property(GetExtraLong,SetExtraLong,doc="See `GetExtraLong` and `SetExtraLong`") 
    Int = property(GetInt,SetInt,doc="See `GetInt` and `SetInt`") 
    Selection = property(GetSelection,doc="See `GetSelection`") 
    String = property(GetString,SetString,doc="See `GetString` and `SetString`") 
_core_.CommandEvent_swigregister(CommandEvent)

#---------------------------------------------------------------------------

class NotifyEvent(CommandEvent):
    """
    An instance of this class (or one of its derived classes) is sent from
    a control when the control's state is being changed and the control
    allows that change to be prevented from happening.  The event handler
    can call `Veto` or `Allow` to tell the control what to do.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int winid=0) -> NotifyEvent

        An instance of this class (or one of its derived classes) is sent from
        a control when the control's state is being changed and the control
        allows that change to be prevented from happening.  The event handler
        can call `Veto` or `Allow` to tell the control what to do.
        """
        _core_.NotifyEvent_swiginit(self,_core_.new_NotifyEvent(*args, **kwargs))
    def Veto(*args, **kwargs):
        """
        Veto(self)

        Prevents the change announced by this event from happening.

        It is in general a good idea to notify the user about the reasons for
        vetoing the change because otherwise the applications behaviour (which
        just refuses to do what the user wants) might be quite surprising.
        """
        return _core_.NotifyEvent_Veto(*args, **kwargs)

    def Allow(*args, **kwargs):
        """
        Allow(self)

        This is the opposite of `Veto`: it explicitly allows the event to be
        processed. For most events it is not necessary to call this method as
        the events are allowed anyhow but some are forbidden by default (this
        will be mentioned in the corresponding event description).
        """
        return _core_.NotifyEvent_Allow(*args, **kwargs)

    def IsAllowed(*args, **kwargs):
        """
        IsAllowed(self) -> bool

        Returns true if the change is allowed (`Veto` hasn't been called) or
        false otherwise (if it was).
        """
        return _core_.NotifyEvent_IsAllowed(*args, **kwargs)

_core_.NotifyEvent_swigregister(NotifyEvent)

#---------------------------------------------------------------------------

class ScrollEvent(CommandEvent):
    """
    A scroll event holds information about events sent from stand-alone
    scrollbars and sliders. Note that scrolled windows do not send
    instnaces of this event class, but send the `wx.ScrollWinEvent`
    instead.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int winid=0, int pos=0, 
            int orient=0) -> ScrollEvent
        """
        _core_.ScrollEvent_swiginit(self,_core_.new_ScrollEvent(*args, **kwargs))
    def GetOrientation(*args, **kwargs):
        """
        GetOrientation(self) -> int

        Returns wx.HORIZONTAL or wx.VERTICAL, depending on the orientation of
        the scrollbar.
        """
        return _core_.ScrollEvent_GetOrientation(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> int

        Returns the position of the scrollbar.
        """
        return _core_.ScrollEvent_GetPosition(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(self, int orient)"""
        return _core_.ScrollEvent_SetOrientation(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, int pos)"""
        return _core_.ScrollEvent_SetPosition(*args, **kwargs)

    Orientation = property(GetOrientation,SetOrientation,doc="See `GetOrientation` and `SetOrientation`") 
    Position = property(GetPosition,SetPosition,doc="See `GetPosition` and `SetPosition`") 
_core_.ScrollEvent_swigregister(ScrollEvent)

#---------------------------------------------------------------------------

class ScrollWinEvent(Event):
    """
    A wx.ScrollWinEvent holds information about scrolling and is sent from
    scrolling windows.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int pos=0, int orient=0) -> ScrollWinEvent

        A wx.ScrollWinEvent holds information about scrolling and is sent from
        scrolling windows.
        """
        _core_.ScrollWinEvent_swiginit(self,_core_.new_ScrollWinEvent(*args, **kwargs))
    def GetOrientation(*args, **kwargs):
        """
        GetOrientation(self) -> int

        Returns wx.HORIZONTAL or wx.VERTICAL, depending on the orientation of
        the scrollbar.
        """
        return _core_.ScrollWinEvent_GetOrientation(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> int

        Returns the position of the scrollbar for the thumb track and release
        events. Note that this field can't be used for the other events, you
        need to query the window itself for the current position in that case.
        """
        return _core_.ScrollWinEvent_GetPosition(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(self, int orient)"""
        return _core_.ScrollWinEvent_SetOrientation(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, int pos)"""
        return _core_.ScrollWinEvent_SetPosition(*args, **kwargs)

    Orientation = property(GetOrientation,SetOrientation,doc="See `GetOrientation` and `SetOrientation`") 
    Position = property(GetPosition,SetPosition,doc="See `GetPosition` and `SetPosition`") 
_core_.ScrollWinEvent_swigregister(ScrollWinEvent)

#---------------------------------------------------------------------------

MOUSE_BTN_ANY = _core_.MOUSE_BTN_ANY
MOUSE_BTN_NONE = _core_.MOUSE_BTN_NONE
MOUSE_BTN_LEFT = _core_.MOUSE_BTN_LEFT
MOUSE_BTN_MIDDLE = _core_.MOUSE_BTN_MIDDLE
MOUSE_BTN_RIGHT = _core_.MOUSE_BTN_RIGHT
class MouseEvent(Event):
    """
    This event class contains information about the events generated by
    the mouse: they include mouse buttons press and release events and
    mouse move events.

    All mouse events involving the buttons use ``wx.MOUSE_BTN_LEFT`` for
    the left mouse button, ``wx.MOUSE_BTN_MIDDLE`` for the middle one and
    ``wx.MOUSE_BTN_RIGHT`` for the right one. Note that not all mice have
    a middle button so a portable application should avoid relying on the
    events from it.

    Note the difference between methods like `LeftDown` and `LeftIsDown`:
    the former returns true when the event corresponds to the left mouse
    button click while the latter returns true if the left mouse button is
    currently being pressed. For example, when the user is dragging the
    mouse you can use `LeftIsDown` to test whether the left mouse button
    is (still) depressed. Also, by convention, if `LeftDown` returns true,
    `LeftIsDown` will also return true in wxWidgets whatever the
    underlying GUI behaviour is (which is platform-dependent). The same
    applies, of course, to other mouse buttons as well.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType mouseType=wxEVT_NULL) -> MouseEvent

        Constructs a wx.MouseEvent.  Valid event types are:

            * wxEVT_ENTER_WINDOW
            * wxEVT_LEAVE_WINDOW
            * wxEVT_LEFT_DOWN
            * wxEVT_LEFT_UP
            * wxEVT_LEFT_DCLICK
            * wxEVT_MIDDLE_DOWN
            * wxEVT_MIDDLE_UP
            * wxEVT_MIDDLE_DCLICK
            * wxEVT_RIGHT_DOWN
            * wxEVT_RIGHT_UP
            * wxEVT_RIGHT_DCLICK
            * wxEVT_MOTION
            * wxEVT_MOUSEWHEEL 
        """
        _core_.MouseEvent_swiginit(self,_core_.new_MouseEvent(*args, **kwargs))
    def IsButton(*args, **kwargs):
        """
        IsButton(self) -> bool

        Returns true if the event was a mouse button event (not necessarily a
        button down event - that may be tested using `ButtonDown`).
        """
        return _core_.MouseEvent_IsButton(*args, **kwargs)

    def ButtonDown(*args, **kwargs):
        """
        ButtonDown(self, int but=MOUSE_BTN_ANY) -> bool

        If the argument is omitted, this returns true if the event was any
        mouse button down event. Otherwise the argument specifies which
        button-down event shold be checked for (see `Button` for the possible
        values).
        """
        return _core_.MouseEvent_ButtonDown(*args, **kwargs)

    def ButtonDClick(*args, **kwargs):
        """
        ButtonDClick(self, int but=MOUSE_BTN_ANY) -> bool

        If the argument is omitted, this returns true if the event was any
        mouse double click event. Otherwise the argument specifies which
        double click event to check for (see `Button` for the possible
        values).
        """
        return _core_.MouseEvent_ButtonDClick(*args, **kwargs)

    def ButtonUp(*args, **kwargs):
        """
        ButtonUp(self, int but=MOUSE_BTN_ANY) -> bool

        If the argument is omitted, this returns true if the event was any
        mouse button up event. Otherwise the argument specifies which button
        up event to check for (see `Button` for the possible values).
        """
        return _core_.MouseEvent_ButtonUp(*args, **kwargs)

    def Button(*args, **kwargs):
        """
        Button(self, int button) -> bool

        Returns true if the identified mouse button is changing state. Valid
        values of button are:

             ====================      =====================================
             wx.MOUSE_BTN_LEFT         check if left button was pressed
             wx.MOUSE_BTN_MIDDLE       check if middle button was pressed
             wx.MOUSE_BTN_RIGHT        check if right button was pressed
             wx.MOUSE_BTN_ANY          check if any button was pressed
             ====================      =====================================

        """
        return _core_.MouseEvent_Button(*args, **kwargs)

    def ButtonIsDown(*args, **kwargs):
        """ButtonIsDown(self, int but) -> bool"""
        return _core_.MouseEvent_ButtonIsDown(*args, **kwargs)

    def GetButton(*args, **kwargs):
        """
        GetButton(self) -> int

        Returns the mouse button which generated this event or
        wx.MOUSE_BTN_NONE if no button is involved (for mouse move, enter or
        leave event, for example). Otherwise wx.MOUSE_BTN_LEFT is returned for
        the left button down, up and double click events, wx.MOUSE_BTN_MIDDLE
        and wx.MOUSE_BTN_RIGHT for the same events for the middle and the
        right buttons respectively.
        """
        return _core_.MouseEvent_GetButton(*args, **kwargs)

    def ControlDown(*args, **kwargs):
        """
        ControlDown(self) -> bool

        Returns true if the control key was down at the time of the event.
        """
        return _core_.MouseEvent_ControlDown(*args, **kwargs)

    def MetaDown(*args, **kwargs):
        """
        MetaDown(self) -> bool

        Returns true if the Meta key was down at the time of the event.
        """
        return _core_.MouseEvent_MetaDown(*args, **kwargs)

    def AltDown(*args, **kwargs):
        """
        AltDown(self) -> bool

        Returns true if the Alt key was down at the time of the event.
        """
        return _core_.MouseEvent_AltDown(*args, **kwargs)

    def ShiftDown(*args, **kwargs):
        """
        ShiftDown(self) -> bool

        Returns true if the Shift key was down at the time of the event.
        """
        return _core_.MouseEvent_ShiftDown(*args, **kwargs)

    def CmdDown(*args, **kwargs):
        """
        CmdDown(self) -> bool

        "Cmd" is a pseudo key which is the same as Control for PC and Unix
        platforms but the special "Apple" (a.k.a as "Command") key on
        Macs. It often makes sense to use it instead of, say, `ControlDown`
        because Cmd key is used for the same thing under Mac as Ctrl
        elsewhere. The Ctrl key still exists, it's just not used for this
        purpose. So for non-Mac platforms this is the same as `ControlDown`
        and Macs this is the same as `MetaDown`.
        """
        return _core_.MouseEvent_CmdDown(*args, **kwargs)

    def LeftDown(*args, **kwargs):
        """
        LeftDown(self) -> bool

        Returns true if the left mouse button state changed to down.
        """
        return _core_.MouseEvent_LeftDown(*args, **kwargs)

    def MiddleDown(*args, **kwargs):
        """
        MiddleDown(self) -> bool

        Returns true if the middle mouse button state changed to down.
        """
        return _core_.MouseEvent_MiddleDown(*args, **kwargs)

    def RightDown(*args, **kwargs):
        """
        RightDown(self) -> bool

        Returns true if the right mouse button state changed to down.
        """
        return _core_.MouseEvent_RightDown(*args, **kwargs)

    def LeftUp(*args, **kwargs):
        """
        LeftUp(self) -> bool

        Returns true if the left mouse button state changed to up.
        """
        return _core_.MouseEvent_LeftUp(*args, **kwargs)

    def MiddleUp(*args, **kwargs):
        """
        MiddleUp(self) -> bool

        Returns true if the middle mouse button state changed to up.
        """
        return _core_.MouseEvent_MiddleUp(*args, **kwargs)

    def RightUp(*args, **kwargs):
        """
        RightUp(self) -> bool

        Returns true if the right mouse button state changed to up.
        """
        return _core_.MouseEvent_RightUp(*args, **kwargs)

    def LeftDClick(*args, **kwargs):
        """
        LeftDClick(self) -> bool

        Returns true if the event was a left button double click.
        """
        return _core_.MouseEvent_LeftDClick(*args, **kwargs)

    def MiddleDClick(*args, **kwargs):
        """
        MiddleDClick(self) -> bool

        Returns true if the event was a middle button double click.
        """
        return _core_.MouseEvent_MiddleDClick(*args, **kwargs)

    def RightDClick(*args, **kwargs):
        """
        RightDClick(self) -> bool

        Returns true if the event was a right button double click.
        """
        return _core_.MouseEvent_RightDClick(*args, **kwargs)

    def LeftIsDown(*args, **kwargs):
        """
        LeftIsDown(self) -> bool

        Returns true if the left mouse button is currently down, independent
        of the current event type.

        Please notice that it is not the same as LeftDown which returns true
        if the left mouse button was just pressed. Rather, it describes the
        state of the mouse button before the event happened.

        This event is usually used in the mouse event handlers which process
        "move mouse" messages to determine whether the user is (still)
        dragging the mouse.
        """
        return _core_.MouseEvent_LeftIsDown(*args, **kwargs)

    def MiddleIsDown(*args, **kwargs):
        """
        MiddleIsDown(self) -> bool

        Returns true if the middle mouse button is currently down, independent
        of the current event type.
        """
        return _core_.MouseEvent_MiddleIsDown(*args, **kwargs)

    def RightIsDown(*args, **kwargs):
        """
        RightIsDown(self) -> bool

        Returns true if the right mouse button is currently down, independent
        of the current event type.
        """
        return _core_.MouseEvent_RightIsDown(*args, **kwargs)

    def Dragging(*args, **kwargs):
        """
        Dragging(self) -> bool

        Returns true if this was a dragging event (motion while a button is
        depressed).
        """
        return _core_.MouseEvent_Dragging(*args, **kwargs)

    def Moving(*args, **kwargs):
        """
        Moving(self) -> bool

        Returns true if this was a motion event and no mouse buttons were
        pressed. If any mouse button is held pressed, then this method returns
        false and Dragging returns true.
        """
        return _core_.MouseEvent_Moving(*args, **kwargs)

    def Entering(*args, **kwargs):
        """
        Entering(self) -> bool

        Returns true if the mouse was entering the window.
        """
        return _core_.MouseEvent_Entering(*args, **kwargs)

    def Leaving(*args, **kwargs):
        """
        Leaving(self) -> bool

        Returns true if the mouse was leaving the window.
        """
        return _core_.MouseEvent_Leaving(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Returns the pixel position of the mouse in window coordinates when the
        event happened.
        """
        return _core_.MouseEvent_GetPosition(*args, **kwargs)

    def GetPositionTuple(*args, **kwargs):
        """
        GetPositionTuple() -> (x,y)

        Returns the pixel position of the mouse in window coordinates when the
        event happened.
        """
        return _core_.MouseEvent_GetPositionTuple(*args, **kwargs)

    def GetLogicalPosition(*args, **kwargs):
        """
        GetLogicalPosition(self, DC dc) -> Point

        Returns the logical mouse position in pixels (i.e. translated
        according to the translation set for the DC, which usually indicates
        that the window has been scrolled).
        """
        return _core_.MouseEvent_GetLogicalPosition(*args, **kwargs)

    def GetX(*args, **kwargs):
        """
        GetX(self) -> int

        Returns X coordinate of the physical mouse event position.
        """
        return _core_.MouseEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """
        GetY(self) -> int

        Returns Y coordinate of the physical mouse event position.
        """
        return _core_.MouseEvent_GetY(*args, **kwargs)

    def GetWheelRotation(*args, **kwargs):
        """
        GetWheelRotation(self) -> int

        Get wheel rotation, positive or negative indicates direction of
        rotation. Current devices all send an event when rotation is equal to
        +/-WheelDelta, but this allows for finer resolution devices to be
        created in the future. Because of this you shouldn't assume that one
        event is equal to 1 line or whatever, but you should be able to either
        do partial line scrolling or wait until +/-WheelDelta rotation values
        have been accumulated before scrolling.
        """
        return _core_.MouseEvent_GetWheelRotation(*args, **kwargs)

    def GetWheelDelta(*args, **kwargs):
        """
        GetWheelDelta(self) -> int

        Get wheel delta, normally 120. This is the threshold for action to be
        taken, and one such action (for example, scrolling one increment)
        should occur for each delta.
        """
        return _core_.MouseEvent_GetWheelDelta(*args, **kwargs)

    def GetLinesPerAction(*args, **kwargs):
        """
        GetLinesPerAction(self) -> int

        Returns the configured number of lines (or whatever) to be scrolled
        per wheel action. Defaults to three.
        """
        return _core_.MouseEvent_GetLinesPerAction(*args, **kwargs)

    def IsPageScroll(*args, **kwargs):
        """
        IsPageScroll(self) -> bool

        Returns true if the system has been setup to do page scrolling with
        the mouse wheel instead of line scrolling.
        """
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
    Button = property(GetButton,doc="See `GetButton`") 
    LinesPerAction = property(GetLinesPerAction,doc="See `GetLinesPerAction`") 
    LogicalPosition = property(GetLogicalPosition,doc="See `GetLogicalPosition`") 
    Position = property(GetPosition,doc="See `GetPosition`") 
    WheelDelta = property(GetWheelDelta,doc="See `GetWheelDelta`") 
    WheelRotation = property(GetWheelRotation,doc="See `GetWheelRotation`") 
    X = property(GetX,doc="See `GetX`") 
    Y = property(GetY,doc="See `GetY`") 
_core_.MouseEvent_swigregister(MouseEvent)

#---------------------------------------------------------------------------

class SetCursorEvent(Event):
    """
    A SetCursorEvent is generated when the mouse cursor is about to be set
    as a result of mouse motion. This event gives the application the
    chance to perform specific mouse cursor processing based on the
    current position of the mouse within the window. Use the `SetCursor`
    method to specify the cursor you want to be displayed.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int x=0, int y=0) -> SetCursorEvent

        Construct a new `wx.SetCursorEvent`.
        """
        _core_.SetCursorEvent_swiginit(self,_core_.new_SetCursorEvent(*args, **kwargs))
    def GetX(*args, **kwargs):
        """
        GetX(self) -> int

        Returns the X coordinate of the mouse in client coordinates.
        """
        return _core_.SetCursorEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """
        GetY(self) -> int

        Returns the Y coordinate of the mouse in client coordinates.
        """
        return _core_.SetCursorEvent_GetY(*args, **kwargs)

    def SetCursor(*args, **kwargs):
        """
        SetCursor(self, Cursor cursor)

        Sets the cursor associated with this event.
        """
        return _core_.SetCursorEvent_SetCursor(*args, **kwargs)

    def GetCursor(*args, **kwargs):
        """
        GetCursor(self) -> Cursor

        Returns a reference to the cursor specified by this event.
        """
        return _core_.SetCursorEvent_GetCursor(*args, **kwargs)

    def HasCursor(*args, **kwargs):
        """
        HasCursor(self) -> bool

        Returns true if the cursor specified by this event is a valid cursor.
        """
        return _core_.SetCursorEvent_HasCursor(*args, **kwargs)

    Cursor = property(GetCursor,SetCursor,doc="See `GetCursor` and `SetCursor`") 
    X = property(GetX,doc="See `GetX`") 
    Y = property(GetY,doc="See `GetY`") 
_core_.SetCursorEvent_swigregister(SetCursorEvent)

#---------------------------------------------------------------------------

class KeyEvent(Event):
    """
    This event class contains information about keypress and character
    events.  These events are only sent to the widget that currently has
    the keyboard focus.

    Notice that there are three different kinds of keyboard events in
    wxWidgets: key down and up events and char events. The difference
    between the first two is clear - the first corresponds to a key press
    and the second to a key release - otherwise they are identical. Just
    note that if the key is maintained in a pressed state you will
    typically get a lot of (automatically generated) down events but only
    one up so it is wrong to assume that there is one up event
    corresponding to each down one.

    Both key events provide untranslated key codes while the char event
    carries the translated one. The untranslated code for alphanumeric
    keys is always an upper case value. For the other keys it is one of
    WXK_XXX values from the keycodes table. The translated key is, in
    general, the character the user expects to appear as the result of the
    key combination when typing the text into a text entry zone, for
    example.

    A few examples to clarify this (all assume that CAPS LOCK is unpressed
    and the standard US keyboard): when the 'A' key is pressed, the key
    down event key code is equal to ASCII A == 65. But the char event key
    code is ASCII a == 97. On the other hand, if you press both SHIFT and
    'A' keys simultaneously , the key code in key down event will still be
    just 'A' while the char event key code parameter will now be 'A' as
    well.

    Although in this simple case it is clear that the correct key code
    could be found in the key down event handler by checking the value
    returned by `ShiftDown`, in general you should use EVT_CHAR for this
    as for non alphanumeric keys or non-US keyboard layouts the
    translation is keyboard-layout dependent and can only be done properly
    by the system itself.

    Another kind of translation is done when the control key is pressed:
    for example, for CTRL-A key press the key down event still carries the
    same key code 'A' as usual but the char event will have key code of 1,
    the ASCII value of this key combination.

    You may discover how the other keys on your system behave
    interactively by running the KeyEvents sample in the wxPython demo and
    pressing some keys while the blue box at the top has the keyboard
    focus.

    **Note**: If a key down event is caught and the event handler does not
    call event.Skip() then the coresponding char event will not
    happen. This is by design and enables the programs that handle both
    types of events to be a bit simpler.

    **Note for Windows programmers**: The key and char events in wxWidgets
    are similar to but slightly different from Windows WM_KEYDOWN and
    WM_CHAR events. In particular, Alt-x combination will generate a char
    event in wxWidgets (unless it is used as an accelerator).

    **Tip**: be sure to call event.Skip() for events that you don't
    process in key event function, otherwise menu shortcuts may cease to
    work under Windows.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType eventType=wxEVT_NULL) -> KeyEvent

        Construct a new `wx.KeyEvent`.  Valid event types are:
            * 
        """
        _core_.KeyEvent_swiginit(self,_core_.new_KeyEvent(*args, **kwargs))
    def GetModifiers(*args, **kwargs):
        """
        GetModifiers(self) -> int

        Returns a bitmask of the current modifier settings.  Can be used to
        check if the key event has exactly the given modifiers without having
        to explicitly check that the other modifiers are not down.  For
        example::

            if event.GetModifers() == wx.MOD_CONTROL:
                DoSomething()

        """
        return _core_.KeyEvent_GetModifiers(*args, **kwargs)

    def ControlDown(*args, **kwargs):
        """
        ControlDown(self) -> bool

        Returns ``True`` if the Control key was down at the time of the event.
        """
        return _core_.KeyEvent_ControlDown(*args, **kwargs)

    def MetaDown(*args, **kwargs):
        """
        MetaDown(self) -> bool

        Returns ``True`` if the Meta key was down at the time of the event.
        """
        return _core_.KeyEvent_MetaDown(*args, **kwargs)

    def AltDown(*args, **kwargs):
        """
        AltDown(self) -> bool

        Returns ``True`` if the Alt key was down at the time of the event.
        """
        return _core_.KeyEvent_AltDown(*args, **kwargs)

    def ShiftDown(*args, **kwargs):
        """
        ShiftDown(self) -> bool

        Returns ``True`` if the Shift key was down at the time of the event.
        """
        return _core_.KeyEvent_ShiftDown(*args, **kwargs)

    def CmdDown(*args, **kwargs):
        """
        CmdDown(self) -> bool

        "Cmd" is a pseudo key which is the same as Control for PC and Unix
        platforms but the special "Apple" (a.k.a as "Command") key on
        Macs. It makes often sense to use it instead of, say, `ControlDown`
        because Cmd key is used for the same thing under Mac as Ctrl
        elsewhere. The Ctrl still exists, it's just not used for this
        purpose. So for non-Mac platforms this is the same as `ControlDown`
        and Macs this is the same as `MetaDown`.
        """
        return _core_.KeyEvent_CmdDown(*args, **kwargs)

    def HasModifiers(*args, **kwargs):
        """
        HasModifiers(self) -> bool

        Returns true if either CTRL or ALT keys was down at the time of the
        key event. Note that this function does not take into account neither
        SHIFT nor META key states (the reason for ignoring the latter is that
        it is common for NUMLOCK key to be configured as META under X but the
        key presses even while NUMLOCK is on should be still processed
        normally).
        """
        return _core_.KeyEvent_HasModifiers(*args, **kwargs)

    def GetKeyCode(*args, **kwargs):
        """
        GetKeyCode(self) -> int

        Returns the virtual key code. ASCII events return normal ASCII values,
        while non-ASCII events return values such as WXK_LEFT for the left
        cursor key. See `wx.KeyEvent` for a full list of the virtual key
        codes.

        Note that in Unicode build, the returned value is meaningful only if
        the user entered a character that can be represented in current
        locale's default charset. You can obtain the corresponding Unicode
        character using `GetUnicodeKey`.
        """
        return _core_.KeyEvent_GetKeyCode(*args, **kwargs)

    def GetUnicodeKey(*args, **kwargs):
        """
        GetUnicodeKey(self) -> int

        Returns the Unicode character corresponding to this key event.  This
        function is only meaningfule in a Unicode build of wxPython.
        """
        return _core_.KeyEvent_GetUnicodeKey(*args, **kwargs)

    GetUniChar = GetUnicodeKey 
    def SetUnicodeKey(*args, **kwargs):
        """
        SetUnicodeKey(self, int uniChar)

        Set the Unicode value of the key event, but only if this is a Unicode
        build of wxPython.
        """
        return _core_.KeyEvent_SetUnicodeKey(*args, **kwargs)

    def GetRawKeyCode(*args, **kwargs):
        """
        GetRawKeyCode(self) -> unsigned int

        Returns the raw key code for this event. This is a platform-dependent
        scan code which should only be used in advanced
        applications. Currently the raw key codes are not supported by all
        ports.
        """
        return _core_.KeyEvent_GetRawKeyCode(*args, **kwargs)

    def GetRawKeyFlags(*args, **kwargs):
        """
        GetRawKeyFlags(self) -> unsigned int

        Returns the low level key flags for this event. The flags are
        platform-dependent and should only be used in advanced applications.
        Currently the raw key flags are not supported by all ports.
        """
        return _core_.KeyEvent_GetRawKeyFlags(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Find the position of the event, if applicable.
        """
        return _core_.KeyEvent_GetPosition(*args, **kwargs)

    def GetPositionTuple(*args, **kwargs):
        """
        GetPositionTuple() -> (x,y)

        Find the position of the event, if applicable.
        """
        return _core_.KeyEvent_GetPositionTuple(*args, **kwargs)

    def GetX(*args, **kwargs):
        """
        GetX(self) -> int

        Returns the X position (in client coordinates) of the event, if
        applicable.
        """
        return _core_.KeyEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """
        GetY(self) -> int

        Returns the Y position (in client coordinates) of the event, if
        applicable.
        """
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
    KeyCode = property(GetKeyCode,doc="See `GetKeyCode`") 
    Modifiers = property(GetModifiers,doc="See `GetModifiers`") 
    Position = property(GetPosition,doc="See `GetPosition`") 
    RawKeyCode = property(GetRawKeyCode,doc="See `GetRawKeyCode`") 
    RawKeyFlags = property(GetRawKeyFlags,doc="See `GetRawKeyFlags`") 
    UnicodeKey = property(GetUnicodeKey,SetUnicodeKey,doc="See `GetUnicodeKey` and `SetUnicodeKey`") 
    X = property(GetX,doc="See `GetX`") 
    Y = property(GetY,doc="See `GetY`") 
_core_.KeyEvent_swigregister(KeyEvent)

#---------------------------------------------------------------------------

class SizeEvent(Event):
    """
    A size event holds information about size change events.  The EVT_SIZE
    handler function will be called when the window it is bound to has
    been resized.

    Note that the size passed is of the whole window: call
    `wx.Window.GetClientSize` for the area which may be used by the
    application.

    When a window is resized, usually only a small part of the window is
    damaged and and that area is all that is in the update region for the
    next paint event. However, if your drawing depends on the size of the
    window, you may need to clear the DC explicitly and repaint the whole
    window. In which case, you may need to call `wx.Window.Refresh` to
    invalidate the entire window.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Size sz=DefaultSize, int winid=0) -> SizeEvent

        Construct a new ``wx.SizeEvent``.
        """
        _core_.SizeEvent_swiginit(self,_core_.new_SizeEvent(*args, **kwargs))
    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> Size

        Returns the entire size of the window generating the size change
        event.
        """
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
    Rect = property(GetRect,SetRect,doc="See `GetRect` and `SetRect`") 
    Size = property(GetSize,SetSize,doc="See `GetSize` and `SetSize`") 
_core_.SizeEvent_swigregister(SizeEvent)

#---------------------------------------------------------------------------

class MoveEvent(Event):
    """
    This event object is sent for EVT_MOVE event bindings when a window is
    moved to a new position.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Point pos=DefaultPosition, int winid=0) -> MoveEvent

        Constructor.
        """
        _core_.MoveEvent_swiginit(self,_core_.new_MoveEvent(*args, **kwargs))
    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Returns the position of the window generating the move change event.
        """
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

    m_pos =  property(GetPosition, SetPosition)
    m_rect = property(GetRect, SetRect)

    Position = property(GetPosition,SetPosition,doc="See `GetPosition` and `SetPosition`") 
    Rect = property(GetRect,SetRect,doc="See `GetRect` and `SetRect`") 
_core_.MoveEvent_swigregister(MoveEvent)

#---------------------------------------------------------------------------

class PaintEvent(Event):
    """
    A paint event is sent when a window's contents needs to be repainted.
    Note that in an EVT_PAINT handler the application must *always* create
    a `wx.PaintDC` object, even if you do not use it.  Otherwise MS
    Windows assumes that the window has not been painted yet and will send
    the event again, causing endless refreshes.

    You can optimize painting by retrieving the rectangles that have been
    damaged using `wx.Window.GetUpdateRegion` and/or `wx.RegionIterator`,
    and only repainting these rectangles. The rectangles are in terms of
    the client area, and are unscrolled, so you will need to do some
    calculations using the current view position to obtain logical,
    scrolled units.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int Id=0) -> PaintEvent"""
        _core_.PaintEvent_swiginit(self,_core_.new_PaintEvent(*args, **kwargs))
_core_.PaintEvent_swigregister(PaintEvent)

class NcPaintEvent(Event):
    """Proxy of C++ NcPaintEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int winid=0) -> NcPaintEvent"""
        _core_.NcPaintEvent_swiginit(self,_core_.new_NcPaintEvent(*args, **kwargs))
_core_.NcPaintEvent_swigregister(NcPaintEvent)

#---------------------------------------------------------------------------

class EraseEvent(Event):
    """
    An erase event is sent whenever the background of a window needs to be
    repainted.  To intercept this event use the EVT_ERASE_BACKGROUND event
    binder.  On some platforms, such as GTK+, this event is simulated
    (simply generated just before the paint event) and may cause flicker.

    To paint a custom background use the `GetDC` method and use the returned
    device context if it is not ``None``, otherwise create a temporary
    `wx.ClientDC` and draw on that.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int Id=0, DC dc=None) -> EraseEvent

        Constructor
        """
        _core_.EraseEvent_swiginit(self,_core_.new_EraseEvent(*args, **kwargs))
    def GetDC(*args, **kwargs):
        """
        GetDC(self) -> DC

        Returns the device context the event handler should draw upon.  If
        ``None`` is returned then create a temporary `wx.ClientDC` and use
        that instead.
        """
        return _core_.EraseEvent_GetDC(*args, **kwargs)

    DC = property(GetDC,doc="See `GetDC`") 
_core_.EraseEvent_swigregister(EraseEvent)

#---------------------------------------------------------------------------

class FocusEvent(Event):
    """
    A focus event is sent when a window's focus changes. The window losing
    focus receives an EVT_KILL_FOCUS event while the window gaining it
    gets an EVT_SET_FOCUS event.

    Notice that the set focus event happens both when the user gives focus
    to the window (whether using the mouse or keyboard) and when it is
    done from the program itself using `wx.Window.SetFocus`.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType type=wxEVT_NULL, int winid=0) -> FocusEvent

        Constructor
        """
        _core_.FocusEvent_swiginit(self,_core_.new_FocusEvent(*args, **kwargs))
    def GetWindow(*args, **kwargs):
        """
        GetWindow(self) -> Window

        Returns the other window associated with this event, that is the
        window which had the focus before for the EVT_SET_FOCUS event and the
        window which is going to receive focus for the wxEVT_KILL_FOCUS event.

        Warning: the window returned may be None!
        """
        return _core_.FocusEvent_GetWindow(*args, **kwargs)

    def SetWindow(*args, **kwargs):
        """SetWindow(self, Window win)"""
        return _core_.FocusEvent_SetWindow(*args, **kwargs)

    Window = property(GetWindow,SetWindow,doc="See `GetWindow` and `SetWindow`") 
_core_.FocusEvent_swigregister(FocusEvent)

#---------------------------------------------------------------------------

class ChildFocusEvent(CommandEvent):
    """
    A child focus event is sent to a (parent-)window when one of its child
    windows gains focus, so that the window could restore the focus back
    to its corresponding child if it loses it now and regains later.

    Notice that child window is the direct child of the window receiving
    the event, and so may not be the actual widget recieving focus if it
    is further down the containment heirarchy.  Use `wx.Window.FindFocus`
    to get the widget that is actually receiving focus.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window win=None) -> ChildFocusEvent

        Constructor
        """
        _core_.ChildFocusEvent_swiginit(self,_core_.new_ChildFocusEvent(*args, **kwargs))
    def GetWindow(*args, **kwargs):
        """
        GetWindow(self) -> Window

        The window, or (grand)parent of the window which has just received the
        focus.
        """
        return _core_.ChildFocusEvent_GetWindow(*args, **kwargs)

    Window = property(GetWindow,doc="See `GetWindow`") 
_core_.ChildFocusEvent_swigregister(ChildFocusEvent)

#---------------------------------------------------------------------------

class ActivateEvent(Event):
    """
    An activate event is sent when a top-level window or the entire
    application is being activated or deactivated.

    A top-level window (a dialog or frame) receives an activate event when
    is being activated or deactivated. This is indicated visually by the
    title bar changing colour, and a subwindow gaining the keyboard focus.
    An application is activated or deactivated when one of its frames
    becomes activated, or a frame becomes inactivate resulting in all
    application frames being inactive.

    Please note that usually you should call event.Skip() in your handlers
    for these events so the default handlers will still be called, as not
    doing so can result in strange effects.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType type=wxEVT_NULL, bool active=True, int Id=0) -> ActivateEvent

        Constructor
        """
        _core_.ActivateEvent_swiginit(self,_core_.new_ActivateEvent(*args, **kwargs))
    def GetActive(*args, **kwargs):
        """
        GetActive(self) -> bool

        Returns true if the application or window is being activated, false
        otherwise.
        """
        return _core_.ActivateEvent_GetActive(*args, **kwargs)

    Active = property(GetActive,doc="See `GetActive`") 
_core_.ActivateEvent_swigregister(ActivateEvent)

#---------------------------------------------------------------------------

class InitDialogEvent(Event):
    """
    A wx.InitDialogEvent is sent as a dialog is being initialised, or for
    any window when `wx.Window.InitDialog` is called.  Handlers for this
    event can transfer data to the window, or anything else that should be
    done before the user begins editing the form. The default handler
    calls `wx.Window.TransferDataToWindow`.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int Id=0) -> InitDialogEvent

        Constructor
        """
        _core_.InitDialogEvent_swiginit(self,_core_.new_InitDialogEvent(*args, **kwargs))
_core_.InitDialogEvent_swigregister(InitDialogEvent)

#---------------------------------------------------------------------------

class MenuEvent(Event):
    """
    This class is used for a variety of menu-related events.  Note that
    these do not include menu command events, which are handled by sending
    `wx.CommandEvent` objects.

    The default handler for wx.EVT_MENU_HIGHLIGHT displays menu item help
    text in the first field of the status bar.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType type=wxEVT_NULL, int winid=0, Menu menu=None) -> MenuEvent

        Constructor
        """
        _core_.MenuEvent_swiginit(self,_core_.new_MenuEvent(*args, **kwargs))
    def GetMenuId(*args, **kwargs):
        """
        GetMenuId(self) -> int

        Returns the menu identifier associated with the event. This method
        should be only used with the HIGHLIGHT events.
        """
        return _core_.MenuEvent_GetMenuId(*args, **kwargs)

    def IsPopup(*args, **kwargs):
        """
        IsPopup(self) -> bool

        Returns ``True`` if the menu which is being opened or closed is a
        popup menu, ``False`` if it is a normal one.  This method should only
        be used with the OPEN and CLOSE events.
        """
        return _core_.MenuEvent_IsPopup(*args, **kwargs)

    def GetMenu(*args, **kwargs):
        """
        GetMenu(self) -> Menu

        Returns the menu which is being opened or closed. This method should
        only be used with the OPEN and CLOSE events.
        """
        return _core_.MenuEvent_GetMenu(*args, **kwargs)

    Menu = property(GetMenu,doc="See `GetMenu`") 
    MenuId = property(GetMenuId,doc="See `GetMenuId`") 
_core_.MenuEvent_swigregister(MenuEvent)

#---------------------------------------------------------------------------

class CloseEvent(Event):
    """
    This event class contains information about window and session close
    events.

    The handler function for EVT_CLOSE is called when the user has tried
    to close a a frame or dialog box using the window manager controls or
    the system menu. It can also be invoked by the application itself
    programmatically, for example by calling the `wx.Window.Close`
    function.

    You should check whether the application is forcing the deletion of
    the window using `CanVeto`. If it returns ``False``, you must destroy
    the window using `wx.Window.Destroy`. If the return value is ``True``,
    it is up to you whether you respond by destroying the window or not.
    For example you may wish to display a message dialog prompting to save
    files or to cancel the close.

    If you don't destroy the window, you should call `Veto` to let the
    calling code know that you did not destroy the window. This allows the
    `wx.Window.Close` function to return ``True`` or ``False`` depending
    on whether the close instruction was honored or not.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType type=wxEVT_NULL, int winid=0) -> CloseEvent

        Constructor.
        """
        _core_.CloseEvent_swiginit(self,_core_.new_CloseEvent(*args, **kwargs))
    def SetLoggingOff(*args, **kwargs):
        """
        SetLoggingOff(self, bool logOff)

        Sets the 'logging off' flag.
        """
        return _core_.CloseEvent_SetLoggingOff(*args, **kwargs)

    def GetLoggingOff(*args, **kwargs):
        """
        GetLoggingOff(self) -> bool

        Returns ``True`` if the user is logging off or ``False`` if the
        system is shutting down. This method can only be called for end
        session and query end session events, it doesn't make sense for close
        window event.
        """
        return _core_.CloseEvent_GetLoggingOff(*args, **kwargs)

    def Veto(*args, **kwargs):
        """
        Veto(self, bool veto=True)

        Call this from your event handler to veto a system shutdown or to
        signal to the calling application that a window close did not happen.

        You can only veto a shutdown or close if `CanVeto` returns true.
        """
        return _core_.CloseEvent_Veto(*args, **kwargs)

    def GetVeto(*args, **kwargs):
        """GetVeto(self) -> bool"""
        return _core_.CloseEvent_GetVeto(*args, **kwargs)

    def SetCanVeto(*args, **kwargs):
        """
        SetCanVeto(self, bool canVeto)

        Sets the 'can veto' flag.
        """
        return _core_.CloseEvent_SetCanVeto(*args, **kwargs)

    def CanVeto(*args, **kwargs):
        """
        CanVeto(self) -> bool

        Returns true if you can veto a system shutdown or a window close
        event. Vetoing a window close event is not possible if the calling
        code wishes to force the application to exit, and so this function
        must be called to check this.
        """
        return _core_.CloseEvent_CanVeto(*args, **kwargs)

    LoggingOff = property(GetLoggingOff,SetLoggingOff,doc="See `GetLoggingOff` and `SetLoggingOff`") 
_core_.CloseEvent_swigregister(CloseEvent)

#---------------------------------------------------------------------------

class ShowEvent(Event):
    """An EVT_SHOW event is sent when a window is shown or hidden."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int winid=0, bool show=False) -> ShowEvent

        An EVT_SHOW event is sent when a window is shown or hidden.
        """
        _core_.ShowEvent_swiginit(self,_core_.new_ShowEvent(*args, **kwargs))
    def SetShow(*args, **kwargs):
        """SetShow(self, bool show)"""
        return _core_.ShowEvent_SetShow(*args, **kwargs)

    def GetShow(*args, **kwargs):
        """GetShow(self) -> bool"""
        return _core_.ShowEvent_GetShow(*args, **kwargs)

    Show = property(GetShow,SetShow,doc="See `GetShow` and `SetShow`") 
_core_.ShowEvent_swigregister(ShowEvent)

#---------------------------------------------------------------------------

class IconizeEvent(Event):
    """
    An EVT_ICONIZE event is sent when a frame is iconized (minimized) or
    restored.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int id=0, bool iconized=True) -> IconizeEvent

        An EVT_ICONIZE event is sent when a frame is iconized (minimized) or
        restored.
        """
        _core_.IconizeEvent_swiginit(self,_core_.new_IconizeEvent(*args, **kwargs))
    def Iconized(*args, **kwargs):
        """
        Iconized(self) -> bool

        Returns ``True`` if the frame has been iconized, ``False`` if it has
        been restored.
        """
        return _core_.IconizeEvent_Iconized(*args, **kwargs)

_core_.IconizeEvent_swigregister(IconizeEvent)

#---------------------------------------------------------------------------

class MaximizeEvent(Event):
    """An EVT_MAXIMIZE event is sent when a frame is maximized or restored."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int id=0) -> MaximizeEvent

        An EVT_MAXIMIZE event is sent when a frame is maximized or restored.
        """
        _core_.MaximizeEvent_swiginit(self,_core_.new_MaximizeEvent(*args, **kwargs))
_core_.MaximizeEvent_swigregister(MaximizeEvent)

#---------------------------------------------------------------------------

class DropFilesEvent(Event):
    """
    This class is used for drop files events, that is, when files have
    been dropped onto the window. This functionality is only available
    under Windows. The window must have previously been enabled for
    dropping by calling `wx.Window.DragAcceptFiles`.

    Important note: this is a separate implementation to the more general
    drag and drop implementation using `wx.FileDropTarget`, and etc. This
    implementation uses the older, Windows message-based approach of
    dropping files.

    Use wx.EVT_DROP_FILES to bind an event handler to receive file drop
    events.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Returns the position at which the files were dropped.
        """
        return _core_.DropFilesEvent_GetPosition(*args, **kwargs)

    def GetNumberOfFiles(*args, **kwargs):
        """
        GetNumberOfFiles(self) -> int

        Returns the number of files dropped.
        """
        return _core_.DropFilesEvent_GetNumberOfFiles(*args, **kwargs)

    def GetFiles(*args, **kwargs):
        """
        GetFiles(self) -> PyObject

        Returns a list of the filenames that were dropped.
        """
        return _core_.DropFilesEvent_GetFiles(*args, **kwargs)

    Files = property(GetFiles,doc="See `GetFiles`") 
    NumberOfFiles = property(GetNumberOfFiles,doc="See `GetNumberOfFiles`") 
    Position = property(GetPosition,doc="See `GetPosition`") 
_core_.DropFilesEvent_swigregister(DropFilesEvent)

#---------------------------------------------------------------------------

UPDATE_UI_PROCESS_ALL = _core_.UPDATE_UI_PROCESS_ALL
UPDATE_UI_PROCESS_SPECIFIED = _core_.UPDATE_UI_PROCESS_SPECIFIED
class UpdateUIEvent(CommandEvent):
    """
    This class is used for EVT_UPDATE_UI pseudo-events which are sent by
    wxWidgets to give an application the chance to update various user
    interface elements.

    Without update UI events, an application has to work hard to
    check/uncheck, enable/disable, and set the text for elements such as
    menu items and toolbar buttons. The code for doing this has to be
    mixed up with the code that is invoked when an action is invoked for a
    menu item or button.

    With update UI events, you define an event handler to look at the
    state of the application and change UI elements accordingly. wxWidgets
    will call your handler functions in idle time, so you don't have to
    worry where to call this code. In addition to being a clearer and more
    declarative method, it also means you don't have to worry whether
    you're updating a toolbar or menubar identifier. The same handler can
    update a menu item and toolbar button, if the ID values are the same.

    Instead of directly manipulating the menu or button, you call
    functions in the event object, such as `Check`. wxWidgets will
    determine whether such a call has been made, and which UI element to
    update.

    These events will work for popup menus as well as menubars. Just
    before a menu is popped up, `wx.Menu.UpdateUI` is called to process
    any UI events for the window that owns the menu.

    If you find that the overhead of UI update processing is affecting
    your application, you can do one or both of the following:

       1. Call `wx.UpdateUIEvent.SetMode` with a value of
          wx.UPDATE_UI_PROCESS_SPECIFIED, and set the extra style
          wx.WS_EX_PROCESS_UPDATE_EVENTS for every window that should
          receive update events. No other windows will receive update
          events.

       2. Call `wx.UpdateUIEvent.SetUpdateInterval` with a millisecond
          value to set the delay between updates. You may need to call
          `wx.Window.UpdateWindowUI` at critical points, for example when
          a dialog is about to be shown, in case the user sees a slight
          delay before windows are updated.

    Note that although events are sent in idle time, defining a EVT_IDLE
    handler for a window does not affect this because the events are sent
    from an internal idle handler.

    wxWidgets tries to optimize update events on some platforms. On
    Windows and GTK+, events for menubar items are only sent when the menu
    is about to be shown, and not in idle time.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int commandId=0) -> UpdateUIEvent

        Constructor
        """
        _core_.UpdateUIEvent_swiginit(self,_core_.new_UpdateUIEvent(*args, **kwargs))
    def GetChecked(*args, **kwargs):
        """
        GetChecked(self) -> bool

        Returns ``True`` if the UI element should be checked.
        """
        return _core_.UpdateUIEvent_GetChecked(*args, **kwargs)

    def GetEnabled(*args, **kwargs):
        """
        GetEnabled(self) -> bool

        Returns ``True`` if the UI element should be enabled.
        """
        return _core_.UpdateUIEvent_GetEnabled(*args, **kwargs)

    def GetShown(*args, **kwargs):
        """
        GetShown(self) -> bool

        Returns ``True`` if the UI element should be shown.
        """
        return _core_.UpdateUIEvent_GetShown(*args, **kwargs)

    def GetText(*args, **kwargs):
        """
        GetText(self) -> String

        Returns the text that should be set for the UI element.
        """
        return _core_.UpdateUIEvent_GetText(*args, **kwargs)

    def GetSetText(*args, **kwargs):
        """
        GetSetText(self) -> bool

        Returns ``True`` if the application has called `SetText`. For
        wxWidgets internal use only.
        """
        return _core_.UpdateUIEvent_GetSetText(*args, **kwargs)

    def GetSetChecked(*args, **kwargs):
        """
        GetSetChecked(self) -> bool

        Returns ``True`` if the application has called `Check`. For wxWidgets
        internal use only.
        """
        return _core_.UpdateUIEvent_GetSetChecked(*args, **kwargs)

    def GetSetEnabled(*args, **kwargs):
        """
        GetSetEnabled(self) -> bool

        Returns ``True`` if the application has called `Enable`. For wxWidgets
        internal use only.
        """
        return _core_.UpdateUIEvent_GetSetEnabled(*args, **kwargs)

    def GetSetShown(*args, **kwargs):
        """
        GetSetShown(self) -> bool

        Returns ``True`` if the application has called `Show`. For wxWidgets
        internal use only.
        """
        return _core_.UpdateUIEvent_GetSetShown(*args, **kwargs)

    def Check(*args, **kwargs):
        """
        Check(self, bool check)

        Check or uncheck the UI element.
        """
        return _core_.UpdateUIEvent_Check(*args, **kwargs)

    def Enable(*args, **kwargs):
        """
        Enable(self, bool enable)

        Enable or disable the UI element.
        """
        return _core_.UpdateUIEvent_Enable(*args, **kwargs)

    def Show(*args, **kwargs):
        """
        Show(self, bool show)

        Show or hide the UI element.
        """
        return _core_.UpdateUIEvent_Show(*args, **kwargs)

    def SetText(*args, **kwargs):
        """
        SetText(self, String text)

        Sets the text for this UI element.
        """
        return _core_.UpdateUIEvent_SetText(*args, **kwargs)

    def SetUpdateInterval(*args, **kwargs):
        """
        SetUpdateInterval(long updateInterval)

        Sets the interval between updates in milliseconds. Set to -1 to
        disable updates, or to 0 to update as frequently as possible. The
        default is 0.

        Use this to reduce the overhead of UI update events if your
        application has a lot of windows. If you set the value to -1 or
        greater than 0, you may also need to call `wx.Window.UpdateWindowUI`
        at appropriate points in your application, such as when a dialog is
        about to be shown.
        """
        return _core_.UpdateUIEvent_SetUpdateInterval(*args, **kwargs)

    SetUpdateInterval = staticmethod(SetUpdateInterval)
    def GetUpdateInterval(*args, **kwargs):
        """
        GetUpdateInterval() -> long

        Returns the current interval between updates in milliseconds. -1
        disables updates, 0 updates as frequently as possible.
        """
        return _core_.UpdateUIEvent_GetUpdateInterval(*args, **kwargs)

    GetUpdateInterval = staticmethod(GetUpdateInterval)
    def CanUpdate(*args, **kwargs):
        """
        CanUpdate(Window win) -> bool

        Returns ``True`` if it is appropriate to update (send UI update events
        to) this window.

        This function looks at the mode used (see `wx.UpdateUIEvent.SetMode`),
        the wx.WS_EX_PROCESS_UPDATE_EVENTS flag in window, the time update
        events were last sent in idle time, and the update interval, to
        determine whether events should be sent to this window now. By default
        this will always return true because the update mode is initially
        wx.UPDATE_UI_PROCESS_ALL and the interval is set to 0; so update
        events will be sent as often as possible. You can reduce the frequency
        that events are sent by changing the mode and/or setting an update
        interval.

        """
        return _core_.UpdateUIEvent_CanUpdate(*args, **kwargs)

    CanUpdate = staticmethod(CanUpdate)
    def ResetUpdateTime(*args, **kwargs):
        """
        ResetUpdateTime()

        Used internally to reset the last-updated time to the current time. It
        is assumed that update events are normally sent in idle time, so this
        is called at the end of idle processing.
        """
        return _core_.UpdateUIEvent_ResetUpdateTime(*args, **kwargs)

    ResetUpdateTime = staticmethod(ResetUpdateTime)
    def SetMode(*args, **kwargs):
        """
        SetMode(int mode)

        Specify how wxWidgets will send update events: to all windows, or only
        to those which specify that they will process the events.

        The mode may be one of the following values:

            =============================   ==========================================
            wxUPDATE_UI_PROCESS_ALL         Send UI update events to all windows.  This
                                            is the default setting.
            wxUPDATE_UI_PROCESS_SPECIFIED   Send UI update events only to windows that
                                            have the wx.WS_EX_PROCESS_UI_UPDATES extra
                                            style set.
            =============================   ==========================================

        """
        return _core_.UpdateUIEvent_SetMode(*args, **kwargs)

    SetMode = staticmethod(SetMode)
    def GetMode(*args, **kwargs):
        """
        GetMode() -> int

        Returns a value specifying how wxWidgets will send update events: to
        all windows, or only to those which specify that they will process the
        events.
        """
        return _core_.UpdateUIEvent_GetMode(*args, **kwargs)

    GetMode = staticmethod(GetMode)
    Checked = property(GetChecked,Check,doc="See `GetChecked`") 
    Enabled = property(GetEnabled,Enable,doc="See `GetEnabled`") 
    Shown = property(GetShown,Show,doc="See `GetShown`") 
    Text = property(GetText,SetText,doc="See `GetText` and `SetText`") 
_core_.UpdateUIEvent_swigregister(UpdateUIEvent)

def UpdateUIEvent_SetUpdateInterval(*args, **kwargs):
  """
    UpdateUIEvent_SetUpdateInterval(long updateInterval)

    Sets the interval between updates in milliseconds. Set to -1 to
    disable updates, or to 0 to update as frequently as possible. The
    default is 0.

    Use this to reduce the overhead of UI update events if your
    application has a lot of windows. If you set the value to -1 or
    greater than 0, you may also need to call `wx.Window.UpdateWindowUI`
    at appropriate points in your application, such as when a dialog is
    about to be shown.
    """
  return _core_.UpdateUIEvent_SetUpdateInterval(*args, **kwargs)

def UpdateUIEvent_GetUpdateInterval(*args):
  """
    UpdateUIEvent_GetUpdateInterval() -> long

    Returns the current interval between updates in milliseconds. -1
    disables updates, 0 updates as frequently as possible.
    """
  return _core_.UpdateUIEvent_GetUpdateInterval(*args)

def UpdateUIEvent_CanUpdate(*args, **kwargs):
  """
    UpdateUIEvent_CanUpdate(Window win) -> bool

    Returns ``True`` if it is appropriate to update (send UI update events
    to) this window.

    This function looks at the mode used (see `wx.UpdateUIEvent.SetMode`),
    the wx.WS_EX_PROCESS_UPDATE_EVENTS flag in window, the time update
    events were last sent in idle time, and the update interval, to
    determine whether events should be sent to this window now. By default
    this will always return true because the update mode is initially
    wx.UPDATE_UI_PROCESS_ALL and the interval is set to 0; so update
    events will be sent as often as possible. You can reduce the frequency
    that events are sent by changing the mode and/or setting an update
    interval.

    """
  return _core_.UpdateUIEvent_CanUpdate(*args, **kwargs)

def UpdateUIEvent_ResetUpdateTime(*args):
  """
    UpdateUIEvent_ResetUpdateTime()

    Used internally to reset the last-updated time to the current time. It
    is assumed that update events are normally sent in idle time, so this
    is called at the end of idle processing.
    """
  return _core_.UpdateUIEvent_ResetUpdateTime(*args)

def UpdateUIEvent_SetMode(*args, **kwargs):
  """
    UpdateUIEvent_SetMode(int mode)

    Specify how wxWidgets will send update events: to all windows, or only
    to those which specify that they will process the events.

    The mode may be one of the following values:

        =============================   ==========================================
        wxUPDATE_UI_PROCESS_ALL         Send UI update events to all windows.  This
                                        is the default setting.
        wxUPDATE_UI_PROCESS_SPECIFIED   Send UI update events only to windows that
                                        have the wx.WS_EX_PROCESS_UI_UPDATES extra
                                        style set.
        =============================   ==========================================

    """
  return _core_.UpdateUIEvent_SetMode(*args, **kwargs)

def UpdateUIEvent_GetMode(*args):
  """
    UpdateUIEvent_GetMode() -> int

    Returns a value specifying how wxWidgets will send update events: to
    all windows, or only to those which specify that they will process the
    events.
    """
  return _core_.UpdateUIEvent_GetMode(*args)

#---------------------------------------------------------------------------

class SysColourChangedEvent(Event):
    """
    This class is used for EVT_SYS_COLOUR_CHANGED, which are generated
    when the user changes the colour settings using the control
    panel. This is only applicable under Windows.

    The default event handler for this event propagates the event to child
    windows, since Windows only sends the events to top-level windows. If
    intercepting this event for a top-level window, remember to call
    `Skip` so the the base class handler will still be executed, or to
    pass the event on to the window's children explicitly.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> SysColourChangedEvent

        Constructor
        """
        _core_.SysColourChangedEvent_swiginit(self,_core_.new_SysColourChangedEvent(*args, **kwargs))
_core_.SysColourChangedEvent_swigregister(SysColourChangedEvent)

#---------------------------------------------------------------------------

class MouseCaptureChangedEvent(Event):
    """
    An mouse capture changed event (EVT_MOUSE_CAPTURE_CHANGED) is sent to
    a window that loses its mouse capture. This is called even if
    `wx.Window.ReleaseMouse` was called by the application code. Handling
    this event allows an application to cater for unexpected capture
    releases which might otherwise confuse mouse handling code.

    This event is implemented under Windows only.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int winid=0, Window gainedCapture=None) -> MouseCaptureChangedEvent

        Constructor
        """
        _core_.MouseCaptureChangedEvent_swiginit(self,_core_.new_MouseCaptureChangedEvent(*args, **kwargs))
    def GetCapturedWindow(*args, **kwargs):
        """
        GetCapturedWindow(self) -> Window

        Returns the window that gained the capture, or ``None`` if it was a
        non-wxWidgets window.
        """
        return _core_.MouseCaptureChangedEvent_GetCapturedWindow(*args, **kwargs)

    CapturedWindow = property(GetCapturedWindow,doc="See `GetCapturedWindow`") 
_core_.MouseCaptureChangedEvent_swigregister(MouseCaptureChangedEvent)

#---------------------------------------------------------------------------

class MouseCaptureLostEvent(Event):
    """
    A mouse capture lost event is sent to a window that obtained mouse
    capture, which was subsequently loss due to "external" event, for
    example when a dialog box is shown or if another application captures
    the mouse.

    If this happens, this event is sent to all windows that are on the
    capture stack (i.e. a window that called `wx.Window.CaptureMouse`, but
    didn't call `wx.Window.ReleaseMouse` yet). The event is *not* sent
    if the capture changes because of a call to CaptureMouse or
    ReleaseMouse.

    This event is currently emitted under Windows only.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int winid=0) -> MouseCaptureLostEvent

        A mouse capture lost event is sent to a window that obtained mouse
        capture, which was subsequently loss due to "external" event, for
        example when a dialog box is shown or if another application captures
        the mouse.

        If this happens, this event is sent to all windows that are on the
        capture stack (i.e. a window that called `wx.Window.CaptureMouse`, but
        didn't call `wx.Window.ReleaseMouse` yet). The event is *not* sent
        if the capture changes because of a call to CaptureMouse or
        ReleaseMouse.

        This event is currently emitted under Windows only.

        """
        _core_.MouseCaptureLostEvent_swiginit(self,_core_.new_MouseCaptureLostEvent(*args, **kwargs))
_core_.MouseCaptureLostEvent_swigregister(MouseCaptureLostEvent)

#---------------------------------------------------------------------------

class DisplayChangedEvent(Event):
    """
    An EVT_DISPLAY_CHANGED event is sent to all windows when the display
    resolution has changed.

    This event is implemented under Windows only.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> DisplayChangedEvent"""
        _core_.DisplayChangedEvent_swiginit(self,_core_.new_DisplayChangedEvent(*args, **kwargs))
_core_.DisplayChangedEvent_swigregister(DisplayChangedEvent)

#---------------------------------------------------------------------------

class PaletteChangedEvent(Event):
    """
    An EVT_PALETTE_CHANGED event is sent when the system palette has
    changed, thereby giving each window a chance to redo their own to
    match.

    This event is implemented under Windows only.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int id=0) -> PaletteChangedEvent

        An EVT_PALETTE_CHANGED event is sent when the system palette has
        changed, thereby giving each window a chance to redo their own to
        match.

        This event is implemented under Windows only.
        """
        _core_.PaletteChangedEvent_swiginit(self,_core_.new_PaletteChangedEvent(*args, **kwargs))
    def SetChangedWindow(*args, **kwargs):
        """SetChangedWindow(self, Window win)"""
        return _core_.PaletteChangedEvent_SetChangedWindow(*args, **kwargs)

    def GetChangedWindow(*args, **kwargs):
        """GetChangedWindow(self) -> Window"""
        return _core_.PaletteChangedEvent_GetChangedWindow(*args, **kwargs)

    ChangedWindow = property(GetChangedWindow,SetChangedWindow,doc="See `GetChangedWindow` and `SetChangedWindow`") 
_core_.PaletteChangedEvent_swigregister(PaletteChangedEvent)

#---------------------------------------------------------------------------

class QueryNewPaletteEvent(Event):
    """
    An EVT_QUERY_NEW_PALETE event indicates the window is getting keyboard
    focus and should re-do its palette.

    This event is implemented under Windows only.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int winid=0) -> QueryNewPaletteEvent

        Constructor.
        """
        _core_.QueryNewPaletteEvent_swiginit(self,_core_.new_QueryNewPaletteEvent(*args, **kwargs))
    def SetPaletteRealized(*args, **kwargs):
        """
        SetPaletteRealized(self, bool realized)

        App should set this if it changes the palette.
        """
        return _core_.QueryNewPaletteEvent_SetPaletteRealized(*args, **kwargs)

    def GetPaletteRealized(*args, **kwargs):
        """GetPaletteRealized(self) -> bool"""
        return _core_.QueryNewPaletteEvent_GetPaletteRealized(*args, **kwargs)

    PaletteRealized = property(GetPaletteRealized,SetPaletteRealized,doc="See `GetPaletteRealized` and `SetPaletteRealized`") 
_core_.QueryNewPaletteEvent_swigregister(QueryNewPaletteEvent)

#---------------------------------------------------------------------------

class NavigationKeyEvent(Event):
    """
    EVT_NAVIGATION_KEY events are used to control moving the focus between
    widgets, otherwise known as tab-traversal.  You woudl normally not
    catch navigation events in applications as there are already
    appropriate handlers in `wx.Dialog` and `wx.Panel`, but you may find
    it useful to send navigation events in certain situations to change
    the focus in certain ways, although it's probably easier to just call
    `wx.Window.Navigate`.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> NavigationKeyEvent"""
        _core_.NavigationKeyEvent_swiginit(self,_core_.new_NavigationKeyEvent(*args, **kwargs))
    def GetDirection(*args, **kwargs):
        """
        GetDirection(self) -> bool

        Returns ``True`` if the direction is forward, ``False`` otherwise.
        """
        return _core_.NavigationKeyEvent_GetDirection(*args, **kwargs)

    def SetDirection(*args, **kwargs):
        """
        SetDirection(self, bool forward)

        Specify the direction that the navigation should take.  Usually the
        difference between using Tab and Shift-Tab.
        """
        return _core_.NavigationKeyEvent_SetDirection(*args, **kwargs)

    def IsWindowChange(*args, **kwargs):
        """
        IsWindowChange(self) -> bool

        Returns ``True`` if window change is allowed.
        """
        return _core_.NavigationKeyEvent_IsWindowChange(*args, **kwargs)

    def SetWindowChange(*args, **kwargs):
        """
        SetWindowChange(self, bool ischange)

        Specify if the navigation should be able to change parent windows.
        For example, changing notebook pages, etc. This is usually implemented
        by using Control-Tab.
        """
        return _core_.NavigationKeyEvent_SetWindowChange(*args, **kwargs)

    def IsFromTab(*args, **kwargs):
        """
        IsFromTab(self) -> bool

        Returns ``True`` if the navigation event is originated from the Tab
        key.
        """
        return _core_.NavigationKeyEvent_IsFromTab(*args, **kwargs)

    def SetFromTab(*args, **kwargs):
        """
        SetFromTab(self, bool bIs)

        Set to true under MSW if the event was generated using the tab key.
        This is required for proper navogation over radio buttons.
        """
        return _core_.NavigationKeyEvent_SetFromTab(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """
        SetFlags(self, long flags)

        Set the navigation flags to a combination of the following:

            * wx.NavigationKeyEvent.IsBackward
            * wx.NavigationKeyEvent.IsForward
            * wx.NavigationKeyEvent.WinChange
            * wx.NavigationKeyEvent.FromTab

        """
        return _core_.NavigationKeyEvent_SetFlags(*args, **kwargs)

    def GetCurrentFocus(*args, **kwargs):
        """
        GetCurrentFocus(self) -> Window

        Returns the child window which currenty has the focus.  May be
        ``None``.
        """
        return _core_.NavigationKeyEvent_GetCurrentFocus(*args, **kwargs)

    def SetCurrentFocus(*args, **kwargs):
        """
        SetCurrentFocus(self, Window win)

        Set the window that has the focus.
        """
        return _core_.NavigationKeyEvent_SetCurrentFocus(*args, **kwargs)

    IsBackward = _core_.NavigationKeyEvent_IsBackward
    IsForward = _core_.NavigationKeyEvent_IsForward
    WinChange = _core_.NavigationKeyEvent_WinChange
    FromTab = _core_.NavigationKeyEvent_FromTab
    CurrentFocus = property(GetCurrentFocus,SetCurrentFocus,doc="See `GetCurrentFocus` and `SetCurrentFocus`") 
    Direction = property(GetDirection,SetDirection,doc="See `GetDirection` and `SetDirection`") 
_core_.NavigationKeyEvent_swigregister(NavigationKeyEvent)

#---------------------------------------------------------------------------

class WindowCreateEvent(CommandEvent):
    """
    The EVT_WINDOW_CREATE event is sent as soon as the window object (the
    underlying GUI object) exists.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window win=None) -> WindowCreateEvent

        The EVT_WINDOW_CREATE event is sent as soon as the window object (the
        underlying GUI object) exists.
        """
        _core_.WindowCreateEvent_swiginit(self,_core_.new_WindowCreateEvent(*args, **kwargs))
    def GetWindow(*args, **kwargs):
        """
        GetWindow(self) -> Window

        Returns the window that this event refers to.
        """
        return _core_.WindowCreateEvent_GetWindow(*args, **kwargs)

    Window = property(GetWindow,doc="See `GetWindow`") 
_core_.WindowCreateEvent_swigregister(WindowCreateEvent)

class WindowDestroyEvent(CommandEvent):
    """
    The EVT_WINDOW_DESTROY event is sent from the `wx.Window` destructor
    when the GUI window is destroyed.

    When a class derived from `wx.Window` is destroyed its destructor will
    have already run by the time this event is sent. Therefore this event
    will not usually be received at all by the window itself.  Since it is
    received after the destructor has run, an object should not try to
    handle its own wx.WindowDestroyEvent, but it can be used to get
    notification of the destruction of another window.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window win=None) -> WindowDestroyEvent

        The EVT_WINDOW_DESTROY event is sent from the `wx.Window` destructor
        when the GUI window is destroyed.

        When a class derived from `wx.Window` is destroyed its destructor will
        have already run by the time this event is sent. Therefore this event
        will not usually be received at all by the window itself.  Since it is
        received after the destructor has run, an object should not try to
        handle its own wx.WindowDestroyEvent, but it can be used to get
        notification of the destruction of another window.
        """
        _core_.WindowDestroyEvent_swiginit(self,_core_.new_WindowDestroyEvent(*args, **kwargs))
    def GetWindow(*args, **kwargs):
        """
        GetWindow(self) -> Window

        Returns the window that this event refers to.
        """
        return _core_.WindowDestroyEvent_GetWindow(*args, **kwargs)

    Window = property(GetWindow,doc="See `GetWindow`") 
_core_.WindowDestroyEvent_swigregister(WindowDestroyEvent)

#---------------------------------------------------------------------------

class ContextMenuEvent(CommandEvent):
    """
    This class is used for context menu events (EVT_CONTECT_MENU,) sent to
    give the application a chance to show a context (popup) menu.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType type=wxEVT_NULL, int winid=0, Point pt=DefaultPosition) -> ContextMenuEvent

        Constructor.
        """
        _core_.ContextMenuEvent_swiginit(self,_core_.new_ContextMenuEvent(*args, **kwargs))
    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Returns the position (in screen coordinants) at which the menu should
        be shown.
        """
        return _core_.ContextMenuEvent_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """
        SetPosition(self, Point pos)

        Sets the position at which the menu should be shown.
        """
        return _core_.ContextMenuEvent_SetPosition(*args, **kwargs)

    Position = property(GetPosition,SetPosition,doc="See `GetPosition` and `SetPosition`") 
_core_.ContextMenuEvent_swigregister(ContextMenuEvent)

#---------------------------------------------------------------------------

IDLE_PROCESS_ALL = _core_.IDLE_PROCESS_ALL
IDLE_PROCESS_SPECIFIED = _core_.IDLE_PROCESS_SPECIFIED
class IdleEvent(Event):
    """
    This class is used for EVT_IDLE events, which are generated and sent
    when the application *becomes* idle.  In other words, the when the
    event queue becomes empty then idle events are sent to all windows (by
    default) and as long as none of them call `RequestMore` then there are
    no more idle events until after the system event queue has some normal
    events and then becomes empty again.

    By default, idle events are sent to all windows. If this is causing a
    significant overhead in your application, you can call
    `wx.IdleEvent.SetMode` with the value wx.IDLE_PROCESS_SPECIFIED, and
    set the wx.WS_EX_PROCESS_IDLE extra window style for every window
    which should receive idle events.  Then idle events will only be sent
    to those windows and not to any others.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> IdleEvent

        Constructor
        """
        _core_.IdleEvent_swiginit(self,_core_.new_IdleEvent(*args, **kwargs))
    def RequestMore(*args, **kwargs):
        """
        RequestMore(self, bool needMore=True)

        Tells wxWidgets that more processing is required. This function can be
        called by an EVT_IDLE handler for a window to indicate that the
        application should forward the EVT_IDLE event once more to the
        application windows. If no window calls this function during its
        EVT_IDLE handler, then the application will remain in a passive event
        loop until a new event is posted to the application by the windowing
        system.
        """
        return _core_.IdleEvent_RequestMore(*args, **kwargs)

    def MoreRequested(*args, **kwargs):
        """
        MoreRequested(self) -> bool

        Returns ``True`` if the OnIdle function processing this event
        requested more processing time.
        """
        return _core_.IdleEvent_MoreRequested(*args, **kwargs)

    def SetMode(*args, **kwargs):
        """
        SetMode(int mode)

        Static method for specifying how wxWidgets will send idle events: to
        all windows, or only to those which specify that they will process the
        events.

        The mode can be one of the following values:

            =========================   ========================================
            wx.IDLE_PROCESS_ALL         Send idle events to all windows
            wx.IDLE_PROCESS_SPECIFIED   Send idle events only to windows that have
                                        the wx.WS_EX_PROCESS_IDLE extra style
                                        flag set.
            =========================   ========================================

        """
        return _core_.IdleEvent_SetMode(*args, **kwargs)

    SetMode = staticmethod(SetMode)
    def GetMode(*args, **kwargs):
        """
        GetMode() -> int

        Static method returning a value specifying how wxWidgets will send
        idle events: to all windows, or only to those which specify that they
        will process the events.
        """
        return _core_.IdleEvent_GetMode(*args, **kwargs)

    GetMode = staticmethod(GetMode)
    def CanSend(*args, **kwargs):
        """
        CanSend(Window win) -> bool

        Returns ``True`` if it is appropriate to send idle events to this
        window.

        This function looks at the mode used (see `wx.IdleEvent.SetMode`), and
        the wx.WS_EX_PROCESS_IDLE style in window to determine whether idle
        events should be sent to this window now. By default this will always
        return ``True`` because the update mode is initially
        wx.IDLE_PROCESS_ALL. You can change the mode to only send idle events
        to windows with the wx.WS_EX_PROCESS_IDLE extra window style set.
        """
        return _core_.IdleEvent_CanSend(*args, **kwargs)

    CanSend = staticmethod(CanSend)
_core_.IdleEvent_swigregister(IdleEvent)

def IdleEvent_SetMode(*args, **kwargs):
  """
    IdleEvent_SetMode(int mode)

    Static method for specifying how wxWidgets will send idle events: to
    all windows, or only to those which specify that they will process the
    events.

    The mode can be one of the following values:

        =========================   ========================================
        wx.IDLE_PROCESS_ALL         Send idle events to all windows
        wx.IDLE_PROCESS_SPECIFIED   Send idle events only to windows that have
                                    the wx.WS_EX_PROCESS_IDLE extra style
                                    flag set.
        =========================   ========================================

    """
  return _core_.IdleEvent_SetMode(*args, **kwargs)

def IdleEvent_GetMode(*args):
  """
    IdleEvent_GetMode() -> int

    Static method returning a value specifying how wxWidgets will send
    idle events: to all windows, or only to those which specify that they
    will process the events.
    """
  return _core_.IdleEvent_GetMode(*args)

def IdleEvent_CanSend(*args, **kwargs):
  """
    IdleEvent_CanSend(Window win) -> bool

    Returns ``True`` if it is appropriate to send idle events to this
    window.

    This function looks at the mode used (see `wx.IdleEvent.SetMode`), and
    the wx.WS_EX_PROCESS_IDLE style in window to determine whether idle
    events should be sent to this window now. By default this will always
    return ``True`` because the update mode is initially
    wx.IDLE_PROCESS_ALL. You can change the mode to only send idle events
    to windows with the wx.WS_EX_PROCESS_IDLE extra window style set.
    """
  return _core_.IdleEvent_CanSend(*args, **kwargs)

#---------------------------------------------------------------------------

class ClipboardTextEvent(CommandEvent):
    """
    A Clipboard Text event is sent when a window intercepts a text
    copy/cut/paste message, i.e. the user has cut/copied/pasted data
    from/into a text control via ctrl-C/X/V, ctrl/shift-del/insert, a
    popup menu command, etc.  NOTE : under windows these events are *NOT*
    generated automatically for a Rich Edit text control.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType type=wxEVT_NULL, int winid=0) -> ClipboardTextEvent

        A Clipboard Text event is sent when a window intercepts a text
        copy/cut/paste message, i.e. the user has cut/copied/pasted data
        from/into a text control via ctrl-C/X/V, ctrl/shift-del/insert, a
        popup menu command, etc.  NOTE : under windows these events are *NOT*
        generated automatically for a Rich Edit text control.
        """
        _core_.ClipboardTextEvent_swiginit(self,_core_.new_ClipboardTextEvent(*args, **kwargs))
_core_.ClipboardTextEvent_swigregister(ClipboardTextEvent)

#---------------------------------------------------------------------------

class PyEvent(Event):
    """
    wx.PyEvent can be used as a base class for implementing custom event
    types in Python.  You should derived from this class instead of
    `wx.Event` because this class is Python-aware and is able to transport
    its Python bits safely through the wxWidgets event system and have
    them still be there when the event handler is invoked.

    :see: `wx.PyCommandEvent`

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int winid=0, EventType eventType=wxEVT_NULL) -> PyEvent"""
        _core_.PyEvent_swiginit(self,_core_.new_PyEvent(*args, **kwargs))
        self._SetSelf(self)

    __swig_destroy__ = _core_.delete_PyEvent
    __del__ = lambda self : None;
    def _SetSelf(*args, **kwargs):
        """_SetSelf(self, PyObject self)"""
        return _core_.PyEvent__SetSelf(*args, **kwargs)

    def _GetSelf(*args, **kwargs):
        """_GetSelf(self) -> PyObject"""
        return _core_.PyEvent__GetSelf(*args, **kwargs)

_core_.PyEvent_swigregister(PyEvent)

class PyCommandEvent(CommandEvent):
    """
    wx.PyCommandEvent can be used as a base class for implementing custom
    event types in Python, where the event shoudl travel up to parent
    windows looking for a handler.  You should derived from this class
    instead of `wx.CommandEvent` because this class is Python-aware and is
    able to transport its Python bits safely through the wxWidgets event
    system and have them still be there when the event handler is invoked.

    :see: `wx.PyEvent`

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EventType eventType=wxEVT_NULL, int id=0) -> PyCommandEvent"""
        _core_.PyCommandEvent_swiginit(self,_core_.new_PyCommandEvent(*args, **kwargs))
        self._SetSelf(self)

    __swig_destroy__ = _core_.delete_PyCommandEvent
    __del__ = lambda self : None;
    def _SetSelf(*args, **kwargs):
        """_SetSelf(self, PyObject self)"""
        return _core_.PyCommandEvent__SetSelf(*args, **kwargs)

    def _GetSelf(*args, **kwargs):
        """_GetSelf(self) -> PyObject"""
        return _core_.PyCommandEvent__GetSelf(*args, **kwargs)

_core_.PyCommandEvent_swigregister(PyCommandEvent)

class DateEvent(CommandEvent):
    """
    This event class holds information about a date change event and is
    used together with `wx.DatePickerCtrl`. It also serves as a base class
    for `wx.calendar.CalendarEvent`.  Bind these event types with
    EVT_DATE_CHANGED.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Window win, DateTime dt, EventType type) -> DateEvent"""
        _core_.DateEvent_swiginit(self,_core_.new_DateEvent(*args, **kwargs))
    def GetDate(*args, **kwargs):
        """
        GetDate(self) -> DateTime

        Returns the date.
        """
        return _core_.DateEvent_GetDate(*args, **kwargs)

    def SetDate(*args, **kwargs):
        """
        SetDate(self, DateTime date)

        Sets the date carried by the event, normally only used by the library
        internally.
        """
        return _core_.DateEvent_SetDate(*args, **kwargs)

    Date = property(GetDate,SetDate,doc="See `GetDate` and `SetDate`") 
_core_.DateEvent_swigregister(DateEvent)

wxEVT_DATE_CHANGED = _core_.wxEVT_DATE_CHANGED
EVT_DATE_CHANGED = wx.PyEventBinder( wxEVT_DATE_CHANGED, 1 )

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> PyApp

        Create a new application object, starting the bootstrap process.
        """
        _core_.PyApp_swiginit(self,_core_.new_PyApp(*args, **kwargs))
        self._setCallbackInfo(self, PyApp, False)
        self._setOORInfo(self, False)

    __swig_destroy__ = _core_.delete_PyApp
    __del__ = lambda self : None;
    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class, bool incref)"""
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

    def GetLayoutDirection(*args, **kwargs):
        """
        GetLayoutDirection(self) -> int

        Return the layout direction for the current locale.
        """
        return _core_.PyApp_GetLayoutDirection(*args, **kwargs)

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
    def IsDisplayAvailable(*args, **kwargs):
        """
        IsDisplayAvailable() -> bool

        Tests if it is possible to create a GUI in the current environment.
        This will mean different things on the different platforms.

           * On X Windows systems this function will return ``False`` if it is
             not able to open a connection to the X display, which can happen
             if $DISPLAY is not set, or is not set correctly.

           * On Mac OS X a ``False`` return value will mean that wx is not
             able to access the window manager, which can happen if logged in
             remotely or if running from the normal version of python instead
             of the framework version, (i.e., pythonw.)

           * On MS Windows...

        """
        return _core_.PyApp_IsDisplayAvailable(*args, **kwargs)

    IsDisplayAvailable = staticmethod(IsDisplayAvailable)
    AppName = property(GetAppName,SetAppName,doc="See `GetAppName` and `SetAppName`") 
    AssertMode = property(GetAssertMode,SetAssertMode,doc="See `GetAssertMode` and `SetAssertMode`") 
    ClassName = property(GetClassName,SetClassName,doc="See `GetClassName` and `SetClassName`") 
    ExitOnFrameDelete = property(GetExitOnFrameDelete,SetExitOnFrameDelete,doc="See `GetExitOnFrameDelete` and `SetExitOnFrameDelete`") 
    LayoutDirection = property(GetLayoutDirection,doc="See `GetLayoutDirection`") 
    PrintMode = property(GetPrintMode,SetPrintMode,doc="See `GetPrintMode` and `SetPrintMode`") 
    TopWindow = property(GetTopWindow,SetTopWindow,doc="See `GetTopWindow` and `SetTopWindow`") 
    Traits = property(GetTraits,doc="See `GetTraits`") 
    UseBestVisual = property(GetUseBestVisual,SetUseBestVisual,doc="See `GetUseBestVisual` and `SetUseBestVisual`") 
    VendorName = property(GetVendorName,SetVendorName,doc="See `GetVendorName` and `SetVendorName`") 
    Active = property(IsActive) 
_core_.PyApp_swigregister(PyApp)

def PyApp_IsMainLoopRunning(*args):
  """
    PyApp_IsMainLoopRunning() -> bool

    Returns True if we're running the main loop, i.e. if the events can
    currently be dispatched.
    """
  return _core_.PyApp_IsMainLoopRunning(*args)

def PyApp_GetMacSupportPCMenuShortcuts(*args):
  """PyApp_GetMacSupportPCMenuShortcuts() -> bool"""
  return _core_.PyApp_GetMacSupportPCMenuShortcuts(*args)

def PyApp_GetMacAboutMenuItemId(*args):
  """PyApp_GetMacAboutMenuItemId() -> long"""
  return _core_.PyApp_GetMacAboutMenuItemId(*args)

def PyApp_GetMacPreferencesMenuItemId(*args):
  """PyApp_GetMacPreferencesMenuItemId() -> long"""
  return _core_.PyApp_GetMacPreferencesMenuItemId(*args)

def PyApp_GetMacExitMenuItemId(*args):
  """PyApp_GetMacExitMenuItemId() -> long"""
  return _core_.PyApp_GetMacExitMenuItemId(*args)

def PyApp_GetMacHelpMenuTitleName(*args):
  """PyApp_GetMacHelpMenuTitleName() -> String"""
  return _core_.PyApp_GetMacHelpMenuTitleName(*args)

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

def PyApp_GetComCtl32Version(*args):
  """
    PyApp_GetComCtl32Version() -> int

    Returns 400, 470, 471, etc. for comctl32.dll 4.00, 4.70, 4.71 or 0 if
    it wasn't found at all.  Raises an exception on non-Windows platforms.
    """
  return _core_.PyApp_GetComCtl32Version(*args)

def PyApp_IsDisplayAvailable(*args):
  """
    PyApp_IsDisplayAvailable() -> bool

    Tests if it is possible to create a GUI in the current environment.
    This will mean different things on the different platforms.

       * On X Windows systems this function will return ``False`` if it is
         not able to open a connection to the X display, which can happen
         if $DISPLAY is not set, or is not set correctly.

       * On Mac OS X a ``False`` return value will mean that wx is not
         able to access the window manager, which can happen if logged in
         remotely or if running from the normal version of python instead
         of the framework version, (i.e., pythonw.)

       * On MS Windows...

    """
  return _core_.PyApp_IsDisplayAvailable(*args)

#---------------------------------------------------------------------------


def Exit(*args):
  """
    Exit()

    Force an exit of the application.  Convenience for wx.GetApp().Exit()
    """
  return _core_.Exit(*args)

def Yield(*args):
  """
    Yield() -> bool

    Yield to other apps/messages.  Convenience for wx.GetApp().Yield()
    """
  return _core_.Yield(*args)

def YieldIfNeeded(*args):
  """
    YieldIfNeeded() -> bool

    Yield to other apps/messages.  Convenience for wx.GetApp().Yield(True)
    """
  return _core_.YieldIfNeeded(*args)

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

def WakeUpIdle(*args):
  """
    WakeUpIdle()

    Cause the message queue to become empty again, so idle events will be
    sent.
    """
  return _core_.WakeUpIdle(*args)

def PostEvent(*args, **kwargs):
  """
    PostEvent(EvtHandler dest, Event event)

    Send an event to a window or other wx.EvtHandler to be processed
    later.
    """
  return _core_.PostEvent(*args, **kwargs)

def App_CleanUp(*args):
  """
    App_CleanUp()

    For internal use only, it is used to cleanup after wxWidgets when
    Python shuts down.
    """
  return _core_.App_CleanUp(*args)

def GetApp(*args):
  """
    GetApp() -> PyApp

    Return a reference to the current wx.App object.
    """
  return _core_.GetApp(*args)

def SetDefaultPyEncoding(*args, **kwargs):
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
  return _core_.SetDefaultPyEncoding(*args, **kwargs)

def GetDefaultPyEncoding(*args):
  """
    GetDefaultPyEncoding() -> string

    Gets the current encoding that wxPython will use when it needs to
    convert a Python string or unicode object to or from a wxString.
    """
  return _core_.GetDefaultPyEncoding(*args)
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
        self.frame.Bind(wx.EVT_CLOSE, self.OnCloseWindow)
        

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

        # make sure we can create a GUI
        if not self.IsDisplayAvailable():
            
            if wx.Platform == "__WXMAC__":
                msg = """This program needs access to the screen.
Please run with 'pythonw', not 'python', and only when you are logged
in on the main display of your Mac."""
                
            elif wx.Platform == "__WXGTK__":
                msg ="Unable to access the X Display, is $DISPLAY set properly?"

            else:
                msg = "Unable to create GUI"
                # TODO: more description is needed for wxMSW...

            raise SystemExit(msg)
        
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


    def OnPreInit(self):
        """
        Things that must be done after _BootstrapApp has done its
        thing, but would be nice if they were already done by the time
        that OnInit is called.
        """
        wx.StockGDI._initStockObjects()
        

    def __del__(self, destroy=wx.PyApp.__del__):
        self.RestoreStdio()  # Just in case the MainLoop was overridden
        destroy(self)

    def Destroy(self):
        wx.PyApp.Destroy(self)
        self.thisown = 0

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
        try:
            _sys.stdout, _sys.stderr = self.saveStdio
        except:
            pass


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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> EventLoop"""
        _core_.EventLoop_swiginit(self,_core_.new_EventLoop(*args, **kwargs))
    __swig_destroy__ = _core_.delete_EventLoop
    __del__ = lambda self : None;
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
_core_.EventLoop_swigregister(EventLoop)

def EventLoop_GetActive(*args):
  """EventLoop_GetActive() -> EventLoop"""
  return _core_.EventLoop_GetActive(*args)

def EventLoop_SetActive(*args, **kwargs):
  """EventLoop_SetActive(EventLoop loop)"""
  return _core_.EventLoop_SetActive(*args, **kwargs)

class EventLoopActivator(object):
    """Proxy of C++ EventLoopActivator class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EventLoop evtLoop) -> EventLoopActivator"""
        _core_.EventLoopActivator_swiginit(self,_core_.new_EventLoopActivator(*args, **kwargs))
    __swig_destroy__ = _core_.delete_EventLoopActivator
    __del__ = lambda self : None;
_core_.EventLoopActivator_swigregister(EventLoopActivator)

#---------------------------------------------------------------------------

ACCEL_ALT = _core_.ACCEL_ALT
ACCEL_CTRL = _core_.ACCEL_CTRL
ACCEL_SHIFT = _core_.ACCEL_SHIFT
ACCEL_NORMAL = _core_.ACCEL_NORMAL
ACCEL_CMD = _core_.ACCEL_CMD
class AcceleratorEntry(object):
    """
    A class used to define items in an `wx.AcceleratorTable`.  wxPython
    programs can choose to use wx.AcceleratorEntry objects, but using a
    list of 3-tuple of integers (flags, keyCode, cmdID) usually works just
    as well.  See `__init__` for  of the tuple values.

    :see: `wx.AcceleratorTable`
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int flags=0, int keyCode=0, int cmdID=0) -> AcceleratorEntry

        Construct a wx.AcceleratorEntry.
        """
        _core_.AcceleratorEntry_swiginit(self,_core_.new_AcceleratorEntry(*args, **kwargs))
    __swig_destroy__ = _core_.delete_AcceleratorEntry
    __del__ = lambda self : None;
    def Set(*args, **kwargs):
        """
        Set(self, int flags, int keyCode, int cmd)

        (Re)set the attributes of a wx.AcceleratorEntry.
        :see `__init__`
        """
        return _core_.AcceleratorEntry_Set(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(String str) -> AcceleratorEntry

        Create accelerator corresponding to the specified string, or None if
        it coulnd't be parsed.
        """
        return _core_.AcceleratorEntry_Create(*args, **kwargs)

    Create = staticmethod(Create)
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

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _core_.AcceleratorEntry_IsOk(*args, **kwargs)

    def ToString(*args, **kwargs):
        """
        ToString(self) -> String

        Returns a string representation for the this accelerator.  The string
        is formatted using the <flags>-<keycode> format where <flags> maybe a
        hyphen-separed list of "shift|alt|ctrl"

        """
        return _core_.AcceleratorEntry_ToString(*args, **kwargs)

    def FromString(*args, **kwargs):
        """
        FromString(self, String str) -> bool

        Returns true if the given string correctly initialized this object.
        """
        return _core_.AcceleratorEntry_FromString(*args, **kwargs)

    Command = property(GetCommand,doc="See `GetCommand`") 
    Flags = property(GetFlags,doc="See `GetFlags`") 
    KeyCode = property(GetKeyCode,doc="See `GetKeyCode`") 
_core_.AcceleratorEntry_swigregister(AcceleratorEntry)

def AcceleratorEntry_Create(*args, **kwargs):
  """
    AcceleratorEntry_Create(String str) -> AcceleratorEntry

    Create accelerator corresponding to the specified string, or None if
    it coulnd't be parsed.
    """
  return _core_.AcceleratorEntry_Create(*args, **kwargs)

class AcceleratorTable(Object):
    """
    An accelerator table allows the application to specify a table of
    keyboard shortcuts for menus or other commands. On Windows, menu or
    button commands are supported; on GTK, only menu commands are
    supported.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(entries) -> AcceleratorTable

        Construct an AcceleratorTable from a list of `wx.AcceleratorEntry`
        items or or of 3-tuples (flags, keyCode, cmdID)

        :see: `wx.AcceleratorEntry`
        """
        _core_.AcceleratorTable_swiginit(self,_core_.new_AcceleratorTable(*args, **kwargs))
    __swig_destroy__ = _core_.delete_AcceleratorTable
    __del__ = lambda self : None;
    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _core_.AcceleratorTable_Ok(*args, **kwargs)

_core_.AcceleratorTable_swigregister(AcceleratorTable)


def GetAccelFromString(*args, **kwargs):
  """GetAccelFromString(String label) -> AcceleratorEntry"""
  return _core_.GetAccelFromString(*args, **kwargs)
#---------------------------------------------------------------------------

class VisualAttributes(object):
    """struct containing all the visual attributes of a control"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> VisualAttributes

        struct containing all the visual attributes of a control
        """
        _core_.VisualAttributes_swiginit(self,_core_.new_VisualAttributes(*args, **kwargs))
    __swig_destroy__ = _core_.delete_VisualAttributes
    __del__ = lambda self : None;
    font = property(_core_.VisualAttributes_font_get, _core_.VisualAttributes_font_set)
    colFg = property(_core_.VisualAttributes_colFg_get, _core_.VisualAttributes_colFg_set)
    colBg = property(_core_.VisualAttributes_colBg_get, _core_.VisualAttributes_colBg_set)
_core_.VisualAttributes_swigregister(VisualAttributes)
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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=PanelNameStr) -> Window

        Construct and show a generic Window.
        """
        _core_.Window_swiginit(self,_core_.new_Window(*args, **kwargs))
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
        val = _core_.Window_Destroy(*args, **kwargs)
        args[0].thisown = 0
        return val

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
    def GetLayoutDirection(*args, **kwargs):
        """
        GetLayoutDirection(self) -> int

        Get the layout direction (LTR or RTL) for this window.  Returns
        ``wx.Layout_Default`` if layout direction is not supported.
        """
        return _core_.Window_GetLayoutDirection(*args, **kwargs)

    def SetLayoutDirection(*args, **kwargs):
        """
        SetLayoutDirection(self, int dir)

        Set the layout direction (LTR or RTL) for this window.
        """
        return _core_.Window_SetLayoutDirection(*args, **kwargs)

    def AdjustForLayoutDirection(*args, **kwargs):
        """
        AdjustForLayoutDirection(self, int x, int width, int widthTotal) -> int

        Mirror coordinates for RTL layout if this window uses it and if the
        mirroring is not done automatically like Win32.
        """
        return _core_.Window_AdjustForLayoutDirection(*args, **kwargs)

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

        Raises the window to the top of the window hierarchy.  In current
        version of wxWidgets this works both for manage and child windows.
        """
        return _core_.Window_Raise(*args, **kwargs)

    def Lower(*args, **kwargs):
        """
        Lower(self)

        Lowers the window to the bottom of the window hierarchy.  In current
        version of wxWidgets this works both for manage and child windows.
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

        Get the window's position.  Notice that the position is in client
        coordinates for child windows and screen coordinates for the top level
        ones, use `GetScreenPosition` if you need screen coordinates for all
        kinds of windows.
        """
        return _core_.Window_GetPosition(*args, **kwargs)

    def GetPositionTuple(*args, **kwargs):
        """
        GetPositionTuple() -> (x,y)

        Get the window's position.  Notice that the position is in client
        coordinates for child windows and screen coordinates for the top level
        ones, use `GetScreenPosition` if you need screen coordinates for all
        kinds of windows.
        """
        return _core_.Window_GetPositionTuple(*args, **kwargs)

    def GetScreenPosition(*args, **kwargs):
        """
        GetScreenPosition(self) -> Point

        Get the position of the window in screen coordinantes.
        """
        return _core_.Window_GetScreenPosition(*args, **kwargs)

    def GetScreenPositionTuple(*args, **kwargs):
        """
        GetScreenPositionTuple() -> (x,y)

        Get the position of the window in screen coordinantes.
        """
        return _core_.Window_GetScreenPositionTuple(*args, **kwargs)

    def GetScreenRect(*args, **kwargs):
        """
        GetScreenRect(self) -> Rect

        Returns the size and position of the window in screen coordinantes as
        a `wx.Rect` object.
        """
        return _core_.Window_GetScreenRect(*args, **kwargs)

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

        Returns the size and position of the window as a `wx.Rect` object.
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

    def CacheBestSize(*args, **kwargs):
        """
        CacheBestSize(self, Size size)

        Cache the best size so it doesn't need to be calculated again, (at least until
        some properties of the window change.)
        """
        return _core_.Window_CacheBestSize(*args, **kwargs)

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

    def IsShownOnScreen(*args, **kwargs):
        """
        IsShownOnScreen(self) -> bool

        Returns ``True`` if the window is physically visible on the screen,
        i.e. it is shown and all its parents up to the toplevel window are
        shown as well.
        """
        return _core_.Window_IsShownOnScreen(*args, **kwargs)

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

    def SetDoubleBuffered(*args, **kwargs):
        """
        SetDoubleBuffered(self, bool on)

        Currently wxGTK2 only.
        """
        return _core_.Window_SetDoubleBuffered(*args, **kwargs)

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
        that is capable of processing the events sent to a window.  (In other
        words, is able to dispatch the events to handler function.)  By
        default, the window is its own event handler, but an application may
        wish to substitute another, for example to allow central
        implementation of event-handling for a variety of different window
        classes.

        It is usually better to use `wx.Window.PushEventHandler` since this sets
        up a chain of event handlers, where an event not handled by one event
        handler is handed off to the next one in the chain.
        """
        return _core_.Window_SetEventHandler(*args, **kwargs)

    def PushEventHandler(*args, **kwargs):
        """
        PushEventHandler(self, EvtHandler handler)

        Pushes this event handler onto the event handler stack for the window.
        An event handler is an object that is capable of processing the events
        sent to a window.  (In other words, is able to dispatch the events to
        handler function.)  By default, the window is its own event handler,
        but an application may wish to substitute another, for example to
        allow central implementation of event-handling for a variety of
        different window classes.

        wx.Window.PushEventHandler allows an application to set up a chain of
        event handlers, where an event not handled by one event handler is
        handed to the next one in the chain.  Use `wx.Window.PopEventHandler`
        to remove the event handler.  Ownership of the handler is *not* given
        to the window, so you should be sure to pop the handler before the
        window is destroyed and either let PopEventHandler destroy it, or call
        its Destroy method yourself.
        """
        return _core_.Window_PushEventHandler(*args, **kwargs)

    def PopEventHandler(*args, **kwargs):
        """
        PopEventHandler(self, bool deleteHandler=False) -> EvtHandler

        Removes and returns the top-most event handler on the event handler
        stack.  If deleteHandler is True then the wx.EvtHandler object will be
        destroyed after it is popped, and ``None`` will be returned instead.
        """
        return _core_.Window_PopEventHandler(*args, **kwargs)

    def RemoveEventHandler(*args, **kwargs):
        """
        RemoveEventHandler(self, EvtHandler handler) -> bool

        Find the given handler in the event handler chain and remove (but not
        delete) it from the event handler chain, returns True if it was found
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
        release the mouse as many times as you capture it, unless the window
        receives the `wx.MouseCaptureLostEvent` event.
         
        Any application which captures the mouse in the beginning of some
        operation *must* handle `wx.MouseCaptureLostEvent` and cancel this
        operation when it receives the event. The event handler must not
        recapture mouse.
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
        RefreshRect(self, Rect rect, bool eraseBackground=True)

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

    def IsFrozen(*args, **kwargs):
        """
        IsFrozen(self) -> bool

        Returns ``True`` if the window has been frozen and not thawed yet.

        :see: `Freeze` and `Thaw`
        """
        return _core_.Window_IsFrozen(*args, **kwargs)

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

    def InheritsBackgroundColour(*args, **kwargs):
        """InheritsBackgroundColour(self) -> bool"""
        return _core_.Window_InheritsBackgroundColour(*args, **kwargs)

    def UseBgCol(*args, **kwargs):
        """UseBgCol(self) -> bool"""
        return _core_.Window_UseBgCol(*args, **kwargs)

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
        return _core_.Window_HasTransparentBackground(*args, **kwargs)

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
        `wx.UpdateUIEvent.SetMode` or `wx.UpdateUIEvent.SetUpdateInterval` to
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

    def HasMultiplePages(*args, **kwargs):
        """HasMultiplePages(self) -> bool"""
        return _core_.Window_HasMultiplePages(*args, **kwargs)

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

    def OnPaint(*args, **kwargs):
        """OnPaint(self, PaintEvent event)"""
        return _core_.Window_OnPaint(*args, **kwargs)

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
        `wx.HelpProvider` implementation, and not in the window object itself.
        """
        return _core_.Window_SetHelpText(*args, **kwargs)

    def SetHelpTextForId(*args, **kwargs):
        """
        SetHelpTextForId(self, String text)

        Associate this help text with all windows with the same id as this
        one.
        """
        return _core_.Window_SetHelpTextForId(*args, **kwargs)

    def GetHelpTextAtPoint(*args, **kwargs):
        """
        GetHelpTextAtPoint(self, Point pt, wxHelpEvent::Origin origin) -> String

        Get the help string associated with the given position in this window.

        Notice that pt may be invalid if event origin is keyboard or unknown
        and this method should return the global window help text then

        """
        return _core_.Window_GetHelpTextAtPoint(*args, **kwargs)

    def GetHelpText(*args, **kwargs):
        """
        GetHelpText(self) -> String

        Gets the help text to be used as context-sensitive help for this
        window.  Note that the text is actually stored by the current
        `wx.HelpProvider` implementation, and not in the window object itself.
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

    def DragAcceptFiles(*args, **kwargs):
        """
        DragAcceptFiles(self, bool accept)

        Enables or disables eligibility for drop file events, EVT_DROP_FILES.
        Only functional on Windows.
        """
        return _core_.Window_DragAcceptFiles(*args, **kwargs)

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
        when the window is resized.  lease note that this only happens for the
        windows usually used to contain children, namely `wx.Panel` and
        `wx.TopLevelWindow` (and the classes deriving from them).

        This method is called implicitly by `SetSizer` but if you use
        `SetConstraints` you should call it manually or otherwise the window
        layout won't be correctly updated when its size changes.
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

    def CanSetTransparent(*args, **kwargs):
        """
        CanSetTransparent(self) -> bool

        Returns ``True`` if the platform supports setting the transparency for
        this window.  Note that this method will err on the side of caution,
        so it is possible that this will return ``False`` when it is in fact
        possible to set the transparency.

        NOTE: On X-windows systems the X server must have the composite
        extension loaded, and there must be a composite manager program (such
        as xcompmgr) running.
        """
        return _core_.Window_CanSetTransparent(*args, **kwargs)

    def SetTransparent(*args, **kwargs):
        """
        SetTransparent(self, byte alpha) -> bool

        Attempt to set the transparency of this window to the ``alpha`` value,
        returns True on success.  The ``alpha`` value is an integer in the
        range of 0 to 255, where 0 is fully transparent and 255 is fully
        opaque.
        """
        return _core_.Window_SetTransparent(*args, **kwargs)

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

    def SendSizeEvent(self):
        self.GetEventHandler().ProcessEvent(wx.SizeEvent((-1,-1)))

    AcceleratorTable = property(GetAcceleratorTable,SetAcceleratorTable,doc="See `GetAcceleratorTable` and `SetAcceleratorTable`") 
    AdjustedBestSize = property(GetAdjustedBestSize,doc="See `GetAdjustedBestSize`") 
    AutoLayout = property(GetAutoLayout,SetAutoLayout,doc="See `GetAutoLayout` and `SetAutoLayout`") 
    BackgroundColour = property(GetBackgroundColour,SetBackgroundColour,doc="See `GetBackgroundColour` and `SetBackgroundColour`") 
    BackgroundStyle = property(GetBackgroundStyle,SetBackgroundStyle,doc="See `GetBackgroundStyle` and `SetBackgroundStyle`") 
    BestFittingSize = property(GetBestFittingSize,SetBestFittingSize,doc="See `GetBestFittingSize` and `SetBestFittingSize`") 
    BestSize = property(GetBestSize,doc="See `GetBestSize`") 
    BestVirtualSize = property(GetBestVirtualSize,doc="See `GetBestVirtualSize`") 
    Border = property(GetBorder,doc="See `GetBorder`") 
    Caret = property(GetCaret,SetCaret,doc="See `GetCaret` and `SetCaret`") 
    CharHeight = property(GetCharHeight,doc="See `GetCharHeight`") 
    CharWidth = property(GetCharWidth,doc="See `GetCharWidth`") 
    Children = property(GetChildren,doc="See `GetChildren`") 
    ClientAreaOrigin = property(GetClientAreaOrigin,doc="See `GetClientAreaOrigin`") 
    ClientRect = property(GetClientRect,SetClientRect,doc="See `GetClientRect` and `SetClientRect`") 
    ClientSize = property(GetClientSize,SetClientSize,doc="See `GetClientSize` and `SetClientSize`") 
    Constraints = property(GetConstraints,SetConstraints,doc="See `GetConstraints` and `SetConstraints`") 
    ContainingSizer = property(GetContainingSizer,SetContainingSizer,doc="See `GetContainingSizer` and `SetContainingSizer`") 
    Cursor = property(GetCursor,SetCursor,doc="See `GetCursor` and `SetCursor`") 
    DefaultAttributes = property(GetDefaultAttributes,doc="See `GetDefaultAttributes`") 
    DropTarget = property(GetDropTarget,SetDropTarget,doc="See `GetDropTarget` and `SetDropTarget`") 
    EventHandler = property(GetEventHandler,SetEventHandler,doc="See `GetEventHandler` and `SetEventHandler`") 
    ExtraStyle = property(GetExtraStyle,SetExtraStyle,doc="See `GetExtraStyle` and `SetExtraStyle`") 
    Font = property(GetFont,SetFont,doc="See `GetFont` and `SetFont`") 
    ForegroundColour = property(GetForegroundColour,SetForegroundColour,doc="See `GetForegroundColour` and `SetForegroundColour`") 
    GrandParent = property(GetGrandParent,doc="See `GetGrandParent`") 
    Handle = property(GetHandle,doc="See `GetHandle`") 
    HelpText = property(GetHelpText,SetHelpText,doc="See `GetHelpText` and `SetHelpText`") 
    Id = property(GetId,SetId,doc="See `GetId` and `SetId`") 
    Label = property(GetLabel,SetLabel,doc="See `GetLabel` and `SetLabel`") 
    LayoutDirection = property(GetLayoutDirection,SetLayoutDirection,doc="See `GetLayoutDirection` and `SetLayoutDirection`") 
    MaxHeight = property(GetMaxHeight,doc="See `GetMaxHeight`") 
    MaxSize = property(GetMaxSize,SetMaxSize,doc="See `GetMaxSize` and `SetMaxSize`") 
    MaxWidth = property(GetMaxWidth,doc="See `GetMaxWidth`") 
    MinHeight = property(GetMinHeight,doc="See `GetMinHeight`") 
    MinSize = property(GetMinSize,SetMinSize,doc="See `GetMinSize` and `SetMinSize`") 
    MinWidth = property(GetMinWidth,doc="See `GetMinWidth`") 
    Name = property(GetName,SetName,doc="See `GetName` and `SetName`") 
    Parent = property(GetParent,doc="See `GetParent`") 
    Position = property(GetPosition,SetPosition,doc="See `GetPosition` and `SetPosition`") 
    Rect = property(GetRect,SetRect,doc="See `GetRect` and `SetRect`") 
    ScreenPosition = property(GetScreenPosition,doc="See `GetScreenPosition`") 
    ScreenRect = property(GetScreenRect,doc="See `GetScreenRect`") 
    Size = property(GetSize,SetSize,doc="See `GetSize` and `SetSize`") 
    Sizer = property(GetSizer,SetSizer,doc="See `GetSizer` and `SetSizer`") 
    ThemeEnabled = property(GetThemeEnabled,SetThemeEnabled,doc="See `GetThemeEnabled` and `SetThemeEnabled`") 
    ToolTip = property(GetToolTip,SetToolTip,doc="See `GetToolTip` and `SetToolTip`") 
    UpdateClientRect = property(GetUpdateClientRect,doc="See `GetUpdateClientRect`") 
    UpdateRegion = property(GetUpdateRegion,doc="See `GetUpdateRegion`") 
    Validator = property(GetValidator,SetValidator,doc="See `GetValidator` and `SetValidator`") 
    VirtualSize = property(GetVirtualSize,SetVirtualSize,doc="See `GetVirtualSize` and `SetVirtualSize`") 
    WindowStyle = property(GetWindowStyle,SetWindowStyle,doc="See `GetWindowStyle` and `SetWindowStyle`") 
    WindowStyleFlag = property(GetWindowStyleFlag,SetWindowStyleFlag,doc="See `GetWindowStyleFlag` and `SetWindowStyleFlag`") 
    WindowVariant = property(GetWindowVariant,SetWindowVariant,doc="See `GetWindowVariant` and `SetWindowVariant`") 
    Shown = property(IsShown,Show,doc="See `IsShown` and `Show`") 
    Enabled = property(IsEnabled,Enable,doc="See `IsEnabled` and `Enable`") 
    TopLevel = property(IsTopLevel,doc="See `IsTopLevel`") 
_core_.Window_swigregister(Window)

def PreWindow(*args, **kwargs):
    """
    PreWindow() -> Window

    Precreate a Window for 2-phase creation.
    """
    val = _core_.new_PreWindow(*args, **kwargs)
    return val

def Window_NewControlId(*args):
  """
    Window_NewControlId() -> int

    Generate a control id for the controls which were not given one.
    """
  return _core_.Window_NewControlId(*args)

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

def Window_FindFocus(*args):
  """
    Window_FindFocus() -> Window

    Returns the window or control that currently has the keyboard focus,
    or None.
    """
  return _core_.Window_FindFocus(*args)

def Window_GetCapture(*args):
  """
    Window_GetCapture() -> Window

    Returns the window which currently captures the mouse or None
    """
  return _core_.Window_GetCapture(*args)

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

def GetTopLevelWindows(*args):
  """
    GetTopLevelWindows() -> PyObject

    Returns a list of the the application's top-level windows, (frames,
    dialogs, etc.)  NOTE: Currently this is a copy of the list maintained
    by wxWidgets, and so it is only valid as long as no top-level windows
    are closed or new top-level windows are created.

    """
  return _core_.GetTopLevelWindows(*args)
#---------------------------------------------------------------------------

class Validator(EvtHandler):
    """Proxy of C++ Validator class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> Validator"""
        _core_.Validator_swiginit(self,_core_.new_Validator(*args, **kwargs))
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
    Window = property(GetWindow,SetWindow,doc="See `GetWindow` and `SetWindow`") 
_core_.Validator_swigregister(Validator)

def Validator_IsSilent(*args):
  """Validator_IsSilent() -> bool"""
  return _core_.Validator_IsSilent(*args)

def Validator_SetBellOnError(*args, **kwargs):
  """Validator_SetBellOnError(int doIt=True)"""
  return _core_.Validator_SetBellOnError(*args, **kwargs)

class PyValidator(Validator):
    """Proxy of C++ PyValidator class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> PyValidator"""
        _core_.PyValidator_swiginit(self,_core_.new_PyValidator(*args, **kwargs))
        
        self._setCallbackInfo(self, PyValidator, 1)
        self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class, int incref=True)"""
        return _core_.PyValidator__setCallbackInfo(*args, **kwargs)

_core_.PyValidator_swigregister(PyValidator)

#---------------------------------------------------------------------------

class Menu(EvtHandler):
    """Proxy of C++ Menu class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String title=EmptyString, long style=0) -> Menu"""
        _core_.Menu_swiginit(self,_core_.new_Menu(*args, **kwargs))
        self._setOORInfo(self)

    def Append(*args, **kwargs):
        """
        Append(self, int id, String text=EmptyString, String help=EmptyString, 
            int kind=ITEM_NORMAL) -> MenuItem
        """
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

    def AppendSubMenu(*args, **kwargs):
        """AppendSubMenu(self, Menu submenu, String text, String help=EmptyString) -> MenuItem"""
        return _core_.Menu_AppendSubMenu(*args, **kwargs)

    def AppendItem(*args, **kwargs):
        """AppendItem(self, MenuItem item) -> MenuItem"""
        return _core_.Menu_AppendItem(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """InsertItem(self, size_t pos, MenuItem item) -> MenuItem"""
        return _core_.Menu_InsertItem(*args, **kwargs)

    def PrependItem(*args, **kwargs):
        """PrependItem(self, MenuItem item) -> MenuItem"""
        return _core_.Menu_PrependItem(*args, **kwargs)

    def Break(*args, **kwargs):
        """Break(self)"""
        return _core_.Menu_Break(*args, **kwargs)

    def Insert(*args, **kwargs):
        """
        Insert(self, size_t pos, int id, String text=EmptyString, String help=EmptyString, 
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

    def Prepend(*args, **kwargs):
        """
        Prepend(self, int id, String text=EmptyString, String help=EmptyString, 
            int kind=ITEM_NORMAL) -> MenuItem
        """
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
        val = _core_.Menu_Destroy(*args, **kwargs)
        args[0].thisown = 0
        return val

    def DestroyId(*args, **kwargs):
        """
        DestroyId(self, int id) -> bool

        Deletes the C++ object this Python object is a proxy for.
        """
        val = _core_.Menu_DestroyId(*args, **kwargs)
        args[0].thisown = 0
        return val

    def DestroyItem(*args, **kwargs):
        """
        DestroyItem(self, MenuItem item) -> bool

        Deletes the C++ object this Python object is a proxy for.
        """
        val = _core_.Menu_DestroyItem(*args, **kwargs)
        args[0].thisown = 0
        return val

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

    EventHandler = property(GetEventHandler,SetEventHandler,doc="See `GetEventHandler` and `SetEventHandler`") 
    HelpString = property(GetHelpString,SetHelpString,doc="See `GetHelpString` and `SetHelpString`") 
    InvokingWindow = property(GetInvokingWindow,SetInvokingWindow,doc="See `GetInvokingWindow` and `SetInvokingWindow`") 
    MenuBar = property(GetMenuBar,doc="See `GetMenuBar`") 
    MenuItemCount = property(GetMenuItemCount,doc="See `GetMenuItemCount`") 
    MenuItems = property(GetMenuItems,doc="See `GetMenuItems`") 
    Parent = property(GetParent,SetParent,doc="See `GetParent` and `SetParent`") 
    Style = property(GetStyle,doc="See `GetStyle`") 
    Title = property(GetTitle,SetTitle,doc="See `GetTitle` and `SetTitle`") 
_core_.Menu_swigregister(Menu)
DefaultValidator = cvar.DefaultValidator

#---------------------------------------------------------------------------

class MenuBar(Window):
    """Proxy of C++ MenuBar class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, long style=0) -> MenuBar"""
        _core_.MenuBar_swiginit(self,_core_.new_MenuBar(*args, **kwargs))
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

    def UpdateMenus(*args, **kwargs):
        """UpdateMenus(self)"""
        return _core_.MenuBar_UpdateMenus(*args, **kwargs)

    def SetAutoWindowMenu(*args, **kwargs):
        """SetAutoWindowMenu(bool enable)"""
        return _core_.MenuBar_SetAutoWindowMenu(*args, **kwargs)

    SetAutoWindowMenu = staticmethod(SetAutoWindowMenu)
    def GetAutoWindowMenu(*args, **kwargs):
        """GetAutoWindowMenu() -> bool"""
        return _core_.MenuBar_GetAutoWindowMenu(*args, **kwargs)

    GetAutoWindowMenu = staticmethod(GetAutoWindowMenu)
    def GetMenus(self):
        """Return a list of (menu, label) items for the menus in the MenuBar. """
        return [(self.GetMenu(i), self.GetLabelTop(i)) 
                for i in range(self.GetMenuCount())]
        
    def SetMenus(self, items):
        """Clear and add new menus to the MenuBar from a list of (menu, label) items. """
        for i in range(self.GetMenuCount()-1, -1, -1):
            self.Remove(i)
        for m, l in items:
            self.Append(m, l)

    Frame = property(GetFrame,doc="See `GetFrame`") 
    Menu = property(GetMenu,doc="See `GetMenu`") 
    MenuCount = property(GetMenuCount,doc="See `GetMenuCount`") 
    Menus = property(GetMenus,SetMenus,doc="See `GetMenus` and `SetMenus`") 
_core_.MenuBar_swigregister(MenuBar)

def MenuBar_SetAutoWindowMenu(*args, **kwargs):
  """MenuBar_SetAutoWindowMenu(bool enable)"""
  return _core_.MenuBar_SetAutoWindowMenu(*args, **kwargs)

def MenuBar_GetAutoWindowMenu(*args):
  """MenuBar_GetAutoWindowMenu() -> bool"""
  return _core_.MenuBar_GetAutoWindowMenu(*args)

#---------------------------------------------------------------------------

class MenuItem(Object):
    """Proxy of C++ MenuItem class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Menu parentMenu=None, int id=ID_ANY, String text=EmptyString, 
            String help=EmptyString, int kind=ITEM_NORMAL, 
            Menu subMenu=None) -> MenuItem
        """
        _core_.MenuItem_swiginit(self,_core_.new_MenuItem(*args, **kwargs))
    __swig_destroy__ = _core_.delete_MenuItem
    __del__ = lambda self : None;
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

    def SetBitmap(*args, **kwargs):
        """SetBitmap(self, Bitmap bitmap)"""
        return _core_.MenuItem_SetBitmap(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap(self) -> Bitmap"""
        return _core_.MenuItem_GetBitmap(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(self, Font font)"""
        return _core_.MenuItem_SetFont(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(self) -> Font"""
        return _core_.MenuItem_GetFont(*args, **kwargs)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(self, Colour colText)"""
        return _core_.MenuItem_SetTextColour(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour(self) -> Colour"""
        return _core_.MenuItem_GetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(self, Colour colBack)"""
        return _core_.MenuItem_SetBackgroundColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self) -> Colour"""
        return _core_.MenuItem_GetBackgroundColour(*args, **kwargs)

    def SetBitmaps(*args, **kwargs):
        """SetBitmaps(self, Bitmap bmpChecked, Bitmap bmpUnchecked=wxNullBitmap)"""
        return _core_.MenuItem_SetBitmaps(*args, **kwargs)

    def SetDisabledBitmap(*args, **kwargs):
        """SetDisabledBitmap(self, Bitmap bmpDisabled)"""
        return _core_.MenuItem_SetDisabledBitmap(*args, **kwargs)

    def GetDisabledBitmap(*args, **kwargs):
        """GetDisabledBitmap(self) -> Bitmap"""
        return _core_.MenuItem_GetDisabledBitmap(*args, **kwargs)

    def SetMarginWidth(*args, **kwargs):
        """SetMarginWidth(self, int nWidth)"""
        return _core_.MenuItem_SetMarginWidth(*args, **kwargs)

    def GetMarginWidth(*args, **kwargs):
        """GetMarginWidth(self) -> int"""
        return _core_.MenuItem_GetMarginWidth(*args, **kwargs)

    def GetDefaultMarginWidth(*args, **kwargs):
        """GetDefaultMarginWidth() -> int"""
        return _core_.MenuItem_GetDefaultMarginWidth(*args, **kwargs)

    GetDefaultMarginWidth = staticmethod(GetDefaultMarginWidth)
    def IsOwnerDrawn(*args, **kwargs):
        """IsOwnerDrawn(self) -> bool"""
        return _core_.MenuItem_IsOwnerDrawn(*args, **kwargs)

    def SetOwnerDrawn(*args, **kwargs):
        """SetOwnerDrawn(self, bool ownerDrawn=True)"""
        return _core_.MenuItem_SetOwnerDrawn(*args, **kwargs)

    def ResetOwnerDrawn(*args, **kwargs):
        """ResetOwnerDrawn(self)"""
        return _core_.MenuItem_ResetOwnerDrawn(*args, **kwargs)

    Accel = property(GetAccel,SetAccel,doc="See `GetAccel` and `SetAccel`") 
    BackgroundColour = property(GetBackgroundColour,SetBackgroundColour,doc="See `GetBackgroundColour` and `SetBackgroundColour`") 
    Bitmap = property(GetBitmap,SetBitmap,doc="See `GetBitmap` and `SetBitmap`") 
    DisabledBitmap = property(GetDisabledBitmap,SetDisabledBitmap,doc="See `GetDisabledBitmap` and `SetDisabledBitmap`") 
    Font = property(GetFont,SetFont,doc="See `GetFont` and `SetFont`") 
    Help = property(GetHelp,SetHelp,doc="See `GetHelp` and `SetHelp`") 
    Id = property(GetId,SetId,doc="See `GetId` and `SetId`") 
    Kind = property(GetKind,SetKind,doc="See `GetKind` and `SetKind`") 
    Label = property(GetLabel,doc="See `GetLabel`") 
    MarginWidth = property(GetMarginWidth,SetMarginWidth,doc="See `GetMarginWidth` and `SetMarginWidth`") 
    Menu = property(GetMenu,SetMenu,doc="See `GetMenu` and `SetMenu`") 
    SubMenu = property(GetSubMenu,SetSubMenu,doc="See `GetSubMenu` and `SetSubMenu`") 
    Text = property(GetText,SetText,doc="See `GetText` and `SetText`") 
    TextColour = property(GetTextColour,SetTextColour,doc="See `GetTextColour` and `SetTextColour`") 
_core_.MenuItem_swigregister(MenuItem)

def MenuItem_GetLabelFromText(*args, **kwargs):
  """MenuItem_GetLabelFromText(String text) -> String"""
  return _core_.MenuItem_GetLabelFromText(*args, **kwargs)

def MenuItem_GetDefaultMarginWidth(*args):
  """MenuItem_GetDefaultMarginWidth() -> int"""
  return _core_.MenuItem_GetDefaultMarginWidth(*args)

#---------------------------------------------------------------------------

class Control(Window):
    """
    This is the base class for a control or 'widget'.

    A control is generally a small window which processes user input
    and/or displays one or more item of data.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, Validator validator=DefaultValidator, 
            String name=ControlNameStr) -> Control

        Create a Control.  Normally you should only call this from a subclass'
        __init__ as a plain old wx.Control is not very useful.
        """
        _core_.Control_swiginit(self,_core_.new_Control(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, Validator validator=DefaultValidator, 
            String name=ControlNameStr) -> bool

        Do the 2nd phase and create the GUI control.
        """
        return _core_.Control_Create(*args, **kwargs)

    def GetAlignment(*args, **kwargs):
        """
        GetAlignment(self) -> int

        Get the control alignment (left/right/centre, top/bottom/centre)
        """
        return _core_.Control_GetAlignment(*args, **kwargs)

    def GetLabelText(*args, **kwargs):
        """
        GetLabelText(self) -> String

        Get just the text of the label, without mnemonic characters ('&')
        """
        return _core_.Control_GetLabelText(*args, **kwargs)

    def Command(*args, **kwargs):
        """
        Command(self, CommandEvent event)

        Simulates the effect of the user issuing a command to the item.

        :see: `wx.CommandEvent`

        """
        return _core_.Control_Command(*args, **kwargs)

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
    Alignment = property(GetAlignment,doc="See `GetAlignment`") 
    LabelText = property(GetLabelText,doc="See `GetLabelText`") 
_core_.Control_swigregister(Control)
ControlNameStr = cvar.ControlNameStr

def PreControl(*args, **kwargs):
    """
    PreControl() -> Control

    Precreate a Control control for 2-phase creation
    """
    val = _core_.new_PreControl(*args, **kwargs)
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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
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
        Insert(self, String item, unsigned int pos, PyObject clientData=None) -> int

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
        Delete(self, unsigned int n)

        Deletes the item at the zero-based index 'n' from the control. Note
        that it is an error (signalled by a `wx.PyAssertionError` exception if
        enabled) to remove an item with the index negative or greater or equal
        than the number of items in the control.
        """
        return _core_.ItemContainer_Delete(*args, **kwargs)

    def GetClientData(*args, **kwargs):
        """
        GetClientData(self, unsigned int n) -> PyObject

        Returns the client data associated with the given item, (if any.)
        """
        return _core_.ItemContainer_GetClientData(*args, **kwargs)

    def SetClientData(*args, **kwargs):
        """
        SetClientData(self, unsigned int n, PyObject clientData)

        Associate the given client data with the item at position n.
        """
        return _core_.ItemContainer_SetClientData(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """
        GetCount(self) -> unsigned int

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
        GetString(self, unsigned int n) -> String

        Returns the label of the item with the given index.
        """
        return _core_.ItemContainer_GetString(*args, **kwargs)

    def GetStrings(*args, **kwargs):
        """GetStrings(self) -> wxArrayString"""
        return _core_.ItemContainer_GetStrings(*args, **kwargs)

    def SetString(*args, **kwargs):
        """
        SetString(self, unsigned int n, String s)

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

    def SetSelection(*args, **kwargs):
        """
        SetSelection(self, int n)

        Sets the item at index 'n' to be the selected item.
        """
        return _core_.ItemContainer_SetSelection(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """
        GetSelection(self) -> int

        Returns the index of the selected item or ``wx.NOT_FOUND`` if no item
        is selected.
        """
        return _core_.ItemContainer_GetSelection(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """SetStringSelection(self, String s) -> bool"""
        return _core_.ItemContainer_SetStringSelection(*args, **kwargs)

    def GetStringSelection(*args, **kwargs):
        """
        GetStringSelection(self) -> String

        Returns the label of the selected item or an empty string if no item
        is selected.
        """
        return _core_.ItemContainer_GetStringSelection(*args, **kwargs)

    def Select(*args, **kwargs):
        """
        Select(self, int n)

        This is the same as `SetSelection` and exists only because it is
        slightly more natural for controls which support multiple selection.
        """
        return _core_.ItemContainer_Select(*args, **kwargs)

    def GetItems(self):
        """Return a list of the strings in the control"""
        return [self.GetString(i) for i in xrange(self.GetCount())]
        
    def SetItems(self, items):
        """Clear and set the strings in the control from a list"""
        self.Clear()
        for i in items:
            self.Append(i)        

    Count = property(GetCount,doc="See `GetCount`") 
    Items = property(GetItems,SetItems,doc="See `GetItems` and `SetItems`") 
    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
    StringSelection = property(GetStringSelection,SetStringSelection,doc="See `GetStringSelection` and `SetStringSelection`") 
    Strings = property(GetStrings,doc="See `GetStrings`") 
_core_.ItemContainer_swigregister(ItemContainer)

#---------------------------------------------------------------------------

class ControlWithItems(Control,ItemContainer):
    """
    wx.ControlWithItems combines the ``wx.ItemContainer`` class with the
    wx.Control class, and is used for the base class of various controls
    that have items.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
_core_.ControlWithItems_swigregister(ControlWithItems)

#---------------------------------------------------------------------------

class SizerItem(Object):
    """
    The wx.SizerItem class is used to track the position, size and other
    attributes of each item managed by a `wx.Sizer`. It is not usually
    necessary to use this class because the sizer elements can also be
    identified by their positions or window or sizer references but
    sometimes it may be more convenient to use wx.SizerItem directly.
    Also, custom classes derived from `wx.PySizer` will probably need to
    use the collection of wx.SizerItems held by wx.Sizer when calculating
    layout.

    :see: `wx.Sizer`, `wx.GBSizerItem`
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
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
        _core_.SizerItem_swiginit(self,_core_.new_SizerItem(*args, **kwargs))
    __swig_destroy__ = _core_.delete_SizerItem
    __del__ = lambda self : None;
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

    def SetUserData(*args, **kwargs):
        """
        SetUserData(self, PyObject userData)

        Associate a Python object with this sizer item.
        """
        return _core_.SizerItem_SetUserData(*args, **kwargs)

    Border = property(GetBorder,SetBorder,doc="See `GetBorder` and `SetBorder`") 
    Flag = property(GetFlag,SetFlag,doc="See `GetFlag` and `SetFlag`") 
    MinSize = property(GetMinSize,doc="See `GetMinSize`") 
    MinSizeWithBorder = property(GetMinSizeWithBorder,doc="See `GetMinSizeWithBorder`") 
    Position = property(GetPosition,doc="See `GetPosition`") 
    Proportion = property(GetProportion,SetProportion,doc="See `GetProportion` and `SetProportion`") 
    Ratio = property(GetRatio,SetRatio,doc="See `GetRatio` and `SetRatio`") 
    Rect = property(GetRect,doc="See `GetRect`") 
    Size = property(GetSize,doc="See `GetSize`") 
    Sizer = property(GetSizer,SetSizer,doc="See `GetSizer` and `SetSizer`") 
    Spacer = property(GetSpacer,SetSpacer,doc="See `GetSpacer` and `SetSpacer`") 
    UserData = property(GetUserData,SetUserData,doc="See `GetUserData` and `SetUserData`") 
    Window = property(GetWindow,SetWindow,doc="See `GetWindow` and `SetWindow`") 
_core_.SizerItem_swigregister(SizerItem)

def SizerItemWindow(*args, **kwargs):
    """
    SizerItemWindow(Window window, int proportion, int flag, int border, 
        PyObject userData=None) -> SizerItem

    Constructs a `wx.SizerItem` for tracking a window.
    """
    val = _core_.new_SizerItemWindow(*args, **kwargs)
    return val

def SizerItemSpacer(*args, **kwargs):
    """
    SizerItemSpacer(int width, int height, int proportion, int flag, int border, 
        PyObject userData=None) -> SizerItem

    Constructs a `wx.SizerItem` for tracking a spacer.
    """
    val = _core_.new_SizerItemSpacer(*args, **kwargs)
    return val

def SizerItemSizer(*args, **kwargs):
    """
    SizerItemSizer(Sizer sizer, int proportion, int flag, int border, 
        PyObject userData=None) -> SizerItem

    Constructs a `wx.SizerItem` for tracking a subsizer
    """
    val = _core_.new_SizerItemSizer(*args, **kwargs)
    return val

class Sizer(Object):
    """
    wx.Sizer is the abstract base class used for laying out subwindows in
    a window.  You cannot use wx.Sizer directly; instead, you will have to
    use one of the sizer classes derived from it such as `wx.BoxSizer`,
    `wx.StaticBoxSizer`, `wx.GridSizer`, `wx.FlexGridSizer` and
    `wx.GridBagSizer`.

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _core_.delete_Sizer
    __del__ = lambda self : None;
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
        the item to be found.
        """
        return _core_.Sizer_GetItem(*args, **kwargs)

    def _SetItemMinSize(*args, **kwargs):
        """_SetItemMinSize(self, PyObject item, Size size)"""
        return _core_.Sizer__SetItemMinSize(*args, **kwargs)

    def _ReplaceWin(*args, **kwargs):
        """_ReplaceWin(self, Window oldwin, Window newwin, bool recursive=False) -> bool"""
        return _core_.Sizer__ReplaceWin(*args, **kwargs)

    def _ReplaceSizer(*args, **kwargs):
        """_ReplaceSizer(self, Sizer oldsz, Sizer newsz, bool recursive=False) -> bool"""
        return _core_.Sizer__ReplaceSizer(*args, **kwargs)

    def _ReplaceItem(*args, **kwargs):
        """_ReplaceItem(self, size_t index, SizerItem newitem) -> bool"""
        return _core_.Sizer__ReplaceItem(*args, **kwargs)

    def Replace(self, olditem, item, recursive=False):
        """
        Detaches the given ``olditem`` from the sizer and replaces it with
        ``item`` which can be a window, sizer, or `wx.SizerItem`.  The
        detached child is destroyed only if it is not a window, (because
        windows are owned by their parent, not the sizer.)  The
        ``recursive`` parameter can be used to search for the given
        element recursivly in subsizers.

        This method does not cause any layout or resizing to take place,
        call `Layout` to do so.

        Returns ``True`` if the child item was found and removed.
        """
        if isinstance(olditem, wx.Window):
            return self._ReplaceWin(olditem, item, recursive)
        elif isinstnace(olditem, wx.Sizer):
            return self._ReplaceSizer(olditem, item, recursive)
        elif isinstnace(olditem, int):
            return self._ReplaceItem(olditem, item)
        else:
            raise TypeError("Expected Window, Sizer, or integer for first parameter.")

    def SetContainingWindow(*args, **kwargs):
        """
        SetContainingWindow(self, Window window)

        Set (or unset) the window this sizer is used in.
        """
        return _core_.Sizer_SetContainingWindow(*args, **kwargs)

    def GetContainingWindow(*args, **kwargs):
        """
        GetContainingWindow(self) -> Window

        Get the window this sizer is used in.
        """
        return _core_.Sizer_GetContainingWindow(*args, **kwargs)

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

    def AddSpacer(self, *args, **kw):
        """AddSpacer(int size) --> SizerItem

        Add a spacer that is (size,size) pixels.
        """
        if args and type(args[0]) == int:
            return self.Add( (args[0],args[0] ), 0)
        else: # otherwise stay compatible with old AddSpacer
            return self.Add(*args, **kw)
    def PrependSpacer(self, *args, **kw):
        """PrependSpacer(int size) --> SizerItem

        Prepend a spacer that is (size, size) pixels."""
        if args and type(args[0]) == int:
            return self.Prepend( (args[0],args[0] ), 0)
        else: # otherwise stay compatible with old PrependSpacer
            return self.Prepend(*args, **kw)
    def InsertSpacer(self, index, *args, **kw):
        """InsertSpacer(int index, int size) --> SizerItem

        Insert a spacer at position index that is (size, size) pixels."""
        if args and type(args[0]) == int:
            return self.Insert( index, (args[0],args[0] ), 0)
        else: # otherwise stay compatible with old InsertSpacer
            return self.Insert(index, *args, **kw)

                   
    def AddStretchSpacer(self, prop=1):
        """AddStretchSpacer(int prop=1) --> SizerItem

        Add a stretchable spacer."""
        return self.Add((0,0), prop)
    def PrependStretchSpacer(self, prop=1):
        """PrependStretchSpacer(int prop=1) --> SizerItem

        Prepend a stretchable spacer."""
        return self.Prepend((0,0), prop)
    def InsertStretchSpacer(self, index, prop=1):
        """InsertStretchSpacer(int index, int prop=1) --> SizerItem

        Insert a stretchable spacer."""
        return self.Insert(index, (0,0), prop)

            
    # for backwards compatibility only, please do not use in new code
    def AddWindow(self, *args, **kw):
        """Compatibility alias for `Add`."""
        return self.Add(*args, **kw)
    def AddSizer(self, *args, **kw):
        """Compatibility alias for `Add`."""
        return self.Add(*args, **kw)

    def PrependWindow(self, *args, **kw):
        """Compatibility alias for `Prepend`."""
        return self.Prepend(*args, **kw)
    def PrependSizer(self, *args, **kw):
        """Compatibility alias for `Prepend`."""
        return self.Prepend(*args, **kw)

    def InsertWindow(self, *args, **kw):
        """Compatibility alias for `Insert`."""
        return self.Insert(*args, **kw)
    def InsertSizer(self, *args, **kw):
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
        GetChildren(self) -> list

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

        Determines if the item is currently shown. To make a sizer
        item disappear or reappear, use Show followed by `Layout`.  The *item*
        parameter can be either a window, a sizer, or the zero-based index of
        the item.
        """
        return _core_.Sizer_IsShown(*args, **kwargs)

    def Hide(self, item, recursive=False):
        """
        A convenience method for `Show` (item, False, recursive).
        """
        return self.Show(item, False, recursive)

    def ShowItems(*args, **kwargs):
        """
        ShowItems(self, bool show)

        Recursively call `wx.SizerItem.Show` on all sizer items.
        """
        return _core_.Sizer_ShowItems(*args, **kwargs)

    Children = property(GetChildren,doc="See `GetChildren`") 
    ContainingWindow = property(GetContainingWindow,SetContainingWindow,doc="See `GetContainingWindow` and `SetContainingWindow`") 
    MinSize = property(GetMinSize,SetMinSize,doc="See `GetMinSize` and `SetMinSize`") 
    Position = property(GetPosition,doc="See `GetPosition`") 
    Size = property(GetSize,doc="See `GetSize`") 
_core_.Sizer_swigregister(Sizer)

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
    `CalcMin` and reusing them in `RecalcSizes`.

    :see: `wx.SizerItem`, `wx.Sizer.GetChildren`


    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> PySizer

        Creates a wx.PySizer.  Must be called from the __init__ in the derived
        class.
        """
        _core_.PySizer_swiginit(self,_core_.new_PySizer(*args, **kwargs))
        self._setCallbackInfo(self, PySizer);self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _core_.PySizer__setCallbackInfo(*args, **kwargs)

_core_.PySizer_swigregister(PySizer)

#---------------------------------------------------------------------------

class BoxSizer(Sizer):
    """
    The basic idea behind a box sizer is that windows will most often be
    laid out in rather simple basic geometry, typically in a row or a
    column or nested hierarchies of either.  A wx.BoxSizer will lay out
    its items in a simple row or column, depending on the orientation
    parameter passed to the constructor.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int orient=HORIZONTAL) -> BoxSizer

        Constructor for a wx.BoxSizer. *orient* may be one of ``wx.VERTICAL``
        or ``wx.HORIZONTAL`` for creating either a column sizer or a row
        sizer.
        """
        _core_.BoxSizer_swiginit(self,_core_.new_BoxSizer(*args, **kwargs))
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

    Orientation = property(GetOrientation,SetOrientation,doc="See `GetOrientation` and `SetOrientation`") 
_core_.BoxSizer_swigregister(BoxSizer)

#---------------------------------------------------------------------------

class StaticBoxSizer(BoxSizer):
    """
    wx.StaticBoxSizer derives from and functions identically to the
    `wx.BoxSizer` and adds a `wx.StaticBox` around the items that the sizer
    manages.  Note that this static box must be created separately and
    passed to the sizer constructor.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, StaticBox box, int orient=HORIZONTAL) -> StaticBoxSizer

        Constructor. It takes an associated static box and the orientation
        *orient* as parameters - orient can be either of ``wx.VERTICAL`` or
        ``wx.HORIZONTAL``.
        """
        _core_.StaticBoxSizer_swiginit(self,_core_.new_StaticBoxSizer(*args, **kwargs))
        self._setOORInfo(self)

    def GetStaticBox(*args, **kwargs):
        """
        GetStaticBox(self) -> StaticBox

        Returns the static box associated with this sizer.
        """
        return _core_.StaticBoxSizer_GetStaticBox(*args, **kwargs)

    StaticBox = property(GetStaticBox,doc="See `GetStaticBox`") 
_core_.StaticBoxSizer_swigregister(StaticBoxSizer)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int rows=1, int cols=0, int vgap=0, int hgap=0) -> GridSizer

        Constructor for a wx.GridSizer. *rows* and *cols* determine the number
        of columns and rows in the sizer - if either of the parameters is
        zero, it will be calculated to from the total number of children in
        the sizer, thus making the sizer grow dynamically. *vgap* and *hgap*
        define extra space between all children.
        """
        _core_.GridSizer_swiginit(self,_core_.new_GridSizer(*args, **kwargs))
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

    def CalcRowsCols(self):
        """
        CalcRowsCols() -> (rows, cols)

        Calculates how many rows and columns will be in the sizer based
        on the current number of items and also the rows, cols specified
        in the constructor.
        """
        nitems = len(self.GetChildren())
        rows = self.GetRows()
        cols = self.GetCols()
        assert rows != 0 or cols != 0, "Grid sizer must have either rows or columns fixed"
        if cols != 0:
            rows = (nitems + cols - 1) / cols
        elif rows != 0:
            cols = (nitems + rows - 1) / rows
        return (rows, cols)

    Cols = property(GetCols,SetCols,doc="See `GetCols` and `SetCols`") 
    HGap = property(GetHGap,SetHGap,doc="See `GetHGap` and `SetHGap`") 
    Rows = property(GetRows,SetRows,doc="See `GetRows` and `SetRows`") 
    VGap = property(GetVGap,SetVGap,doc="See `GetVGap` and `SetVGap`") 
_core_.GridSizer_swigregister(GridSizer)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int rows=1, int cols=0, int vgap=0, int hgap=0) -> FlexGridSizer

        Constructor for a wx.FlexGridSizer. *rows* and *cols* determine the
        number of columns and rows in the sizer - if either of the parameters
        is zero, it will be calculated to from the total number of children in
        the sizer, thus making the sizer grow dynamically. *vgap* and *hgap*
        define extra space between all children.
        """
        _core_.FlexGridSizer_swiginit(self,_core_.new_FlexGridSizer(*args, **kwargs))
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

    ColWidths = property(GetColWidths,doc="See `GetColWidths`") 
    FlexibleDirection = property(GetFlexibleDirection,SetFlexibleDirection,doc="See `GetFlexibleDirection` and `SetFlexibleDirection`") 
    NonFlexibleGrowMode = property(GetNonFlexibleGrowMode,SetNonFlexibleGrowMode,doc="See `GetNonFlexibleGrowMode` and `SetNonFlexibleGrowMode`") 
    RowHeights = property(GetRowHeights,doc="See `GetRowHeights`") 
_core_.FlexGridSizer_swigregister(FlexGridSizer)

class StdDialogButtonSizer(BoxSizer):
    """
    A special sizer that knows how to order and position standard buttons
    in order to conform to the current platform's standards.  You simply
    need to add each `wx.Button` to the sizer, and be sure to create the
    buttons using the standard ID's.  Then call `Realize` and the sizer
    will take care of the rest.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> StdDialogButtonSizer"""
        _core_.StdDialogButtonSizer_swiginit(self,_core_.new_StdDialogButtonSizer(*args, **kwargs))
    def AddButton(*args, **kwargs):
        """
        AddButton(self, wxButton button)

        Use this to add the buttons to this sizer.  Do not use the `Add`
        method in the base class.
        """
        return _core_.StdDialogButtonSizer_AddButton(*args, **kwargs)

    def Realize(*args, **kwargs):
        """
        Realize(self)

        This funciton needs to be called after all the buttons have been added
        to the sizer.  It will reorder them and position them in a platform
        specifc manner.
        """
        return _core_.StdDialogButtonSizer_Realize(*args, **kwargs)

    def SetAffirmativeButton(*args, **kwargs):
        """SetAffirmativeButton(self, wxButton button)"""
        return _core_.StdDialogButtonSizer_SetAffirmativeButton(*args, **kwargs)

    def SetNegativeButton(*args, **kwargs):
        """SetNegativeButton(self, wxButton button)"""
        return _core_.StdDialogButtonSizer_SetNegativeButton(*args, **kwargs)

    def SetCancelButton(*args, **kwargs):
        """SetCancelButton(self, wxButton button)"""
        return _core_.StdDialogButtonSizer_SetCancelButton(*args, **kwargs)

    def GetAffirmativeButton(*args, **kwargs):
        """GetAffirmativeButton(self) -> wxButton"""
        return _core_.StdDialogButtonSizer_GetAffirmativeButton(*args, **kwargs)

    def GetApplyButton(*args, **kwargs):
        """GetApplyButton(self) -> wxButton"""
        return _core_.StdDialogButtonSizer_GetApplyButton(*args, **kwargs)

    def GetNegativeButton(*args, **kwargs):
        """GetNegativeButton(self) -> wxButton"""
        return _core_.StdDialogButtonSizer_GetNegativeButton(*args, **kwargs)

    def GetCancelButton(*args, **kwargs):
        """GetCancelButton(self) -> wxButton"""
        return _core_.StdDialogButtonSizer_GetCancelButton(*args, **kwargs)

    def GetHelpButton(*args, **kwargs):
        """GetHelpButton(self) -> wxButton"""
        return _core_.StdDialogButtonSizer_GetHelpButton(*args, **kwargs)

    AffirmativeButton = property(GetAffirmativeButton,SetAffirmativeButton,doc="See `GetAffirmativeButton` and `SetAffirmativeButton`") 
    ApplyButton = property(GetApplyButton,doc="See `GetApplyButton`") 
    CancelButton = property(GetCancelButton,SetCancelButton,doc="See `GetCancelButton` and `SetCancelButton`") 
    HelpButton = property(GetHelpButton,doc="See `GetHelpButton`") 
    NegativeButton = property(GetNegativeButton,SetNegativeButton,doc="See `GetNegativeButton` and `SetNegativeButton`") 
_core_.StdDialogButtonSizer_swigregister(StdDialogButtonSizer)

#---------------------------------------------------------------------------

class GBPosition(object):
    """
    This class represents the position of an item in a virtual grid of
    rows and columns managed by a `wx.GridBagSizer`.  wxPython has
    typemaps that will automatically convert from a 2-element sequence of
    integers to a wx.GBPosition, so you can use the more pythonic
    representation of the position nearly transparently in Python code.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int row=0, int col=0) -> GBPosition

        This class represents the position of an item in a virtual grid of
        rows and columns managed by a `wx.GridBagSizer`.  wxPython has
        typemaps that will automatically convert from a 2-element sequence of
        integers to a wx.GBPosition, so you can use the more pythonic
        representation of the position nearly transparently in Python code.
        """
        _core_.GBPosition_swiginit(self,_core_.new_GBPosition(*args, **kwargs))
    __swig_destroy__ = _core_.delete_GBPosition
    __del__ = lambda self : None;
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
        """
        __eq__(self, PyObject other) -> bool

        Compare GBPosition for equality.
        """
        return _core_.GBPosition___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, PyObject other) -> bool

        Compare GBPosition for inequality.
        """
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

_core_.GBPosition_swigregister(GBPosition)

class GBSpan(object):
    """
    This class is used to hold the row and column spanning attributes of
    items in a `wx.GridBagSizer`.  wxPython has typemaps that will
    automatically convert from a 2-element sequence of integers to a
    wx.GBSpan, so you can use the more pythonic representation of the span
    nearly transparently in Python code.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int rowspan=1, int colspan=1) -> GBSpan

        Construct a new wxGBSpan, optionally setting the rowspan and
        colspan. The default is (1,1). (Meaning that the item occupies one
        cell in each direction.
        """
        _core_.GBSpan_swiginit(self,_core_.new_GBSpan(*args, **kwargs))
    __swig_destroy__ = _core_.delete_GBSpan
    __del__ = lambda self : None;
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
        """
        __eq__(self, PyObject other) -> bool

        Compare wxGBSpan for equality.
        """
        return _core_.GBSpan___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, PyObject other) -> bool

        Compare GBSpan for inequality.
        """
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

_core_.GBSpan_swigregister(GBSpan)

class GBSizerItem(SizerItem):
    """
    The wx.GBSizerItem class is used to track the additional data about
    items in a `wx.GridBagSizer` such as the item's position in the grid
    and how many rows or columns it spans.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> GBSizerItem

        Constructs an empty wx.GBSizerItem.  Either a window, sizer or spacer
        size will need to be set, as well as a position and span before this
        item can be used in a Sizer.

        You will probably never need to create a wx.GBSizerItem directly as they
        are created automatically when the sizer's Add method is called.
        """
        _core_.GBSizerItem_swiginit(self,_core_.new_GBSizerItem(*args, **kwargs))
    __swig_destroy__ = _core_.delete_GBSizerItem
    __del__ = lambda self : None;
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

    EndPos = property(GetEndPos,doc="See `GetEndPos`") 
    GBSizer = property(GetGBSizer,SetGBSizer,doc="See `GetGBSizer` and `SetGBSizer`") 
    Pos = property(GetPos,SetPos,doc="See `GetPos` and `SetPos`") 
    Span = property(GetSpan,SetSpan,doc="See `GetSpan` and `SetSpan`") 
_core_.GBSizerItem_swigregister(GBSizerItem)
DefaultSpan = cvar.DefaultSpan

def GBSizerItemWindow(*args, **kwargs):
    """
    GBSizerItemWindow(Window window, GBPosition pos, GBSpan span, int flag, 
        int border, PyObject userData=None) -> GBSizerItem

    Construct a `wx.GBSizerItem` for a window.
    """
    val = _core_.new_GBSizerItemWindow(*args, **kwargs)
    return val

def GBSizerItemSizer(*args, **kwargs):
    """
    GBSizerItemSizer(Sizer sizer, GBPosition pos, GBSpan span, int flag, 
        int border, PyObject userData=None) -> GBSizerItem

    Construct a `wx.GBSizerItem` for a sizer
    """
    val = _core_.new_GBSizerItemSizer(*args, **kwargs)
    return val

def GBSizerItemSpacer(*args, **kwargs):
    """
    GBSizerItemSpacer(int width, int height, GBPosition pos, GBSpan span, 
        int flag, int border, PyObject userData=None) -> GBSizerItem

    Construct a `wx.GBSizerItem` for a spacer.
    """
    val = _core_.new_GBSizerItemSpacer(*args, **kwargs)
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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int vgap=0, int hgap=0) -> GridBagSizer

        Constructor, with optional parameters to specify the gap between the
        rows and columns.
        """
        _core_.GridBagSizer_swiginit(self,_core_.new_GridBagSizer(*args, **kwargs))
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

_core_.GridBagSizer_swigregister(GridBagSizer)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
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

    Done = property(GetDone,SetDone,doc="See `GetDone` and `SetDone`") 
    Margin = property(GetMargin,SetMargin,doc="See `GetMargin` and `SetMargin`") 
    MyEdge = property(GetMyEdge,doc="See `GetMyEdge`") 
    OtherEdge = property(GetOtherEdge,doc="See `GetOtherEdge`") 
    OtherWindow = property(GetOtherWindow,doc="See `GetOtherWindow`") 
    Percent = property(GetPercent,doc="See `GetPercent`") 
    Relationship = property(GetRelationship,SetRelationship,doc="See `GetRelationship` and `SetRelationship`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_core_.IndividualLayoutConstraint_swigregister(IndividualLayoutConstraint)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
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
        _core_.LayoutConstraints_swiginit(self,_core_.new_LayoutConstraints(*args, **kwargs))
    __swig_destroy__ = _core_.delete_LayoutConstraints
    __del__ = lambda self : None;
    def SatisfyConstraints(*args, **kwargs):
        """SatisfyConstraints(Window win) -> (areSatisfied, noChanges)"""
        return _core_.LayoutConstraints_SatisfyConstraints(*args, **kwargs)

    def AreSatisfied(*args, **kwargs):
        """AreSatisfied(self) -> bool"""
        return _core_.LayoutConstraints_AreSatisfied(*args, **kwargs)

_core_.LayoutConstraints_swigregister(LayoutConstraints)

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

# Set wxPython's default string<-->unicode conversion encoding from
# the locale, but only if Python's default hasn't been changed.  (We
# assume that if the user has customized it already then that is the
# encoding we need to use as well.)
#
# The encoding selected here is used when string or unicode objects
# need to be converted in order to pass them to wxWidgets.  Please be
# aware that the default encoding within the same locale may be
# slightly different on different platforms.  For example, please see
# http://www.alanwood.net/demos/charsetdiffs.html for differences
# between the common latin/roman encodings.

default = _sys.getdefaultencoding()
if default == 'ascii':
    import locale
    import codecs
    try:
        default = locale.getdefaultlocale()[1]
        codecs.lookup(default)
    except (ValueError, LookupError, TypeError):
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

    Interval = property(GetInterval)
    Result = property(GetResult)


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
    functions, in order to reduce the clutter in the API docs.
    """
    def __init__(self, globals):
        self._globals = globals
        
    def __call__(self, name):
        import types
        obj = self._globals.get(name, None)

        # only document classes and function
        if type(obj) not in [type, types.ClassType, types.FunctionType, types.BuiltinFunctionType]:
            return False

        # skip other things that are private or will be documented as part of somethign else
        if name.startswith('_') or name.startswith('EVT') or name.endswith('_swigregister')  or name.endswith('Ptr') :
            return False

        # skip functions that are duplicates of static functions in a class
        if name.find('_') != -1:
            cls = self._globals.get(name.split('_')[0], None)
            methname = name.split('_')[1]
            if hasattr(cls, methname) and type(getattr(cls, methname)) is types.FunctionType:
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

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------



