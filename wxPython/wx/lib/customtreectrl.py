# --------------------------------------------------------------------------------- #
# CUSTOMTREECTRL wxPython IMPLEMENTATION
# Inspired By And Heavily Based On wxGenericTreeCtrl.
#
# Andrea Gavana, @ 17 May 2006
# Latest Revision: 26 May 2006, 22.30 CET
#
#
# TODO List
#
# Almost All The Features Of wx.TreeCtrl Are Available, And There Is Practically
# No Limit In What Could Be Added To This Class. The First Things That Comes
# To My Mind Are:
#
# 1. Implement The Style TR_EXTENDED (I Have Never Used It, But It May Be Useful).
#
# 2. Add Support For 3-State CheckBoxes (Is That Really Useful?).
#
# 3. Try To Implement A More Flicker-Free Background Image In Cases Like
#    Centered Or Stretched Image (Now CustomTreeCtrl Supports Only Tiled
#    Background Images).
#
# 4. Try To Mimic Windows wx.TreeCtrl Expanding/Collapsing behaviour: CustomTreeCtrl
#    Suddenly Expands/Collapses The Nodes On Mouse Click While The Native Control
#    Has Some Kind Of "Smooth" Expanding/Collapsing, Like A Wave. I Don't Even
#    Know Where To Start To Do That.
#
# 5. Speed Up General OnPaint Things? I Have No Idea, Here CustomTreeCtrl Is Quite
#    Fast, But We Should See On Slower Machines.
#
#
# For All Kind Of Problems, Requests Of Enhancements And Bug Reports, Please
# Write To Me At:
#
# gavana@kpo.kz
# andrea.gavana@gmail.com
#
# Or, Obviously, To The wxPython Mailing List!!!
#
#
# End Of Comments
# --------------------------------------------------------------------------------- #


"""
Description
===========

CustomTreeCtrl is a class that mimics the behaviour of wx.TreeCtrl, with almost the
same base functionalities plus some more enhancements. This class does not rely on
the native control, as it is a full owner-drawn tree control.
Apart of the base functionalities of CustomTreeCtrl (described below), in addition
to the standard wx.TreeCtrl behaviour this class supports:

* CheckBox-type items: checkboxes are easy to handle, just selected or unselected
  state with no particular issues in handling the item's children;

* RadioButton-type items: since I elected to put radiobuttons in CustomTreeCtrl, I
  needed some way to handle them, that made sense. So, I used the following approach:
     - All peer-nodes that are radiobuttons will be mutually exclusive. In other words,
       only one of a set of radiobuttons that share a common parent can be checked at
       once. If a radiobutton node becomes checked, then all of its peer radiobuttons
       must be unchecked.
     - If a radiobutton node becomes unchecked, then all of its child nodes will become
       inactive.

* Hyperlink-type items: they look like an hyperlink, with the proper mouse cursor on
  hovering.

* Multiline text items.

* Enabling/disabling items (together with their plain or grayed out icons).

* Whatever non-toplevel widget can be attached next to an item.

* Default selection style, gradient (horizontal/vertical) selection style and Windows
  Vista selection style.

* Customized drag and drop images built on the fly.

* Setting the CustomTreeCtrl item buttons to a personalized imagelist.

* Setting the CustomTreeCtrl check/radio item icons to a personalized imagelist.

* Changing the style of the lines that connect the items (in terms of wx.Pen styles).

* Using an image as a CustomTreeCtrl background (currently only in "tile" mode).

And a lot more. Check the demo for an almost complete review of the functionalities.


Base Functionalities
====================

CustomTreeCtrl supports all the wx.TreeCtrl styles, except:
  - TR_EXTENDED: supports for this style is on the todo list (Am I sure of this?).

Plus it has 2 more styles to handle checkbox-type items:
  - TR_AUTO_CHECK_CHILD : automatically checks/unchecks the item children;
  - TR_AUTO_TOGGLE_CHILD: automatically toggles the item children.

All the methods available in wx.TreeCtrl are also available in CustomTreeCtrl.


Events
======

All the events supported by wx.TreeCtrl are also available in CustomTreeCtrl, with
a few exceptions:

  - EVT_TREE_GET_INFO (don't know what this means);
  - EVT_TREE_SET_INFO (don't know what this means);
  - EVT_TREE_ITEM_MIDDLE_CLICK (not implemented, but easy to add);
  - EVT_TREE_STATE_IMAGE_CLICK: no need for that, look at the checking events below.

Plus, CustomTreeCtrl supports the events related to the checkbutton-type items:

  - EVT_TREE_ITEM_CHECKING: an item is being checked;
  - EVT_TREE_ITEM_CHECKED: an item has been checked.

And to hyperlink-type items:

  - EVT_TREE_ITEM_HYPERLINK: an hyperlink item has been clicked (this event is sent
    after the EVT_TREE_SEL_CHANGED event).


Supported Platforms
===================

CustomTreeCtrl has been tested on the following platforms:
  * Windows (Windows XP);
  * GTK (Thanks to Michele Petrazzo);
  * Mac OS (Thanks to John Jackson).


Latest Revision: Andrea Gavana @ 26 May 2006, 22.30 CET
Version 0.8

"""


import wx
import zlib
import cStringIO

# ----------------------------------------------------------------------------
# Constants
# ----------------------------------------------------------------------------

_NO_IMAGE = -1
_PIXELS_PER_UNIT = 10

# Start editing the current item after half a second (if the mouse hasn't
# been clicked/moved)
_DELAY = 500

# ----------------------------------------------------------------------------
# Constants
# ----------------------------------------------------------------------------

# Enum for different images associated with a treectrl item
TreeItemIcon_Normal = 0              # not selected, not expanded
TreeItemIcon_Selected = 1            #     selected, not expanded
TreeItemIcon_Expanded = 2            # not selected,     expanded
TreeItemIcon_SelectedExpanded = 3    #     selected,     expanded

TreeItemIcon_Checked = 0             # check button,     checked
TreeItemIcon_NotChecked = 1          # check button, not checked
TreeItemIcon_Flagged = 2             # radio button,     selected
TreeItemIcon_NotFlagged = 3          # radio button, not selected

# ----------------------------------------------------------------------------
# CustomTreeCtrl flags
# ----------------------------------------------------------------------------

TR_NO_BUTTONS = wx.TR_NO_BUTTONS                               # for convenience
TR_HAS_BUTTONS = wx.TR_HAS_BUTTONS                             # draw collapsed/expanded btns
TR_NO_LINES = wx.TR_NO_LINES                                   # don't draw lines at all
TR_LINES_AT_ROOT = wx.TR_LINES_AT_ROOT                         # connect top-level nodes
TR_TWIST_BUTTONS = wx.TR_TWIST_BUTTONS                         # still used by wxTreeListCtrl

TR_SINGLE = wx.TR_SINGLE                                       # for convenience
TR_MULTIPLE = wx.TR_MULTIPLE                                   # can select multiple items
TR_EXTENDED = wx.TR_EXTENDED                                   # TODO: allow extended selection
TR_HAS_VARIABLE_ROW_HEIGHT = wx.TR_HAS_VARIABLE_ROW_HEIGHT     # what it says

TR_EDIT_LABELS = wx.TR_EDIT_LABELS                             # can edit item labels
TR_ROW_LINES = wx.TR_ROW_LINES                                 # put border around items
TR_HIDE_ROOT = wx.TR_HIDE_ROOT                                 # don't display root node

TR_FULL_ROW_HIGHLIGHT = wx.TR_FULL_ROW_HIGHLIGHT               # highlight full horz space

TR_AUTO_CHECK_CHILD = 0x4000                                   # only meaningful for checkboxes
TR_AUTO_TOGGLE_CHILD = 0x8000                                  # only meaningful for checkboxes

TR_DEFAULT_STYLE = wx.TR_DEFAULT_STYLE                         # default style for the tree control

# Values for the `flags' parameter of CustomTreeCtrl.HitTest() which determine
# where exactly the specified point is situated:

TREE_HITTEST_ABOVE            = wx.TREE_HITTEST_ABOVE
TREE_HITTEST_BELOW            = wx.TREE_HITTEST_BELOW
TREE_HITTEST_NOWHERE          = wx.TREE_HITTEST_NOWHERE
# on the button associated with an item.
TREE_HITTEST_ONITEMBUTTON     = wx.TREE_HITTEST_ONITEMBUTTON
# on the bitmap associated with an item.
TREE_HITTEST_ONITEMICON       = wx.TREE_HITTEST_ONITEMICON
# on the indent associated with an item.
TREE_HITTEST_ONITEMINDENT     = wx.TREE_HITTEST_ONITEMINDENT
# on the label (string) associated with an item.
TREE_HITTEST_ONITEMLABEL      = wx.TREE_HITTEST_ONITEMLABEL
# on the right of the label associated with an item.
TREE_HITTEST_ONITEMRIGHT      = wx.TREE_HITTEST_ONITEMRIGHT
# on the label (string) associated with an item.
TREE_HITTEST_ONITEMSTATEICON  = wx.TREE_HITTEST_ONITEMSTATEICON
# on the left of the CustomTreeCtrl.
TREE_HITTEST_TOLEFT           = wx.TREE_HITTEST_TOLEFT
# on the right of the CustomTreeCtrl.
TREE_HITTEST_TORIGHT          = wx.TREE_HITTEST_TORIGHT
# on the upper part (first half) of the item.
TREE_HITTEST_ONITEMUPPERPART  = wx.TREE_HITTEST_ONITEMUPPERPART
# on the lower part (second half) of the item.
TREE_HITTEST_ONITEMLOWERPART  = wx.TREE_HITTEST_ONITEMLOWERPART
# on the check icon, if present
TREE_HITTEST_ONITEMCHECKICON  = 0x4000
# anywhere on the item
TREE_HITTEST_ONITEM  = TREE_HITTEST_ONITEMICON | TREE_HITTEST_ONITEMLABEL | TREE_HITTEST_ONITEMCHECKICON


# Background Image Style
_StyleTile = 0
_StyleStretch = 1

# Windows Vista Colours
_rgbSelectOuter = wx.Colour(170, 200, 245)
_rgbSelectInner = wx.Colour(230, 250, 250)
_rgbSelectTop = wx.Colour(210, 240, 250)
_rgbSelectBottom = wx.Colour(185, 215, 250)
_rgbNoFocusTop = wx.Colour(250, 250, 250)
_rgbNoFocusBottom = wx.Colour(235, 235, 235)
_rgbNoFocusOuter = wx.Colour(220, 220, 220)
_rgbNoFocusInner = wx.Colour(245, 245, 245)

# Flags for wx.RendererNative
_CONTROL_EXPANDED = 8
_CONTROL_CURRENT = 16
            
# Version Info
__version__ = "0.8"


# ----------------------------------------------------------------------------
# CustomTreeCtrl events and binding for handling them
# ----------------------------------------------------------------------------

wxEVT_TREE_BEGIN_DRAG = wx.wxEVT_COMMAND_TREE_BEGIN_DRAG
wxEVT_TREE_BEGIN_RDRAG = wx.wxEVT_COMMAND_TREE_BEGIN_RDRAG
wxEVT_TREE_BEGIN_LABEL_EDIT = wx.wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT
wxEVT_TREE_END_LABEL_EDIT = wx.wxEVT_COMMAND_TREE_END_LABEL_EDIT
wxEVT_TREE_DELETE_ITEM = wx.wxEVT_COMMAND_TREE_DELETE_ITEM
wxEVT_TREE_GET_INFO = wx.wxEVT_COMMAND_TREE_GET_INFO
wxEVT_TREE_SET_INFO = wx.wxEVT_COMMAND_TREE_SET_INFO
wxEVT_TREE_ITEM_EXPANDED = wx.wxEVT_COMMAND_TREE_ITEM_EXPANDED
wxEVT_TREE_ITEM_EXPANDING = wx.wxEVT_COMMAND_TREE_ITEM_EXPANDING
wxEVT_TREE_ITEM_COLLAPSED = wx.wxEVT_COMMAND_TREE_ITEM_COLLAPSED
wxEVT_TREE_ITEM_COLLAPSING = wx.wxEVT_COMMAND_TREE_ITEM_COLLAPSING
wxEVT_TREE_SEL_CHANGED = wx.wxEVT_COMMAND_TREE_SEL_CHANGED
wxEVT_TREE_SEL_CHANGING = wx.wxEVT_COMMAND_TREE_SEL_CHANGING
wxEVT_TREE_KEY_DOWN = wx.wxEVT_COMMAND_TREE_KEY_DOWN
wxEVT_TREE_ITEM_ACTIVATED = wx.wxEVT_COMMAND_TREE_ITEM_ACTIVATED
wxEVT_TREE_ITEM_RIGHT_CLICK = wx.wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK
wxEVT_TREE_ITEM_MIDDLE_CLICK = wx.wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK
wxEVT_TREE_END_DRAG = wx.wxEVT_COMMAND_TREE_END_DRAG
wxEVT_TREE_STATE_IMAGE_CLICK = wx.wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK
wxEVT_TREE_ITEM_GETTOOLTIP = wx.wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP
wxEVT_TREE_ITEM_MENU = wx.wxEVT_COMMAND_TREE_ITEM_MENU
wxEVT_TREE_ITEM_CHECKING = wx.NewEventType()
wxEVT_TREE_ITEM_CHECKED = wx.NewEventType()
wxEVT_TREE_ITEM_HYPERLINK = wx.NewEventType()

EVT_TREE_BEGIN_DRAG = wx.EVT_TREE_BEGIN_DRAG
EVT_TREE_BEGIN_RDRAG = wx.EVT_TREE_BEGIN_RDRAG
EVT_TREE_BEGIN_LABEL_EDIT = wx.EVT_TREE_BEGIN_LABEL_EDIT
EVT_TREE_END_LABEL_EDIT = wx.EVT_TREE_END_LABEL_EDIT
EVT_TREE_DELETE_ITEM = wx.EVT_TREE_DELETE_ITEM
EVT_TREE_GET_INFO = wx.EVT_TREE_GET_INFO
EVT_TREE_SET_INFO = wx.EVT_TREE_SET_INFO
EVT_TREE_ITEM_EXPANDED = wx.EVT_TREE_ITEM_EXPANDED
EVT_TREE_ITEM_EXPANDING = wx.EVT_TREE_ITEM_EXPANDING
EVT_TREE_ITEM_COLLAPSED = wx.EVT_TREE_ITEM_COLLAPSED
EVT_TREE_ITEM_COLLAPSING = wx.EVT_TREE_ITEM_COLLAPSING
EVT_TREE_SEL_CHANGED = wx.EVT_TREE_SEL_CHANGED
EVT_TREE_SEL_CHANGING = wx.EVT_TREE_SEL_CHANGING
EVT_TREE_KEY_DOWN = wx.EVT_TREE_KEY_DOWN
EVT_TREE_ITEM_ACTIVATED = wx.EVT_TREE_ITEM_ACTIVATED
EVT_TREE_ITEM_RIGHT_CLICK = wx.EVT_TREE_ITEM_RIGHT_CLICK
EVT_TREE_ITEM_MIDDLE_CLICK = wx.EVT_TREE_ITEM_MIDDLE_CLICK
EVT_TREE_END_DRAG = wx.EVT_TREE_END_DRAG
EVT_TREE_STATE_IMAGE_CLICK = wx.EVT_TREE_STATE_IMAGE_CLICK
EVT_TREE_ITEM_GETTOOLTIP = wx.EVT_TREE_ITEM_GETTOOLTIP
EVT_TREE_ITEM_MENU = wx.EVT_TREE_ITEM_MENU
EVT_TREE_ITEM_CHECKING = wx.PyEventBinder(wxEVT_TREE_ITEM_CHECKING, 1)
EVT_TREE_ITEM_CHECKED = wx.PyEventBinder(wxEVT_TREE_ITEM_CHECKED, 1)
EVT_TREE_ITEM_HYPERLINK = wx.PyEventBinder(wxEVT_TREE_ITEM_HYPERLINK, 1)


def GetFlaggedData():
    return zlib.decompress(
'x\xda\x012\x02\xcd\xfd\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\r\x00\
\x00\x00\r\x08\x06\x00\x00\x00r\xeb\xe4|\x00\x00\x00\x04sBIT\x08\x08\x08\x08\
|\x08d\x88\x00\x00\x01\xe9IDAT(\x91u\x92\xd1K\xd3a\x14\x86\x9f\xef|J2J\xc3%\
\x85\x8e\x1cb\x93Hl\xd9,\x06F]4\x10\tD3\x83\x88\xc8\xbf\xc0\xb4\xaeBP1\xe9\
\xa2(\xec\xaan\xc3\x82pD\xa1\x84\xb0\x88@3\x8c\xc9\xa2bT\xa2^\x8c\x81V3\xb6\
\xb5\x9f\xce9\xbe.j\xb20\xdf\xeb\xf7\xe19\x07^\xa5D\x93\x9f\x9ea\xbf\t\x04\
\xbf\x12\x8b[\xd8Kl\xf8<.\xeet\xb5\xab\xfc\x8e\xca\x87*ZzM\xf3\xb1j|G\xab\
\xf0\xd4\x94\x13\x9a_&0\xbb\xc8\xd8\xf4g\xa2\xcfo\xa8-P\xc7\xf5\x07\xa6\xedD\
\r\x8d\xb5\xfb\x11\x11\xb4\xd6\x88h\xb4\xd6L}\x8a\xf0\xe4\xd5G\x1e\rt*\x00\
\xc9\x19\xb6\x03D4\xa7\xdcU\\8\xed\xa6\xa2\xa5\xd7\x00\xe8\xab\xf7\x9e\x9a\
\xca\xb2\x9d\\\xf2\xd5!"dT\x86\xc9\xe4\x14\x83s\x83HF\xe3\xdc\xe5\xa4\xa8\
\xb0\x88\xaa\xf2=D\x7f$il>\xdf\xafSe\xf5\xfd\x9dM\x87\xa9\xdc\xb7\x1b\xad5\
\x93\xc9)\xfc\xe9Q\x12\xe9\x04\x13\x0b\x13\x94\xaaR\xdc{\x8f "\xec(,\xe0\xfe\
\xb3\xb7H,a\xe1\xa9)\xdf<e$2Ble\x85\x94e\xb1\x96\xcep\xfb\xdd-D\x04\xa5\x14\
\xdeZ\'\xb1\x84\x85\xd8\x8bm\x84\xe6\x977\x7f8kog)\xba\xc4\xb7\xe5\xef$\xe2?\
\xe9\xa9\xbf\x86R\n\x11a&\x1c\xc1^lC|\r.\x02\xb3\x8b\x9b\xa6&G\x13W\xaa\xbb\
\x91_\x05\x0c\x1d\xbfI\xc7\xa1\x8e\xbf&a|:\x8c\xaf\xc1\x05J4\x8e\xd6>36\x192\
\xc9d\xdc\xa4RI\xb3\xbaj\x99tz\xcd\xac\xaf\xa7\xcd\xc6F\xc6d\xb3Y\xf32\xf8\
\xc58Z\xfb\x8c\x12\xfd\x07R\xa2\xb98\xf0\xd0\xbcx\xf3a[\xe0\xf2\xd0c\x93\xeb\
nYD\xdb\xc9:\xcex\x0f\xe2\xadu2\x13\x8e0>\x1d\xc6\xff\xfa\xfd\xff\x17\x91K\
\xf7\xf0\xa8\t\x04\xe7X\x89[\x94\x96\xd8\xf0y\x0ep\xb7\xeb\xdc?\xdb\xfb\r|\
\xd0\xd1]\x98\xbdm\xdc\x00\x00\x00\x00IEND\xaeB`\x82\x91\xe2\x08\x8f' )

def GetFlaggedBitmap():
    return wx.BitmapFromImage(GetFlaggedImage())

def GetFlaggedImage():
    stream = cStringIO.StringIO(GetFlaggedData())
    return wx.ImageFromStream(stream)

#----------------------------------------------------------------------
def GetNotFlaggedData():
    return zlib.decompress(
'x\xda\x01\xad\x01R\xfe\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\r\x00\
\x00\x00\r\x08\x06\x00\x00\x00r\xeb\xe4|\x00\x00\x00\x04sBIT\x08\x08\x08\x08\
|\x08d\x88\x00\x00\x01dIDAT(\x91\x95\xd21K\x82a\x14\x86\xe1\xe7=\xef798\xb8\
\x89\x0e"|Cd\x94\x88\x83\x065\x88\x108\x88Q\x8b-\xd1\x1f\x88\x9a\n\x04\x11j\
\x8eh\x08\xdaZ\x84(\x82\xc2 0\xc1 $\xb4P\xa1\x10\x11D\xb061\xd4\xd4\xcc\xe44\
\x84 \xa8Hg~.\xcer\x0bA\x12\x83\xb7ux\xce\xd1T\x01\xd5z\x0b:\xad\x06n\xbb\
\x8a\x83\xcdU1\xb8\x11\x83\xc8\xe0\r\xf0\x92\xdd\x0c\x97\xd5\x04\x9b\xaaG\
\xb6XA,]B\xe41\x8f\xf7\xab=1\x84Vv\x8e\xd97\xaf\xc29m\x04\x91\x84\x94\n\xa4\
\x94P\x14\x05\x89\xd77\x9c\xc5_\x10\x0em\x08\x00\xa0\xfe\x87q@J\x89\xc593\
\xfc\xaeY\x18\xbc\x01\x06\x00\xb1}t\xc9\xf5F\x03\x01\xbfs$ \x92 "\x10I\xec\
\x9e\xdcBQ\x08\x14M\x15\xe0\xb2\x9a&\x02"\x82\xc71\x85h\xaa\x00\xaa\xd6[\xb0\
\xa9\xfa\x89\x80\x88\xe0\xb0\x98P\xad\xb7@:\xad\x06\xd9be" "$se\xe8\xb4\x1a\
\x90\xdb\xae"\x96.M\x04D\x84H"\x07\xb7]\x05\x04I\x18}A\xbe\xbe\x7f\xe6Z\xed\
\x83\x1b\x8d\x1a7\x9b\x9f\xdcn\xb7\xb8\xd3\xf9\xe2n\xf7\x9b{\xbd\x1f\xbe{\
\xca\xb3\xd1\x17dA\xf2\x0f\t\x92X\x0b\x9d\xf2\xcdCf,X\xdf\x0fs\x7f;T\xc4\xf2\
\xc2\x0c<\x8e)8,&$seD\x129\\\xc43\xa3\x8b\xf8O{\xbf\xf1\xb5\xa5\x990\x0co\
\xd6\x00\x00\x00\x00IEND\xaeB`\x82&\x11\xab!' )

def GetNotFlaggedBitmap():
    return wx.BitmapFromImage(GetNotFlaggedImage())

def GetNotFlaggedImage():
    stream = cStringIO.StringIO(GetNotFlaggedData())
    return wx.ImageFromStream(stream)

#----------------------------------------------------------------------
def GetCheckedData():
    return zlib.decompress(
"x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd1 \xcc\xc1\x06$\
\x8b^?\xa9\x01R,\xc5N\x9e!\x1c@P\xc3\x91\xd2\x01\xe4\xaf\xf4tq\x0c\xd1\x98\
\x98<\x853\xe7\xc7y\x07\xa5\x84\xc4\x84\x84\x04\x0b3C1\xbd\x03'N\x1c9p\x84\
\xe5\xe0\x993gx||\xce\x14\xcc\xea\xec\xect4^7\xbf\x91\xf3&\x8b\x93\xd4\x8c\
\x19\n\xa7fv\\L\xd8p\x90C\xebx\xcf\x05\x17\x0ff \xb8c\xb6Cm\x06\xdb\xea\xd8\
\xb2\x08\xd3\x03W\x0c\x8c\x8c\x16e%\xa5\xb5E\xe4\xee\xba\xca\xe4|\xb8\xb7\
\xe35OOO\xcf\n\xb3\x83>m\x8c1R\x12\x92\x81s\xd8\x0b/\xb56\x14k|l\\\xc7x\xb4\
\xf2\xc4\xc1*\xd5'B~\xbc\x19uNG\x98\x85\x85\x8d\xe3x%\x16\xb2_\xee\xf1\x07\
\x99\xcb\xacl\x99\xc9\xcf\xb0\xc0_.\x87+\xff\x99\x05\xd0\xd1\x0c\x9e\xae~.\
\xeb\x9c\x12\x9a\x00\x92\xccS\x9f" )

def GetCheckedBitmap():
    return wx.BitmapFromImage(GetCheckedImage())

def GetCheckedImage():
    stream = cStringIO.StringIO(GetCheckedData())
    return wx.ImageFromStream(stream)

#----------------------------------------------------------------------
def GetNotCheckedData():
    return zlib.decompress(
"x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd1 \xcc\xc1\x06$\
\x8b^?\xa9\x01R,\xc5N\x9e!\x1c@P\xc3\x91\xd2\x01\xe4\xe7z\xba8\x86hL\x9c{\
\xe9 o\x83\x01\x07\xeb\x85\xf3\xed\x86w\x0ed\xdaT\x96\x8a\xbc\x9fw\xe7\xc4\
\xd9/\x01\x8b\x97\x8a\xd7\xab*\xfar\xf0Ob\x93^\xf6\xd5%\x9d\x85A\xe6\xf6\x1f\
\x11\x8f{/\x0b\xf8wX+\x9d\xf2\xb6:\x96\xca\xfe\x9a3\xbeA\xe7\xed\x1b\xc6%\
\xfb=X3'sI-il\t\xb9\xa0\xc0;#\xd4\x835m\x9a\xf9J\x85\xda\x16.\x86\x03\xff\
\xee\xdcc\xdd\xc0\xce\xf9\xc8\xcc(\xbe\x1bh1\x83\xa7\xab\x9f\xcb:\xa7\x84&\
\x00\x87S=\xbe" )

def GetNotCheckedBitmap():
    return wx.BitmapFromImage(GetNotCheckedImage())

def GetNotCheckedImage():
    stream = cStringIO.StringIO(GetNotCheckedData())
    return wx.ImageFromStream(stream)


def GrayOut(anImage):
    """
    Convert the given image (in place) to a grayed-out version,
    appropriate for a 'disabled' appearance.
    """
    
    factor = 0.7        # 0 < f < 1.  Higher Is Grayer
    
    if anImage.HasMask():
        maskColor = (anImage.GetMaskRed(), anImage.GetMaskGreen(), anImage.GetMaskBlue())
    else:
        maskColor = None
        
    data = map(ord, list(anImage.GetData()))

    for i in range(0, len(data), 3):
        
        pixel = (data[i], data[i+1], data[i+2])
        pixel = MakeGray(pixel, factor, maskColor)

        for x in range(3):
            data[i+x] = pixel[x]

    anImage.SetData(''.join(map(chr, data)))
    
    return anImage


def MakeGray((r,g,b), factor, maskColor):
    """
    Make a pixel grayed-out. If the pixel matches the maskcolor, it won't be
    changed.
    """
    
    if (r,g,b) != maskColor:
        return map(lambda x: int((230 - x) * factor) + x, (r,g,b))
    else:
        return (r,g,b)


def DrawTreeItemButton(win, dc, rect, flags):
    """ A simple replacement of wx.RendererNative.DrawTreeItemButton. """

    # white background
    dc.SetPen(wx.GREY_PEN)
    dc.SetBrush(wx.WHITE_BRUSH)
    dc.DrawRectangleRect(rect)

    # black lines
    xMiddle = rect.x + rect.width/2
    yMiddle = rect.y + rect.height/2

    # half of the length of the horz lines in "-" and "+"
    halfWidth = rect.width/2 - 2
    dc.SetPen(wx.BLACK_PEN)
    dc.DrawLine(xMiddle - halfWidth, yMiddle,
                xMiddle + halfWidth + 1, yMiddle)

    if not flags & _CONTROL_EXPANDED:
    
        # turn "-" into "+"
        halfHeight = rect.height/2 - 2
        dc.DrawLine(xMiddle, yMiddle - halfHeight,
                    xMiddle, yMiddle + halfHeight + 1)

    
#---------------------------------------------------------------------------
# DragImage Implementation
# This Class Handles The Creation Of A Custom Image In Case Of Item Drag
# And Drop.
#---------------------------------------------------------------------------

class DragImage(wx.DragImage):
    """
    This class handles the creation of a custom image in case of item drag
    and drop.
    """

    def __init__(self, treeCtrl, item):
        """
        Default class constructor.
        For internal use: do not call it in your code!
        """
        
        text = item.GetText()
        font = item.Attr().GetFont()
        colour = item.Attr().GetTextColour()
        if colour is None:
            colour = wx.BLACK
        if font is None:
            font = treeCtrl._normalFont
    
        backcolour = treeCtrl.GetBackgroundColour()
        r, g, b = int(backcolour.Red()), int(backcolour.Green()), int(backcolour.Blue())
        backcolour = ((r >> 1) + 20, (g >> 1) + 20, (b >> 1) + 20)
        backcolour = wx.Colour(backcolour[0], backcolour[1], backcolour[2])
        self._backgroundColour = backcolour

        tempdc = wx.ClientDC(treeCtrl)
        tempdc.SetFont(font)
        width, height, dummy = tempdc.GetMultiLineTextExtent(text + "M")
        
        image = item.GetCurrentImage()

        image_w, image_h = 0, 0
        wcheck, hcheck = 0, 0
        itemcheck = None
        itemimage = None
        ximagepos = 0
        yimagepos = 0
        xcheckpos = 0
        ycheckpos = 0
        
        if image != _NO_IMAGE:    
            if treeCtrl._imageListNormal:
                image_w, image_h = treeCtrl._imageListNormal.GetSize(image)
                image_w += 4
                itemimage = treeCtrl._imageListNormal.GetBitmap(image)
            
        checkimage = item.GetCurrentCheckedImage()

        if checkimage is not None:
            if treeCtrl._imageListCheck:
                wcheck, hcheck = treeCtrl._imageListCheck.GetSize(checkimage)
                wcheck += 4
                itemcheck = treeCtrl._imageListCheck.GetBitmap(checkimage)

        total_h = max(hcheck, height)
        total_h = max(image_h, total_h)
                
        if image_w:
            ximagepos = wcheck
            yimagepos = ((total_h > image_h) and [(total_h-image_h)/2] or [0])[0]

        if checkimage is not None:
            xcheckpos = 2
            ycheckpos = ((total_h > image_h) and [(total_h-image_h)/2] or [0])[0] + 2

        extraH = ((total_h > height) and [(total_h - height)/2] or [0])[0]
        
        xtextpos = wcheck + image_w
        ytextpos = extraH

        total_h = max(image_h, hcheck)
        total_h = max(total_h, height)
        
        if total_h < 30:
            total_h += 2            # at least 2 pixels
        else:
            total_h += total_h/10   # otherwise 10% extra spacing

        total_w = image_w + wcheck + width

        self._total_w = total_w
        self._total_h = total_h
        self._itemimage = itemimage
        self._itemcheck = itemcheck
        self._text = text
        self._colour = colour
        self._font = font
        self._xtextpos = xtextpos
        self._ytextpos = ytextpos
        self._ximagepos = ximagepos
        self._yimagepos = yimagepos
        self._xcheckpos = xcheckpos
        self._ycheckpos = ycheckpos
        self._textwidth = width
        self._textheight = height
        self._extraH = extraH
        
        self._bitmap = self.CreateBitmap()

        wx.DragImage.__init__(self, self._bitmap)


    def CreateBitmap(self):
        """Actually creates the dnd bitmap."""

        memory = wx.MemoryDC()

        bitmap = wx.EmptyBitmap(self._total_w, self._total_h)
        memory.SelectObject(bitmap)

        memory.SetTextBackground(self._backgroundColour)
        memory.SetBackground(wx.Brush(self._backgroundColour))
        memory.SetFont(self._font)
        memory.SetTextForeground(self._colour)
        memory.Clear()

        if self._itemimage:
            memory.DrawBitmap(self._itemimage, self._ximagepos, self._yimagepos, True)

        if self._itemcheck:
            memory.DrawBitmap(self._itemcheck, self._xcheckpos, self._ycheckpos, True)

        textrect = wx.Rect(self._xtextpos, self._ytextpos+self._extraH, self._textwidth, self._textheight)
        memory.DrawLabel(self._text, textrect)

        memory.SelectObject(wx.NullBitmap)

        return bitmap        
            
    
# ----------------------------------------------------------------------------
# TreeItemAttr: a structure containing the visual attributes of an item
# ----------------------------------------------------------------------------

class TreeItemAttr:
    """Creates the item attributes (text colour, background colour and font)."""
    
    def __init__(self, colText=wx.NullColour, colBack=wx.NullColour, font=wx.NullFont):
        """
        Default class constructor.
        For internal use: do not call it in your code!
        """
        
        self._colText = colText
        self._colBack = colBack
        self._font = font

    # setters
    def SetTextColour(self, colText):
        """Sets the attribute text colour."""
        
        self._colText = colText


    def SetBackgroundColour(self, colBack):
        """Sets the attribute background colour."""
        
        self._colBack = colBack

        
    def SetFont(self, font):
        """Sets the attribute font."""
        
        self._font = font

        
    # accessors
    def HasTextColour(self):
        """Returns whether the attribute has text colour."""
        
        return self._colText != wx.NullColour


    def HasBackgroundColour(self):
        """Returns whether the attribute has background colour."""
        
        return self._colBack != wx.NullColour


    def HasFont(self):
        """Returns whether the attribute has font."""

        return self._font != wx.NullFont


    # getters
    def GetTextColour(self):
        """Returns the attribute text colour."""
        
        return self._colText

    
    def GetBackgroundColour(self):
        """Returns the attribute background colour."""

        return self._colBack

    
    def GetFont(self):
        """Returns the attribute font."""

        return self._font


# ----------------------------------------------------------------------------
# CommandTreeEvent Is A Special Subclassing Of wx.PyCommandEvent
#
# NB: Note That Not All The Accessors Make Sense For All The Events, See The
# Event Description Below. 
# ----------------------------------------------------------------------------

class CommandTreeEvent(wx.PyCommandEvent):
    """
    CommandTreeEvent is a special subclassing of wx.PyCommandEvent.
    NB: note that not all the accessors make sense for all the events, see the
    event description for every method in this class. 
    """
    
    def __init__(self, type, id, item=None, evtKey=None, point=None,
                 label=None, **kwargs):
        """
        Default class constructor.
        For internal use: do not call it in your code!
        """

        wx.PyCommandEvent.__init__(self, type, id, **kwargs)
        self._item = item
        self._evtKey = evtKey
        self._pointDrag = point
        self._label = label
        

    def GetItem(self):
        """
        Gets the item on which the operation was performed or the newly selected
        item for EVT_TREE_SEL_CHANGED/ING events.
        """
        
        return self._item

    
    def SetItem(self, item):
        """
        Sets the item on which the operation was performed or the newly selected
        item for EVT_TREE_SEL_CHANGED/ING events.
        """

        self._item = item


    def GetOldItem(self):
        """For EVT_TREE_SEL_CHANGED/ING events, gets the previously selected item."""

        return self._itemOld
    

    def SetOldItem(self, item):
        """For EVT_TREE_SEL_CHANGED/ING events, sets the previously selected item."""
        
        self._itemOld = item


    def GetPoint(self):
        """
        Returns the point where the mouse was when the drag operation started
        (for EVT_TREE_BEGIN(R)DRAG events only) or the click position.
        """

        return self._pointDrag

    
    def SetPoint(self, pt):
        """
        Sets the point where the mouse was when the drag operation started
        (for EVT_TREE_BEGIN(R)DRAG events only) or the click position.
        """
        
        self._pointDrag = pt


    def GetKeyEvent(self):
        """Keyboard data (for EVT_TREE_KEY_DOWN only)."""
        
        return self._evtKey


    def GetKeyCode(self):
        """Returns the integer key code (for EVT_TREE_KEY_DOWN only)."""

        return self._evtKey.GetKeyCode()

    
    def SetKeyEvent(self, evt):
        """Keyboard data (for EVT_TREE_KEY_DOWN only)."""

        self._evtKey = evt
        

    def GetLabel(self):
        """Returns the label-itemtext (for EVT_TREE_BEGIN|END_LABEL_EDIT only)."""

        return self._label

    
    def SetLabel(self, label):
        """Sets the label-itemtext (for EVT_TREE_BEGIN|END_LABEL_EDIT only)."""

        self._label = label


    def IsEditCancelled(self):
        """Returns the edit cancel flag (for EVT_TREE_BEGIN|END_LABEL_EDIT only)."""

        return self._editCancelled


    def SetEditCanceled(self, editCancelled):
        """Sets the edit cancel flag (for EVT_TREE_BEGIN|END_LABEL_EDIT only)."""

        self._editCancelled = editCancelled


    def SetToolTip(self, toolTip):
        """Sets the tooltip for the item (for EVT_TREE_ITEM_GETTOOLTIP events)."""

        self._label = toolTip

        
    def GetToolTip(self):
        """Gets the tooltip for the item (for EVT_TREE_ITEM_GETTOOLTIP events)."""

        return self._label
    

# ----------------------------------------------------------------------------
# TreeEvent is a special class for all events associated with tree controls
#
# NB: note that not all accessors make sense for all events, see the event
#     descriptions below
# ----------------------------------------------------------------------------

class TreeEvent(CommandTreeEvent):
    
    def __init__(self, type, id, item=None, evtKey=None, point=None,
                 label=None, **kwargs):
        """
        Default class constructor.
        For internal use: do not call it in your code!
        """

        CommandTreeEvent.__init__(self, type, id, item, evtKey, point, label, **kwargs)
        self.notify = wx.NotifyEvent(type, id)


    def GetNotifyEvent(self):
        """Returns the actual wx.NotifyEvent."""
        
        return self.notify


    def IsAllowed(self):
        """Returns whether the event is allowed or not."""

        return self.notify.IsAllowed()


    def Veto(self):
        """Vetos the event."""

        self.notify.Veto()


    def Allow(self):
        """The event is allowed."""

        self.notify.Allow()
        
    
# -----------------------------------------------------------------------------
# Auxiliary Classes: TreeRenameTimer
# -----------------------------------------------------------------------------

class TreeRenameTimer(wx.Timer):
    """Timer used for enabling in-place edit."""

    def __init__(self, owner):
        """
        Default class constructor.
        For internal use: do not call it in your code!
        """
        
        wx.Timer.__init__(self)
        self._owner = owner        


    def Notify(self):
        """The timer has expired."""

        self._owner.OnRenameTimer()


# -----------------------------------------------------------------------------
# Auxiliary Classes: TreeTextCtrl
# This Is The Temporary wx.TextCtrl Created When You Edit The Text Of An Item
# -----------------------------------------------------------------------------

class TreeTextCtrl(wx.TextCtrl):
    """Control used for in-place edit."""

    def __init__(self, owner, item=None):
        """
        Default class constructor.
        For internal use: do not call it in your code!
        """
        
        self._owner = owner
        self._itemEdited = item
        self._startValue = item.GetText()
        self._finished = False
        self._aboutToFinish = False

        w = self._itemEdited.GetWidth()
        h = self._itemEdited.GetHeight()

        wnd = self._itemEdited.GetWindow()
        if wnd:
            w = w - self._itemEdited.GetWindowSize()[0]
            h = 0

        x, y = self._owner.CalcScrolledPosition(item.GetX(), item.GetY())

        image_h = 0
        image_w = 0

        image = item.GetCurrentImage()

        if image != _NO_IMAGE:
    
            if self._owner._imageListNormal:
                image_w, image_h = self._owner._imageListNormal.GetSize(image)
                image_w += 4
        
            else:
        
                raise "\n ERROR: You Must Create An Image List To Use Images!"

        checkimage = item.GetCurrentCheckedImage()

        if checkimage is not None:
            wcheck, hcheck = self._owner._imageListCheck.GetSize(checkimage)
            wcheck += 4
        else:
            wcheck = 0            

        if wnd:
            h = max(hcheck, image_h)
            dc = wx.ClientDC(self._owner)
            h = max(h, dc.GetTextExtent("Aq")[1])
            h = h + 2
            
        # FIXME: what are all these hardcoded 4, 8 and 11s really?
        x += image_w + wcheck
        w -= image_w + 4 + wcheck

        if wx.Platform == "__WXMAC__":
            bs = self.DoGetBestSize() 
            # edit control height
            if h > bs.y - 8:
                diff = h - ( bs.y - 8 ) 
                h -= diff 
                y += diff / 2 

        wx.TextCtrl.__init__(self, self._owner, wx.ID_ANY, self._startValue,
                             wx.Point(x - 4, y), wx.Size(w + 15, h))
        
        self.Bind(wx.EVT_CHAR, self.OnChar)
        self.Bind(wx.EVT_KEY_UP, self.OnKeyUp)
        self.Bind(wx.EVT_KILL_FOCUS, self.OnKillFocus)
    

    def AcceptChanges(self):
        """Accepts/refuses the changes made by the user."""

        value = self.GetValue()

        if value == self._startValue:
            # nothing changed, always accept
            # when an item remains unchanged, the owner
            # needs to be notified that the user decided
            # not to change the tree item label, and that
            # the edit has been cancelled
            self._owner.OnRenameCancelled(self._itemEdited)
            return True

        if not self._owner.OnRenameAccept(self._itemEdited, value):
            # vetoed by the user
            return False

        # accepted, do rename the item
        self._owner.SetItemText(self._itemEdited, value)
        
        return True


    def Finish(self):
        """Finish editing."""

        if not self._finished:
        
##            wxPendingDelete.Append(this)
            self._finished = True
            self._owner.SetFocusIgnoringChildren()
            self._owner.ResetTextControl()
        

    def OnChar(self, event):
        """Handles the wx.EVT_CHAR event for TreeTextCtrl."""

        keycode = event.GetKeyCode()

        if keycode == wx.WXK_RETURN:
            self._aboutToFinish = True
            # Notify the owner about the changes
            self.AcceptChanges()
            # Even if vetoed, close the control (consistent with MSW)
            wx.CallAfter(self.Finish)

        elif keycode == wx.WXK_ESCAPE:
            self.StopEditing()

        else:
            event.Skip()
    

    def OnKeyUp(self, event):
        """Handles the wx.EVT_KEY_UP event for TreeTextCtrl."""

        if not self._finished:

            # auto-grow the textctrl:
            parentSize = self._owner.GetSize()
            myPos = self.GetPosition()
            mySize = self.GetSize()
            
            sx, sy = self.GetTextExtent(self.GetValue() + "M")
            if myPos.x + sx > parentSize.x:
                sx = parentSize.x - myPos.x
            if mySize.x > sx:
                sx = mySize.x
                
            self.SetSize((sx, -1))

        event.Skip()


    def OnKillFocus(self, event):
        """Handles the wx.EVT_KILL_FOCUS event for TreeTextCtrl."""

        # I commented out those lines, and everything seems to work fine.
        # But why in the world are these lines of code here? Maybe GTK
        # or MAC give troubles?
        
##        if not self._finished and not self._aboutToFinish:
##        
##            # We must finish regardless of success, otherwise we'll get
##            # focus problems:
##            
##            if not self.AcceptChanges():
##                self._owner.OnRenameCancelled(self._itemEdited)
        
        # We must let the native text control handle focus, too, otherwise
        # it could have problems with the cursor (e.g., in wxGTK).
        event.Skip()


    def StopEditing(self):
        """Suddenly stops the editing."""

        self._owner.OnRenameCancelled(self._itemEdited)
        self.Finish()
        
    
    def item(self):
        """Returns the item currently edited."""

        return self._itemEdited 


# -----------------------------------------------------------------------------
# Auxiliary Classes: TreeFindTimer
# Timer Used To Clear CustomTreeCtrl._findPrefix If No Key Was Pressed For A
# Sufficiently Long Time.
# -----------------------------------------------------------------------------

class TreeFindTimer(wx.Timer):
    """
    Timer used to clear CustomTreeCtrl._findPrefix if no key was pressed
    for a sufficiently long time.
    """

    def __init__(self, owner):
        """
        Default class constructor.
        For internal use: do not call it in your code!
        """

        wx.Timer.__init__(self)
        self._owner = owner


    def Notify(self):
        """The timer has expired."""

        self._owner._findPrefix = ""


# -----------------------------------------------------------------------------
# GenericTreeItem Implementation.
# This Class Holds All The Information And Methods For Every Single Item In
# CustomTreeCtrl.
# -----------------------------------------------------------------------------

class GenericTreeItem:
    """
    This class holds all the information and methods for every single item in
    CustomTreeCtrl. No wx based.
    """
    
    def __init__(self, parent, text="", ct_type=0, wnd=None, image=-1, selImage=-1, data=None):
        """
        Default class constructor.
        For internal use: do not call it in your code!
        """
        
        # since there can be very many of these, we save size by chosing
        # the smallest representation for the elements and by ordering
        # the members to avoid padding.
        self._text = text       # label to be rendered for item
        self._data = data       # user-provided data

        self._children  = []    # list of children
        self._parent = parent   # parent of this item

        self._attr = None       # attributes???

        # tree ctrl images for the normal, selected, expanded and
        # expanded+selected states
        self._images = [-1, -1, -1, -1]
        self._images[TreeItemIcon_Normal] = image
        self._images[TreeItemIcon_Selected] = selImage
        self._images[TreeItemIcon_Expanded] = _NO_IMAGE
        self._images[TreeItemIcon_SelectedExpanded] = _NO_IMAGE

        self._checkedimages = [None, None, None, None]

        self._x = 0             # (virtual) offset from top
        self._y = 0             # (virtual) offset from left
        self._width = 0         # width of this item
        self._height = 0        # height of this item

        self._isCollapsed = True
        self._hasHilight = False    # same as focused
        self._hasPlus = False       # used for item which doesn't have
                                    # children but has a [+] button
        self._isBold = False        # render the label in bold font
        self._isItalic = False      # render the label in italic font
        self._ownsAttr = False      # delete attribute when done
        self._type = ct_type        # item type: 0=normal, 1=check, 2=radio
        self._checked = False       # only meaningful for check and radio
        self._enabled = True        # flag to enable/disable an item
        self._hypertext = False     # indicates if the item is hypertext
        self._visited = False       # visited state for an hypertext item

        if self._type > 0:
            # do not construct the array for normal items
            self._checkedimages[TreeItemIcon_Checked] = 0
            self._checkedimages[TreeItemIcon_NotChecked] = 1
            self._checkedimages[TreeItemIcon_Flagged] = 2
            self._checkedimages[TreeItemIcon_NotFlagged] = 3
        
        if parent:
            if parent.GetType() == 2 and not parent.IsChecked():
                # if the node parent is a radio not enabled, we are disabled
                self._enabled = False

        self._wnd = wnd             # are we holding a window?

        if wnd:
            if wnd.GetSizer():      # the window is a complex one hold by a sizer
                size = wnd.GetBestSize()
            else:                   # simple window, without sizers
                size = wnd.GetSize()

            # We have to bind the wx.EVT_SET_FOCUS for the associated window
            # No other solution to handle the focus changing from an item in
            # CustomTreeCtrl and the window associated to an item
            # Do better strategies exist?
            self._wnd.Bind(wx.EVT_SET_FOCUS, self.OnSetFocus)
            
            self._height = size.GetHeight() + 2
            self._width = size.GetWidth()
            self._windowsize = size
            
            # We don't show the window if the item is collapsed
            if self._isCollapsed:
                self._wnd.Show(False)

            # The window is enabled only if the item is enabled                
            self._wnd.Enable(self._enabled)
            self._windowenabled = self._enabled
        

    def IsOk(self):
        """
        Returns whether the item is ok or not. Useless on Python, but added for
        backward compatibility with the C++ implementation.
        """
        
        return True
    

    def GetChildren(self):
        """Returns the item's children."""

        return self._children 


    def GetText(self):
        """Returns the item text."""

        return self._text 


    def GetImage(self, which=TreeItemIcon_Normal):
        """Returns the item image for a particular state."""
        
        return self._images[which] 


    def GetCheckedImage(self, which=TreeItemIcon_Checked):
        """Returns the item check image. Meaningful only for radio & check items."""

        return self._checkedimages[which]
        

    def GetData(self):
        """Returns the data associated to this item."""
        
        return self._data 


    def SetImage(self, image, which):
        """Sets the item image."""

        self._images[which] = image

        
    def SetData(self, data):
        """Sets the data associated to this item."""

        self._data = data 


    def SetHasPlus(self, has=True):
        """Sets whether an item has the 'plus' button."""

        self._hasPlus = has 


    def SetBold(self, bold):
        """Sets the item font bold."""

        self._isBold = bold 


    def SetItalic(self, italic):
        """Sets the item font italic."""

        self._isItalic = italic
        

    def GetX(self):
        """Returns the x position on an item in the ScrolledWindow."""

        return self._x 


    def GetY(self):
        """Returns the y position on an item in the ScrolledWindow."""

        return self._y 


    def SetX(self, x):
        """Sets the x position on an item in the ScrolledWindow."""

        self._x = x 


    def SetY(self, y):
        """Sets the y position on an item in the ScrolledWindow."""

        self._y = y 


    def GetHeight(self):
        """Returns the height of the item."""

        return self._height 


    def GetWidth(self):
        """Returns the width of the item."""

        return self._width 


    def SetHeight(self, h):
        """Sets the height of the item."""

        self._height = h

        
    def SetWidth(self, w):
        """Sets the width of the item."""

        self._width = w 


    def SetWindow(self, wnd):
        """Sets the window associated to the item."""

        self._wnd = wnd


    def GetWindow(self):
        """Returns the window associated to the item."""

        return self._wnd        


    def GetWindowEnabled(self):
        """Returns whether the associated window is enabled or not."""

        if not self._wnd:
            raise "\nERROR: This Item Has No Window Associated"

        return self._windowenabled


    def SetWindowEnabled(self, enable=True):
        """Sets whether the associated window is enabled or not."""

        if not self._wnd:
            raise "\nERROR: This Item Has No Window Associated"

        self._windowenabled = enable
        self._wnd.Enable(enable)


    def GetWindowSize(self):
        """Returns the associated window size."""
        
        return self._windowsize        


    def OnSetFocus(self, event):
        """Handles the wx.EVT_SET_FOCUS event for the associated window."""

        treectrl = self._wnd.GetParent()
        select = treectrl.GetSelection()

        # If the window is associated to an item that currently is selected
        # (has focus) we don't kill the focus. Otherwise we do it.
        if select != self:
            treectrl._hasFocus = False
        else:
            treectrl._hasFocus = True
            
        event.Skip()


    def GetType(self):
        """
        Returns the item type. It should be one of:
        0: normal items
        1: checkbox item
        2: radiobutton item
        """

        return self._type
    

    def SetHyperText(self, hyper=True):
        """Sets whether the item is hypertext or not."""
        
        self._hypertext = hyper


    def SetVisited(self, visited=True):
        """Sets whether an hypertext item was visited or not."""

        self._visited = visited


    def GetVisited(self):
        """Returns whether an hypertext item was visited or not."""

        return self._visited        


    def IsHyperText(self):
        """Returns whether the item is hypetext or not."""

        return self._hypertext
    

    def GetParent(self):
        """Gets the item parent."""

        return self._parent 


    def Insert(self, child, index):
        """Inserts an item in the item children."""
        
        self._children.insert(index, child) 


    def Expand(self):
        """Expand the item."""

        self._isCollapsed = False 
        

    def Collapse(self):
        """Collapse the item."""

        self._isCollapsed = True
            

    def SetHilight(self, set=True):
        """Sets the item focus/unfocus."""

        self._hasHilight = set 


    def HasChildren(self):
        """Returns whether the item has children or not."""

        return len(self._children) > 0


    def IsSelected(self):
        """Returns whether the item is selected or not."""

        return self._hasHilight != 0 


    def IsExpanded(self):
        """Returns whether the item is expanded or not."""

        return not self._isCollapsed 


    def IsChecked(self):
        """Returns whether the item is checked or not."""

        return self._checked


    def Check(self, checked=True):
        """Check an item. Meaningful only for check and radio items."""

        self._checked = checked        


    def HasPlus(self):
        """Returns whether the item has the plus button or not."""

        return self._hasPlus or self.HasChildren() 


    def IsBold(self):
        """Returns whether the item font is bold or not."""

        return self._isBold != 0 


    def IsItalic(self):
        """Returns whether the item font is italic or not."""

        return self._isItalic != 0 


    def Enable(self, enable=True):
        """Enables/disables the item."""

        self._enabled = enable


    def IsEnabled(self):
        """Returns whether the item is enabled or not."""

        return self._enabled
    

    def GetAttributes(self):
        """Returns the item attributes (font, colours)."""

        return self._attr 


    def Attr(self):
        """Creates a new attribute (font, colours)."""
    
        if not self._attr:
        
            self._attr = TreeItemAttr()
            self._ownsAttr = True
        
        return self._attr

    
    def SetAttributes(self, attr):
        """Sets the item attributes (font, colours)."""
    
        if self._ownsAttr:
             del self._attr
             
        self._attr = attr
        self._ownsAttr = False

    
    def AssignAttributes(self, attr):
        """Assigns the item attributes (font, colours)."""
    
        self.SetAttributes(attr)
        self._ownsAttr = True


    def DeleteChildren(self, tree):
        """Deletes the item children."""

        for child in self._children:
            if tree:
                tree.SendDeleteEvent(child)

            child.DeleteChildren(tree)
            
            if child == tree._select_me:
                tree._select_me = None

            # We have to destroy the associated window
            wnd = child.GetWindow()
            if wnd:
                wnd.Destroy()
                child._wnd = None

            if child in tree._itemWithWindow:
                tree._itemWithWindow.remove(child)
                
            del child
        
        self._children = []


    def SetText(self, text):
        """Sets the item text."""

        self._text = text


    def GetChildrenCount(self, recursively=True):
        """Gets the number of children."""

        count = len(self._children)
        
        if not recursively:
            return count

        total = count

        for n in xrange(count):
            total += self._children[n].GetChildrenCount()
        
        return total


    def GetSize(self, x, y, theButton):
        """Returns the item size."""

        bottomY = self._y + theButton.GetLineHeight(self)

        if y < bottomY:
            y = bottomY

        width = self._x + self._width
        
        if x < width:
            x = width

        if self.IsExpanded():
            for child in self._children:
                x, y = child.GetSize(x, y, theButton)
            
        return x, y        


    def HitTest(self, point, theCtrl, flags=0, level=0):
        """
        HitTest method for an item. Called from the main window HitTest.
        see the CustomTreeCtrl HitTest method for the flags explanation.
        """
        
        # for a hidden root node, don't evaluate it, but do evaluate children
        if not (level == 0 and theCtrl.HasFlag(TR_HIDE_ROOT)):
        
            # evaluate the item
            h = theCtrl.GetLineHeight(self)
            
            if point.y > self._y and point.y < self._y + h:
            
                y_mid = self._y + h/2

                if point.y < y_mid:
                    flags |= TREE_HITTEST_ONITEMUPPERPART
                else:
                    flags |= TREE_HITTEST_ONITEMLOWERPART

                xCross = self._x - theCtrl.GetSpacing()

                if wx.Platform == "__WXMAC__":
                    # according to the drawing code the triangels are drawn
                    # at -4 , -4  from the position up to +10/+10 max
                    if point.x > xCross-4 and point.x < xCross+10 and point.y > y_mid-4 and \
                       point.y < y_mid+10 and self.HasPlus() and theCtrl.HasButtons():

                        flags |= TREE_HITTEST_ONITEMBUTTON
                        return self, flags
                else:
                    # 5 is the size of the plus sign
                    if point.x > xCross-6 and point.x < xCross+6 and point.y > y_mid-6 and \
                       point.y < y_mid+6 and self.HasPlus() and theCtrl.HasButtons():

                        flags |= TREE_HITTEST_ONITEMBUTTON
                        return self, flags

                if point.x >= self._x and point.x <= self._x + self._width:

                    image_w = -1
                    wcheck = 0

                    # assuming every image (normal and selected) has the same size!
                    if self.GetImage() != _NO_IMAGE and theCtrl._imageListNormal:
                        image_w, image_h = theCtrl._imageListNormal.GetSize(self.GetImage())

                    if self.GetCheckedImage() is not None:
                        wcheck, hcheck = theCtrl._imageListCheck.GetSize(self.GetCheckedImage())

                    if wcheck and point.x <= self._x + wcheck + 1:
                        flags |= TREE_HITTEST_ONITEMCHECKICON
                        return self, flags

                    if image_w != -1 and point.x <= self._x + wcheck + image_w + 1:
                        flags |= TREE_HITTEST_ONITEMICON
                    else:
                        flags |= TREE_HITTEST_ONITEMLABEL

                    return self, flags

                if point.x < self._x:
                    flags |= TREE_HITTEST_ONITEMINDENT
                if point.x > self._x + self._width:
                    flags |= TREE_HITTEST_ONITEMRIGHT
                        
                return self, flags
            
            # if children are expanded, fall through to evaluate them
            if self._isCollapsed:
                return None, 0
        
        # evaluate children
        for child in self._children:
            res, flags = child.HitTest(point, theCtrl, flags, level + 1)
            if res != None:
                return res, flags

        return None, 0


    def GetCurrentImage(self):
        """Returns the current item image."""

        image = _NO_IMAGE
        
        if self.IsExpanded():
        
            if self.IsSelected():
            
                image = self.GetImage(TreeItemIcon_SelectedExpanded)

            if image == _NO_IMAGE:
            
                # we usually fall back to the normal item, but try just the
                # expanded one (and not selected) first in this case
                image = self.GetImage(TreeItemIcon_Expanded)
        
        else: # not expanded
        
            if self.IsSelected():
                image = self.GetImage(TreeItemIcon_Selected)
        
        # maybe it doesn't have the specific image we want,
        # try the default one instead
        if image == _NO_IMAGE:
            image = self.GetImage()

        return image


    def GetCurrentCheckedImage(self):
        """Returns the current item check image."""

        if self._type == 0:
            return None

        if self.IsChecked():
            if self._type == 1:     # Checkbox
                return self._checkedimages[TreeItemIcon_Checked]
            else:                   # Radiobutton
                return self._checkedimages[TreeItemIcon_Flagged]
        else:
            if self._type == 1:     # Checkbox
                return self._checkedimages[TreeItemIcon_NotChecked]
            else:                   # Radiobutton
                return self._checkedimages[TreeItemIcon_NotFlagged]
            

def EventFlagsToSelType(style, shiftDown=False, ctrlDown=False):
    """
    Translate the key or mouse event flag to the type of selection we
    are dealing with.
    """

    is_multiple = (style & TR_MULTIPLE) != 0
    extended_select = shiftDown and is_multiple
    unselect_others = not (extended_select or (ctrlDown and is_multiple))

    return is_multiple, extended_select, unselect_others


# -----------------------------------------------------------------------------
# CustomTreeCtrl Main Implementation.
# This Is The Main Class.
# -----------------------------------------------------------------------------

class CustomTreeCtrl(wx.ScrolledWindow):

    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=0, ctstyle=TR_DEFAULT_STYLE, validator=wx.DefaultValidator,
                 name="CustomTreeCtrl"):
        """
        Default class constructor.
        
        parent: parent window. Must not be none.

        id: window identifier. A value of -1 indicates a default value.

        pos: window position.

        size: window size. If the default size (-1, -1) is specified then the window is sized appropriately.

        style: the underlying wx.ScrolledWindow style
        
        ctstyle: CustomTreeCtrl window style. This can be one of:
            TR_NO_BUTTONS
            TR_HAS_BUTTONS                          # draw collapsed/expanded btns
            TR_NO_LINES                             # don't draw lines at all
            TR_LINES_AT_ROOT                        # connect top-level nodes
            TR_TWIST_BUTTONS                        # draw mac-like twist buttons
            TR_SINGLE                               # single selection mode                           
            TR_MULTIPLE                             # can select multiple items
            TR_EXTENDED                             # todo: allow extended selection
            TR_HAS_VARIABLE_ROW_HEIGHT              # allows rows to have variable height
            TR_EDIT_LABELS                          # can edit item labels
            TR_ROW_LINES                            # put border around items
            TR_HIDE_ROOT                            # don't display root node
            TR_FULL_ROW_HIGHLIGHT                   # highlight full horizontal space
            TR_AUTO_CHECK_CHILD                     # only meaningful for checkboxes
            TR_AUTO_TOGGLE_CHILD                    # only meaningful for checkboxes

        validator: window validator.

        name: window name.
        """
        
        self._current = self._key_current = self._anchor = self._select_me = None
        self._hasFocus = False
        self._dirty = False

        # Default line height: it will soon be changed
        self._lineHeight = 10
        # Item indent wrt parent
        self._indent = 15
        # item horizontal spacing between the start and the text
        self._spacing = 18

        # Brushes for focused/unfocused items (also gradient type)
        self._hilightBrush = wx.Brush(wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHT))
        btnshadow = wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW)
        self._hilightUnfocusedBrush = wx.Brush(btnshadow)
        r, g, b = btnshadow.Red(), btnshadow.Green(), btnshadow.Blue()
        backcolour = ((r >> 1) - 20, (g >> 1) - 20, (b >> 1) - 20)
        backcolour = wx.Colour(backcolour[0], backcolour[1], backcolour[2])
        self._hilightUnfocusedBrush2 = wx.Brush(backcolour)

        # image list for icons
        self._imageListNormal = self._imageListButtons = self._imageListState = self._imageListCheck = None
        self._ownsImageListNormal = self._ownsImageListButtons = self._ownsImageListState = False

        # Drag and drop initial settings
        self._dragCount = 0
        self._countDrag = 0
        self._isDragging = False
        self._dropTarget = self._oldSelection = None
        self._dragImage = None
        self._underMouse = None

        # TextCtrl initial settings for editable items        
        self._textCtrl = None
        self._renameTimer = None

        # This one allows us to handle Freeze() and Thaw() calls        
        self._freezeCount = 0

        self._findPrefix = ""
        self._findTimer = None

        self._dropEffectAboveItem = False
        self._lastOnSame = False

        # Default normal and bold fonts for an item
        self._hasFont = True
        self._normalFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        self._boldFont = wx.Font(self._normalFont.GetPointSize(), self._normalFont.GetFamily(),
                                 self._normalFont.GetStyle(), wx.BOLD, self._normalFont.GetUnderlined(),
                                 self._normalFont.GetFaceName(), self._normalFont.GetEncoding())


        # Hyperlinks things
        self._hypertextfont = wx.Font(self._normalFont.GetPointSize(), self._normalFont.GetFamily(),
                                      self._normalFont.GetStyle(), wx.NORMAL, True,
                                      self._normalFont.GetFaceName(), self._normalFont.GetEncoding())
        self._hypertextnewcolour = wx.BLUE
        self._hypertextvisitedcolour = wx.Colour(200, 47, 200)
        self._isonhyperlink = False

        # Default CustomTreeCtrl background colour.    
        self._backgroundColour = wx.WHITE
        
        # Background image settings
        self._backgroundImage = None
        self._imageStretchStyle = _StyleTile

        # Disabled items colour        
        self._disabledColour = wx.Colour(180, 180, 180)

        # Gradient selection colours        
        self._firstcolour = color= wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHT)
        self._secondcolour = wx.WHITE
        self._usegradients = False
        self._gradientstyle = 0   # Horizontal Gradient

        # Vista Selection Styles
        self._vistaselection = False        

        # Connection lines style
        if wx.Platform != "__WXMAC__":
            self._dottedPen = wx.Pen("grey", 1, wx.USER_DASH)
            self._dottedPen.SetDashes([1,1])
            self._dottedPen.SetCap(wx.CAP_BUTT)
        else:
            self._dottedPen = wx.Pen("grey", 1)

        # Pen Used To Draw The Border Around Selected Items
        self._borderPen = wx.BLACK_PEN
        self._cursor = wx.StockCursor(wx.CURSOR_ARROW)
        
        # For Appended Windows
        self._hasWindows = False
        self._itemWithWindow = []
        
        if wx.Platform == "__WXMAC__":
            
            platform, major, minor = wx.GetOsVersion()

            ctstyle &= ~TR_LINES_AT_ROOT
            ctstyle |= TR_NO_LINES
            
            if major < 10:
                ctstyle |= TR_ROW_LINES

        self._windowStyle = ctstyle

        # Create the default check image list        
        self.SetImageListCheck(13, 13)

        # A constant to use my translation of RendererNative.DrawTreeItemButton
        # if the wxPython version is less than 2.6.2.1.
        if wx.VERSION_STRING < "2.6.2.1":
            self._drawingfunction = DrawTreeItemButton
        else:
            self._drawingfunction = wx.RendererNative.Get().DrawTreeItemButton

        # Create our container... at last!    
        wx.ScrolledWindow.__init__(self, parent, id, pos, size, style|wx.HSCROLL|wx.VSCROLL, name)

        # If the tree display has no buttons, but does have
        # connecting lines, we can use a narrower layout.
        # It may not be a good idea to force this...
        if not self.HasButtons() and not self.HasFlag(TR_NO_LINES):
            self._indent= 10
            self._spacing = 10
        
        self.SetValidator(validator)

        attr = self.GetDefaultAttributes()
        self.SetOwnForegroundColour(attr.colFg)
        self.SetOwnBackgroundColour(wx.WHITE)
        
        if not self._hasFont:
            self.SetOwnFont(attr.font)

        self.SetSize(size)

        # Bind the events
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_MOUSE_EVENTS, self.OnMouse)
        self.Bind(wx.EVT_KEY_DOWN, self.OnKeyDown)
        self.Bind(wx.EVT_SET_FOCUS, self.OnSetFocus)
        self.Bind(wx.EVT_KILL_FOCUS, self.OnKillFocus)
        self.Bind(EVT_TREE_ITEM_GETTOOLTIP, self.OnGetToolTip)
        self.Bind(wx.EVT_IDLE, self.OnInternalIdle)
        self.Bind(wx.EVT_WINDOW_DESTROY, self.OnDestroy)

        # Sets the focus to ourselves: this is useful if you have items
        # with associated widgets.
        self.SetFocus()



    def OnDestroy(self, event):
        """Handles the wx.EVT_WINDOW_DESTROY event."""

        # Here there may be something I miss... do I have to destroy
        # something else?
        if self._renameTimer and self._renameTimer.IsRunning():
            self._renameTimer.Stop()
            del self._renameTimer

        if self._findTimer and self._findTimer.IsRunning():
            self._findTimer.Stop()
            del self._findTimer

        event.Skip()

        
    def GetCount(self):
        """Returns the global number of items in the tree."""

        if not self._anchor:
            # the tree is empty
            return 0

        count = self._anchor.GetChildrenCount()
        
        if not self.HasFlag(TR_HIDE_ROOT):
            # take the root itself into account
            count = count + 1
        
        return count


    def GetIndent(self):
        """Returns the item indentation."""

        return self._indent

    
    def GetSpacing(self):
        """Returns the spacing between the start and the text."""

        return self._spacing


    def GetRootItem(self):
        """Returns the root item."""

        return self._anchor


    def GetSelection(self):
        """Returns the current selection: TR_SINGLE only."""

        return self._current


    def ToggleItemSelection(self, item):
        """Toggles the item selection."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        self.SelectItem(item, not self.IsSelected(item))


    def EnableChildren(self, item, enable=True):
        """Enables/disables item children. Used internally."""

        torefresh = False
        if item.IsExpanded():
            torefresh = True

        if item.GetType() == 2 and enable and not item.IsChecked():
            # We hit a radiobutton item not checked, we don't want to
            # enable the children
            return
        
        child, cookie = self.GetFirstChild(item)
        while child:
            self.EnableItem(child, enable, torefresh=torefresh)
            # Recurse on tree
            if child.GetType != 2 or (child.GetType() == 2 and item.IsChecked()):
                self.EnableChildren(child, enable)
            (child, cookie) = self.GetNextChild(item, cookie)


    def EnableItem(self, item, enable=True, torefresh=True):
        """Enables/disables an item."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        if item.IsEnabled() == enable:
            return

        if not enable and item.IsSelected():
            self.SelectItem(item, False)

        item.Enable(enable)
        wnd = item.GetWindow()

        # Handles the eventual window associated to the item        
        if wnd:
            wndenable = item.GetWindowEnabled()
            if enable:
                if wndenable:
                    wnd.Enable(enable)
            else:
                wnd.Enable(enable)
        
        if torefresh:
            # We have to refresh the item line
            dc = wx.ClientDC(self)
            self.CalculateSize(item, dc)
            self.RefreshLine(item)
                

    def IsEnabled(self, item):
        """Returns whether an item is enabled or disabled."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        return item.IsEnabled()        


    def SetDisabledColour(self, colour):
        """Sets the items disabled colour."""
        
        self._disabledColour = colour
        self._dirty = True


    def GetDisabledColour(self):
        """Returns the items disabled colour."""

        return self._disabledColour        
        

    def IsItemChecked(self, item):
        """Returns whether an item is checked or not."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "        

        return item.IsChecked()


    def CheckItem2(self, item, checked=True, torefresh=False):
        """Used internally to avoid EVT_TREE_ITEM_CHECKED events."""

        if item.GetType() == 0:
            return
        
        item.Check(checked)

        if torefresh:
            dc = wx.ClientDC(self)
            self.CalculateSize(item, dc)
            self.RefreshLine(item)
        

    def UnCheckRadioParent(self, item, checked=False):
        """Used internally to handle radio node parent correctly."""

        e = TreeEvent(wxEVT_TREE_ITEM_CHECKING, self.GetId())
        e.SetItem(item)
        e.SetEventObject(self)
        
        if self.GetEventHandler().ProcessEvent(e):
            return False

        item.Check(checked)
        dc = wx.ClientDC(self)
        self.RefreshLine(item)
        self.EnableChildren(item, checked)
        e = TreeEvent(wxEVT_TREE_ITEM_CHECKED, self.GetId())
        e.SetItem(item)
        e.SetEventObject(self)
        self.GetEventHandler().ProcessEvent(e)

        return True        
        

    def CheckItem(self, item, checked=True):
        """
        Actually checks/uncheks an item, sending (eventually) the two
        events EVT_TREE_ITEM_CHECKING/EVT_TREE_ITEM_CHECKED.
        """

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        # Should we raise an error here?!?        
        if item.GetType() == 0:
            return

        if item.GetType() == 2:    # it's a radio button
            if not checked and item.IsChecked():  # Try To Unckeck?
                if item.HasChildren():
                    self.UnCheckRadioParent(item, checked)
                return
            else:
                if not self.UnCheckRadioParent(item, checked):
                    return

                self.CheckSameLevel(item, False)
                return
            
        # Radiobuttons are done, let's handle checkbuttons...
        e = TreeEvent(wxEVT_TREE_ITEM_CHECKING, self.GetId())
        e.SetItem(item)
        e.SetEventObject(self)
        
        if self.GetEventHandler().ProcessEvent(e):
            # Blocked by user
            return 
        
        item.Check(checked)
        dc = wx.ClientDC(self)
        self.RefreshLine(item)

        if self._windowStyle & TR_AUTO_CHECK_CHILD:
            ischeck = self.IsItemChecked(item)
            self.AutoCheckChild(item, ischeck)
        elif self._windowStyle & TR_AUTO_TOGGLE_CHILD:
            self.AutoToggleChild(item)

        e = TreeEvent(wxEVT_TREE_ITEM_CHECKED, self.GetId())
        e.SetItem(item)
        e.SetEventObject(self)
        self.GetEventHandler().ProcessEvent(e)


    def AutoToggleChild(self, item):
        """Transverses the tree and toggles the items. Meaningful only for check items."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        child, cookie = self.GetFirstChild(item)

        torefresh = False
        if item.IsExpanded():
            torefresh = True

        # Recurse on tree            
        while child:
            if child.GetType() == 1 and child.IsEnabled():
                self.CheckItem2(child, not child.IsChecked(), torefresh=torefresh)
            self.AutoToggleChild(child)
            (child, cookie) = self.GetNextChild(item, cookie)


    def AutoCheckChild(self, item, checked):
        """Transverses the tree and checks/unchecks the items. Meaningful only for check items."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        (child, cookie) = self.GetFirstChild(item)

        torefresh = False
        if item.IsExpanded():
            torefresh = True
            
        while child:
            if child.GetType() == 1 and child.IsEnabled():
                self.CheckItem2(child, checked, torefresh=torefresh)
            self.AutoCheckChild(child, checked)
            (child, cookie) = self.GetNextChild(item, cookie)


    def CheckChilds(self, item, checked=True):
        """Programatically check/uncheck item children. Does not generate EVT_TREE_CHECK* events."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        if checked == None:
            self.AutoToggleChild(item)
        else:
            self.AutoCheckChild(item, checked)


    def CheckSameLevel(self, item, checked=False):
        """
        Uncheck radio items which are on the same level of the checked one.
        Used internally.
        """

        parent = item.GetParent()

        if not parent:
            return

        torefresh = False
        if parent.IsExpanded():
            torefresh = True
        
        (child, cookie) = self.GetFirstChild(parent)
        while child:
            if child.GetType() == 2 and child != item:
                self.CheckItem2(child, checked, torefresh=torefresh)
                if child.GetType != 2 or (child.GetType() == 2 and child.IsChecked()):
                    self.EnableChildren(child, checked)
            (child, cookie) = self.GetNextChild(parent, cookie)


    def EditLabel(self, item):
        """Starts editing an item label."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        self.Edit(item)

        
    def ShouldInheritColours(self):
        """We don't inherit colours from anyone."""

        return False        


    def SetIndent(self, indent):
        """Sets item indentation."""

        self._indent = indent
        self._dirty = True


    def SetSpacing(self, spacing):
        """Sets item spacing."""

        self._spacing = spacing
        self._dirty = True


    def HasFlag(self, flag):
        """Returns whether CustomTreeCtrl has a flag."""

        return self._windowStyle & flag
    

    def HasChildren(self, item):
        """Returns whether an item has children or not."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        return len(item.GetChildren()) > 0


    def GetChildrenCount(self, item, recursively=True):
        """Gets the item children count."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.GetChildrenCount(recursively)


    def SetTreeStyle(self, styles):
        """Sets the CustomTreeCtrl style. See __init__ method for the styles explanation."""

        # Do not try to expand the root node if it hasn't been created yet
        if self._anchor and not self.HasFlag(TR_HIDE_ROOT) and styles & TR_HIDE_ROOT:
        
            # if we will hide the root, make sure children are visible
            self._anchor.SetHasPlus()
            self._anchor.Expand()
            self.CalculatePositions()
        
        # right now, just sets the styles.  Eventually, we may
        # want to update the inherited styles, but right now
        # none of the parents has updatable styles

        if self._windowStyle & TR_MULTIPLE and not (styles & TR_MULTIPLE):
            selections = self.GetSelections()
            for select in selections[0:-1]:
                self.SelectItem(select, False)

        self._windowStyle = styles
        self._dirty = True


    def GetTreeStyle(self):
        """Returns the CustomTreeCtrl style."""

        return self._windowStyle
    

    def HasButtons(self):
        """Returns whether CustomTreeCtrl has the TR_AHS_BUTTONS flag."""

        return self.HasFlag(TR_HAS_BUTTONS)


# -----------------------------------------------------------------------------
# functions to work with tree items
# -----------------------------------------------------------------------------

    def GetItemText(self, item):
        """Returns the item text."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.GetText()
    

    def GetItemImage(self, item, which):
        """Returns the item image."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.GetImage(which)


    def GetPyData(self, item):
        """Returns the data associated to an item."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.GetData()

    GetItemPyData = GetPyData 


    def GetItemTextColour(self, item):
        """Returns the item text colour."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.Attr().GetTextColour()


    def GetItemBackgroundColour(self, item):
        """Returns the item background colour."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.Attr().GetBackgroundColour()


    def GetItemFont(self, item):
        """Returns the item font."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.Attr().GetFont()


    def IsItemHyperText(self, item):
        """Returns whether an item is hypertext or not."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        return item.IsHyperText()


    def SetItemText(self, item, text):
        """Sets the item text."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        dc = wx.ClientDC(self)
        item.SetText(text)
        self.CalculateSize(item, dc)
        self.RefreshLine(item)


    def SetItemImage(self, item, image, which=TreeItemIcon_Normal):
        """Sets the item image, depending on the item state."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        item.SetImage(image, which)

        dc = wx.ClientDC(self)
        self.CalculateSize(item, dc)
        self.RefreshLine(item)


    def SetPyData(self, item, data):
        """Sets the data associated to an item."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        item.SetData(data)

    SetItemPyData = SetPyData
    

    def SetItemHasChildren(self, item, has=True):
        """Forces the appearance of the button next to the item."""
        
        if not item:
            raise "\nERROR: Invalid Tree Item. "

        item.SetHasPlus(has)
        self.RefreshLine(item)


    def SetItemBold(self, item, bold=True):
        """Sets the item font bold/unbold."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        # avoid redrawing the tree if no real change
        if item.IsBold() != bold:
            item.SetBold(bold)
            self._dirty = True
    

    def SetItemItalic(self, item, italic=True):
        """Sets the item font italic/non-italic."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        if item.IsItalic() != italic:
            itemFont = self.GetItemFont(item)
            if itemFont != wx.NullFont:
                style = wx.ITALIC
                if not italic:
                    style = ~style

                item.SetItalic(italic)
                itemFont.SetStyle(style)
                self.SetItemFont(item, itemFont)
                self._dirty = True


    def SetItemDropHighlight(self, item, highlight=True):
        """
        Gives the item the visual feedback for drag and drop operations.
        This is useful when something is dragged from outside the CustomTreeCtrl.
        """

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        if highlight:
            bg = wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHT)
            fg = wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHTTEXT)

        item.Attr().SetTextColour(fg)
        item.Attr.SetBackgroundColour(bg)
        self.RefreshLine(item)


    def SetItemTextColour(self, item, col):
        """Sets the item text colour."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        if self.GetItemTextColour(item) == col:
            return

        item.Attr().SetTextColour(col)
        self.RefreshLine(item)


    def SetItemBackgroundColour(self, item, col):
        """Sets the item background colour."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        item.Attr().SetBackgroundColour(col)
        self.RefreshLine(item)


    def SetItemHyperText(self, item, hyper=True):
        """Sets whether the item is hypertext or not."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "        

        item.SetHyperText(hyper)
        self.RefreshLine(item)
        

    def SetItemFont(self, item, font):
        """Sets the item font."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        if self.GetItemFont(item) == font:
            return

        item.Attr().SetFont(font)
        self._dirty = True
        

    def SetFont(self, font):
        """Sets the CustomTreeCtrl font."""

        wx.ScrolledWindow.SetFont(self, font)

        self._normalFont = font 
        self._boldFont = wx.Font(self._normalFont.GetPointSize(), self._normalFont.GetFamily(),
                                 self._normalFont.GetStyle(), wx.BOLD, self._normalFont.GetUnderlined(),
                                 self._normalFont.GetFaceName(), self._normalFont.GetEncoding())

        return True


    def GetHyperTextFont(self):
        """Returns the font used to render an hypertext item."""

        return self._hypertextfont        


    def SetHyperTextFont(self, font):
        """Sets the font used to render an hypertext item."""

        self._hypertextfont = font
        self._dirty = True
        

    def SetHyperTextNewColour(self, colour):
        """Sets the colour used to render a non-visited hypertext item."""

        self._hypertextnewcolour = colour
        self._dirty = True


    def GetHyperTextNewColour(self):
        """Returns the colour used to render a non-visited hypertext item."""

        return self._hypertextnewcolour


    def SetHyperTextVisitedColour(self, colour):
        """Sets the colour used to render a visited hypertext item."""

        self._hypertextvisitedcolour = colour
        self._dirty = True


    def GetHyperTextVisitedColour(self):
        """Returns the colour used to render a visited hypertext item."""

        return self._hypertextvisitedcolour


    def SetItemVisited(self, item, visited=True):
        """Sets whether an hypertext item was visited."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        item.SetVisited(visited)
        self.RefreshLine(item)


    def GetItemVisited(self, item):
        """Returns whether an hypertext item was visited."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.GetVisited()            


    def SetHilightFocusColour(self, colour):
        """
        Sets the colour used to highlight focused selected items.
        This is applied only if gradient and Windows Vista styles are disabled.
        """

        self._hilightBrush = wx.Brush(colour)
        self.RefreshSelected()
            

    def SetHilightNonFocusColour(self, colour):
        """
        Sets the colour used to highlight unfocused selected items.
        This is applied only if gradient and Windows Vista styles are disabled.
        """

        self._hilightUnfocusedBrush = wx.Brush(colour)
        self.RefreshSelected()


    def GetHilightFocusColour(self):
        """
        Returns the colour used to highlight focused selected items.
        This is applied only if gradient and Windows Vista styles are disabled.
        """

        return self._hilightBrush.GetColour()
            

    def GetHilightNonFocusColour(self):
        """
        Returns the colour used to highlight unfocused selected items.
        This is applied only if gradient and Windows Vista styles are disabled.
        """
        
        return self._hilightUnfocusedBrush.GetColour()

    
    def SetFirstGradientColour(self, colour=None):
        """Sets the first gradient colour."""
        
        if colour is None:
            colour = wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHT)

        self._firstcolour = colour
        if self._usegradients:
            self.RefreshSelected()
            

    def SetSecondGradientColour(self, colour=None):
        """Sets the second gradient colour."""

        if colour is None:
            # No colour given, generate a slightly darker from the
            # CustomTreeCtrl background colour
            color = self.GetBackgroundColour()
            r, g, b = int(color.Red()), int(color.Green()), int(color.Blue())
            color = ((r >> 1) + 20, (g >> 1) + 20, (b >> 1) + 20)
            colour = wx.Colour(color[0], color[1], color[2])

        self._secondcolour = colour

        if self._usegradients:
            self.RefreshSelected()


    def GetFirstGradientColour(self):
        """Returns the first gradient colour."""
        
        return self._firstcolour


    def GetSecondGradientColour(self):
        """Returns the second gradient colour."""
        
        return self._secondcolour


    def EnableSelectionGradient(self, enable=True):
        """Globally enables/disables drawing of gradient selection."""

        self._usegradients = enable
        self._vistaselection = False
        self.RefreshSelected()
        

    def SetGradientStyle(self, vertical=0):
        """
        Sets the gradient style:
        0: horizontal gradient
        1: vertical gradient
        """

        # 0 = Horizontal, 1 = Vertical
        self._gradientstyle = vertical

        if self._usegradients:
            self.RefreshSelected()


    def GetGradientStyle(self):
        """
        Returns the gradient style:
        0: horizontal gradient
        1: vertical gradient
        """

        return self._gradientstyle


    def EnableSelectionVista(self, enable=True):
        """Globally enables/disables drawing of Windows Vista selection."""

        self._usegradients = False
        self._vistaselection = enable
        self.RefreshSelected()


    def SetBorderPen(self, pen):
        """
        Sets the pen used to draw the selected item border.
        The border pen is not used if the Windows Vista style is applied.
        """

        self._borderPen = pen
        self.RefreshSelected()


    def GetBorderPen(self):
        """
        Returns the pen used to draw the selected item border.
        The border pen is not used if the Windows Vista style is applied.
        """

        return self._borderPen


    def SetConnectionPen(self, pen):
        """Sets the pen used to draw the connecting lines between items."""

        self._dottedPen = pen
        self._dirty = True


    def GetConnectionPen(self):
        """Returns the pen used to draw the connecting lines between items."""

        return self._dottedPen


    def SetBackgroundImage(self, image):
        """Sets the CustomTreeCtrl background image (can be none)."""

        self._backgroundImage = image
        self.Refresh()
        

    def GetBackgroundImage(self):
        """Returns the CustomTreeCtrl background image (can be none)."""

        return self._backgroundImage        
    

    def GetItemWindow(self, item):
        """Returns the window associated to the item (if any)."""

        if not item:
            raise "\nERROR: Invalid Item"
        
        return item.GetWindow()


    def GetItemWindowEnabled(self, item):
        """Returns whether the window associated to the item is enabled."""

        if not item:
            raise "\nERROR: Invalid Item"
        
        return item.GetWindowEnabled()


    def SetItemWindowEnabled(self, item, enable=True):
        """Enables/disables the window associated to the item."""

        if not item:
            raise "\nERROR: Invalid Item"
        
        item.SetWindowEnabled(enable)


    def GetItemType(self, item):
        """
        Returns the item type:
        0: normal
        1: checkbox item
        2: radiobutton item
        """

        if not item:
            raise "\nERROR: Invalid Item"
        
        return item.GetType()

# -----------------------------------------------------------------------------
# item status inquiries
# -----------------------------------------------------------------------------

    def IsVisible(self, item):
        """Returns whether the item is visible or not."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        # An item is only visible if it's not a descendant of a collapsed item
        parent = item.GetParent()

        while parent:
        
            if not parent.IsExpanded():
                return False
            
            parent = parent.GetParent()
        
        startX, startY = self.GetViewStart()
        clientSize = self.GetClientSize()

        rect = self.GetBoundingRect(item)
        
        if not rect:
            return False
        if rect.GetWidth() == 0 or rect.GetHeight() == 0:
            return False
        if rect.GetBottom() < 0 or rect.GetTop() > clientSize.y:
            return False
        if rect.GetRight() < 0 or rect.GetLeft() > clientSize.x:
            return False

        return True


    def ItemHasChildren(self, item):
        """Returns whether the item has children or not."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        # consider that the item does have children if it has the "+" button: it
        # might not have them (if it had never been expanded yet) but then it
        # could have them as well and it's better to err on this side rather than
        # disabling some operations which are restricted to the items with
        # children for an item which does have them
        return item.HasPlus()


    def IsExpanded(self, item):
        """Returns whether the item is expanded or not."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.IsExpanded()


    def IsSelected(self, item):
        """Returns whether the item is selected or not."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.IsSelected()


    def IsBold(self, item):
        """Returns whether the item font is bold or not."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.IsBold()


    def IsItalic(self, item):
        """Returns whether the item font is italic or not."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.IsItalic()


# -----------------------------------------------------------------------------
# navigation
# -----------------------------------------------------------------------------

    def GetItemParent(self, item):
        """Gets the item parent."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        return item.GetParent()


    def GetFirstChild(self, item):
        """Gets the item first child."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        cookie = 0
        return self.GetNextChild(item, cookie)


    def GetNextChild(self, item, cookie):
        """
        Gets the item next child based on the 'cookie' parameter.
        This method has no sense if you do not call GetFirstChild() before.
        """

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        children = item.GetChildren()

        # it's ok to cast cookie to size_t, we never have indices big enough to
        # overflow "void *"

        if cookie < len(children):
            
            return children[cookie], cookie+1
        
        else:
        
            # there are no more of them
            return None, cookie
    

    def GetLastChild(self, item):
        """Gets the item last child."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        children = item.GetChildren()
        return (len(children) == 0 and [None] or [children[-1]])[0]


    def GetNextSibling(self, item):
        """Gets the next sibling of an item."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        i = item
        parent = i.GetParent()
        
        if parent == None:
        
            # root item doesn't have any siblings
            return None
        
        siblings = parent.GetChildren()
        index = siblings.index(i)
        
        n = index + 1
        return (n == len(siblings) and [None] or [siblings[n]])[0]


    def GetPrevSibling(self, item):
        """Gets the previous sibling of an item."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        i = item
        parent = i.GetParent()
        
        if parent == None:
        
            # root item doesn't have any siblings
            return None
        
        siblings = parent.GetChildren()
        index = siblings.index(i)

        return (index == 0 and [None] or [siblings[index-1]])[0]


    def GetNext(self, item):
        """Gets the next item. Only for internal use right now."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        i = item

        # First see if there are any children.
        children = i.GetChildren()
        if len(children) > 0:
             return children[0]
        else:
             # Try a sibling of this or ancestor instead
             p = item
             toFind = None
             while p and not toFind:
                  toFind = self.GetNextSibling(p)
                  p = self.GetItemParent(p)
                  
             return toFind
        

    def GetFirstVisibleItem(self):
        """Returns the first visible item."""

        id = self.GetRootItem()
        if not id:
            return id

        while id:
            if self.IsVisible(id):
                return id
            id = self.GetNext(id)

        return None


    def GetNextVisible(self, item):
        """Returns the next visible item."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        id = item

        while id:
            id = self.GetNext(id)
            if id and self.IsVisible(id):
                return id
            
        return None


    def GetPrevVisible(self, item):

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        raise "\nERROR: Not Implemented"

        return None


    def ResetTextControl(self):
        """Called by TreeTextCtrl when it marks itself for deletion."""

        self._textCtrl.Destroy()
        self._textCtrl = None


    def FindItem(self, idParent, prefixOrig):
        """Finds the first item starting with the given prefix after the given item."""

        # match is case insensitive as this is more convenient to the user: having
        # to press Shift-letter to go to the item starting with a capital letter
        # would be too bothersome
        prefix = prefixOrig.lower()

        # determine the starting point: we shouldn't take the current item (this
        # allows to switch between two items starting with the same letter just by
        # pressing it) but we shouldn't jump to the next one if the user is
        # continuing to type as otherwise he might easily skip the item he wanted
        id = idParent

        if len(prefix) == 1:
            id = self.GetNext(id)
        
        # look for the item starting with the given prefix after it
        while id and not self.GetItemText(id).lower().startswith(prefix):
        
            id = self.GetNext(id)
        
        # if we haven't found anything...
        if not id:
        
            # ... wrap to the beginning
            id = self.GetRootItem()
            if self.HasFlag(TR_HIDE_ROOT):
                # can't select virtual root
                id = self.GetNext(id)
            
            # and try all the items (stop when we get to the one we started from)
            while id != idParent and not self.GetItemText(id).lower().startswith(prefix):
                id = self.GetNext(id)
            
        return id


# -----------------------------------------------------------------------------
# operations
# -----------------------------------------------------------------------------

    def DoInsertItem(self, parentId, previous, text, ct_type=0, wnd=None, image=-1, selImage=-1, data=None):
        """Actually inserts an item in the tree."""

        if wnd is not None and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert Controls You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"

        if text.find("\n") >= 0 and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert A MultiLine Text You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"

        if ct_type < 0 or ct_type > 2:
            raise "\nERROR: Item Type Should Be 0 (Normal), 1 (CheckBox) or 2 (RadioButton). "
        
        parent = parentId
        
        if not parent:
        
            # should we give a warning here?
            return self.AddRoot(text, ct_type, wnd, image, selImage, data)
        
        self._dirty = True     # do this first so stuff below doesn't cause flicker

        item = GenericTreeItem(parent, text, ct_type, wnd, image, selImage, data)
        
        if wnd is not None:
            self._hasWindows = True
            self._itemWithWindow.append(item)
        
        parent.Insert(item, previous)

        return item


    def AddRoot(self, text, ct_type=0, wnd=None, image=-1, selImage=-1, data=None):
        """Adds a root to the CustomTreeCtrl. Only one root must exist."""

        if self._anchor:
            raise "\nERROR: Tree Can Have Only One Root"

        if wnd is not None and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert Controls You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"

        if text.find("\n") >= 0 and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert A MultiLine Text You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"

        if ct_type < 0 or ct_type > 2:
            raise "\nERROR: Item Type Should Be 0 (Normal), 1 (CheckBox) or 2 (RadioButton). "

        self._dirty = True     # do this first so stuff below doesn't cause flicker

        self._anchor = GenericTreeItem(None, text, ct_type, wnd, image, selImage, data)
        
        if wnd is not None:
            self._hasWindows = True
            self._itemWithWindow.append(self._anchor)            
        
        if self.HasFlag(TR_HIDE_ROOT):
        
            # if root is hidden, make sure we can navigate
            # into children
            self._anchor.SetHasPlus()
            self._anchor.Expand()
            self.CalculatePositions()
        
        if not self.HasFlag(TR_MULTIPLE):
        
            self._current = self._key_current = self._anchor
            self._current.SetHilight(True)
        
        return self._anchor


    def PrependItem(self, parent, text, ct_type=0, wnd=None, image=-1, selImage=-1, data=None):
        """Appends an item as a first child of parent."""

        if wnd is not None and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert Controls You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"

        if text.find("\n") >= 0 and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert A MultiLine Text You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"
        
        return self.DoInsertItem(parent, 0, text, ct_type, wnd, image, selImage, data)


    def InsertItemByItem(self, parentId, idPrevious, text, ct_type=0, wnd=None, image=-1, selImage=-1, data=None):
        """Auxiliary function to cope with the C++ hideous multifunction."""
        
        if wnd is not None and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert Controls You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"

        if text.find("\n") >= 0 and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert A MultiLine Text You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"
        
        parent = parentId
        
        if not parent:
            # should we give a warning here?
            return self.AddRoot(text, ct_type, wnd, image, selImage, data)
        
        index = -1
        if idPrevious:

            try:
                index = parent.GetChildren().index(idPrevious)
            except:
                raise "ERROR: Previous Item In CustomTreeCtrl.InsertItem() Is Not A Sibling"

        return self.DoInsertItem(parentId, index+1, text, ct_type, wnd, image, selImage, data)


    def InsertItemByIndex(self, parentId, before, text, ct_type=0, wnd=None, image=-1, selImage=-1, data=None):
        """Auxiliary function to cope with the C++ hideous multifunction."""

        if wnd is not None and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert Controls You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"

        if text.find("\n") >= 0 and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert A MultiLine Text You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"
        
        parent = parentId
        
        if not parent:
            # should we give a warning here?
            return self.AddRoot(text, ct_type, wnd, image, selImage, data)
        
        return self.DoInsertItem(parentId, before, text, ct_type, wnd, image, selImage, data)


    def InsertItem(self, parentId, input, text, ct_type=0, wnd=None, image=-1, selImage=-1, data=None):
        """Inserts an item after the given previous."""

        if wnd is not None and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert Controls You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"

        if text.find("\n") >= 0 and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert A MultiLine Text You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"
        
        if type(input) == type(1):
            return self.InsertItemByIndex(parentId, input, text, ct_type, wnd, image, selImage, data)
        else:
            return self.InsertItemByItem(parentId, input, text, ct_type, wnd, image, selImage, data)
            

    def AppendItem(self, parentId, text, ct_type=0, wnd=None, image=-1, selImage=-1, data=None):
        """Appends an item as a last child of its parent."""

        if wnd is not None and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert Controls You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"

        if text.find("\n") >= 0 and not (self._windowStyle & TR_HAS_VARIABLE_ROW_HEIGHT):
            raise "\nERROR: In Order To Append/Insert A MultiLine Text You Have To Use The Style TR_HAS_VARIABLE_ROW_HEIGHT"
        
        parent = parentId
        
        if not parent:
            # should we give a warning here?
            return self.AddRoot(text, ct_type, wnd, image, selImage, data)
        
        return self.DoInsertItem(parent, len(parent.GetChildren()), text, ct_type, wnd, image, selImage, data)


    def SendDeleteEvent(self, item):
        """Actully sends the EVT_TREE_DELETE_ITEM event."""

        event = TreeEvent(wxEVT_TREE_DELETE_ITEM, self.GetId())
        event._item = item
        event.SetEventObject(self)
        self.ProcessEvent(event)


    def IsDescendantOf(self, parent, item):
        """Checks if the given item is under another one."""

        while item:
        
            if item == parent:
            
                # item is a descendant of parent
                return True
            
            item = item.GetParent()
        
        return False


    # Don't leave edit or selection on a child which is about to disappear
    def ChildrenClosing(self, item):
        """We are about to destroy the item children."""

        if self._textCtrl != None and item != self._textCtrl.item() and self.IsDescendantOf(item, self._textCtrl.item()):
            self._textCtrl.StopEditing()
        
        if item != self._key_current and self.IsDescendantOf(item, self._key_current):
            self._key_current = None
        
        if self.IsDescendantOf(item, self._select_me):
            self._select_me = item
        
        if item != self._current and self.IsDescendantOf(item, self._current):
            self._current.SetHilight(False)
            self._current = None
            self._select_me = item


    def DeleteChildren(self, item):
        """Delete item children."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        self._dirty = True     # do this first so stuff below doesn't cause flicker

        self.ChildrenClosing(item)
        item.DeleteChildren(self)


    def Delete(self, item):
        """Delete an item."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        self._dirty = True     # do this first so stuff below doesn't cause flicker

        if self._textCtrl != None and self.IsDescendantOf(item, self._textCtrl.item()):
            # can't delete the item being edited, cancel editing it first
            self._textCtrl.StopEditing()
        
        parent = item.GetParent()

        # don't keep stale pointers around!
        if self.IsDescendantOf(item, self._key_current):
        
            # Don't silently change the selection:
            # do it properly in idle time, so event
            # handlers get called.

            # self._key_current = parent
            self._key_current = None
        
        # self._select_me records whether we need to select
        # a different item, in idle time.
        if self._select_me and self.IsDescendantOf(item, self._select_me):
            self._select_me = parent
        
        if self.IsDescendantOf(item, self._current):
        
            # Don't silently change the selection:
            # do it properly in idle time, so event
            # handlers get called.

            # self._current = parent
            self._current = None
            self._select_me = parent
        
        # remove the item from the tree
        if parent:
        
            parent.GetChildren().remove(item)  # remove by value
        
        else: # deleting the root
        
            # nothing will be left in the tree
            self._anchor = None
        
        # and delete all of its children and the item itself now
        item.DeleteChildren(self)
        self.SendDeleteEvent(item)

        if item == self._select_me:
            self._select_me = None

        # Remove the item with window
        if item in self._itemWithWindow:
            wnd = item.GetWindow()
            wnd.Hide()
            wnd.Destroy()
            item._wnd = None
            self._itemWithWindow.remove(item)
            
        del item


    def DeleteAllItems(self):
        """Delete all items in the CustomTreeCtrl."""

        if self._anchor:
            self.Delete(self._anchor)
        

    def Expand(self, item):
        """
        Expands an item, sending a EVT_TREE_ITEM_EXPANDING and
        EVT_TREE_ITEM_EXPANDED events.
        """

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        if self.HasFlag(TR_HIDE_ROOT) and item == self.GetRootItem():
             raise "\nERROR: Can't Expand An Hidden Root. "

        if not item.HasPlus():
            return

        if item.IsExpanded():
            return

        event = TreeEvent(wxEVT_TREE_ITEM_EXPANDING, self.GetId())
        event._item = item
        event.SetEventObject(self)

        if self.ProcessEvent(event) and not event.IsAllowed():
            # cancelled by program
            return
    
        item.Expand()
        self.CalculatePositions()

        self.RefreshSubtree(item)

        if self._hasWindows:
            # We hide the associated window here, we may show it after
            self.HideWindows()
            
        event.SetEventType(wxEVT_TREE_ITEM_EXPANDED)
        self.ProcessEvent(event)


    def ExpandAll(self, item):
        """Expands all the items."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        if not self.HasFlag(TR_HIDE_ROOT) or item != GetRootItem():
            self.Expand(item)
            if not self.IsExpanded(item):
                return
        
        child, cookie = self.GetFirstChild(item)
        
        while child:
            self.ExpandAll(child)
            child, cookie = self.GetNextChild(item, cookie)
        

    def Collapse(self, item):
        """
        Collapse an item, sending a EVT_TREE_ITEM_COLLAPSING and
        EVT_TREE_ITEM_COLLAPSED events.
        """

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        if self.HasFlag(TR_HIDE_ROOT) and item == self.GetRootItem():
             raise "\nERROR: Can't Collapse An Hidden Root. "

        if not item.IsExpanded():
            return

        event = TreeEvent(wxEVT_TREE_ITEM_COLLAPSING, self.GetId())
        event._item = item
        event.SetEventObject(self)
        if self.ProcessEvent(event) and not event.IsAllowed():
            # cancelled by program
            return
    
        self.ChildrenClosing(item)
        item.Collapse()

        self.CalculatePositions()
        self.RefreshSubtree(item)

        if self._hasWindows:
            self.HideWindows()
            
        event.SetEventType(wxEVT_TREE_ITEM_COLLAPSED)
        self.ProcessEvent(event)


    def CollapseAndReset(self, item):
        """Collapse the given item and deletes its children."""

        self.Collapse(item)
        self.DeleteChildren(item)


    def Toggle(self, item):
        """Toggles the item state (collapsed/expanded)."""

        if item.IsExpanded():
            self.Collapse(item)
        else:
            self.Expand(item)


    def HideWindows(self):
        """Hides the windows associated to the items. Used internally."""
        
        for child in self._itemWithWindow:
            if not self.IsVisible(child):
                wnd = child.GetWindow()
                wnd.Hide()
            

    def Unselect(self):
        """Unselects the current selection."""

        if self._current:
        
            self._current.SetHilight(False)
            self.RefreshLine(self._current)

        self._current = None
        self._select_me = None
    

    def UnselectAllChildren(self, item):
        """Unselects all the children of the given item."""

        if item.IsSelected():
        
            item.SetHilight(False)
            self.RefreshLine(item)
        
        if item.HasChildren():
            for child in item.GetChildren():
                self.UnselectAllChildren(child)
            

    def UnselectAll(self):
        """Unselect all the items."""

        rootItem = self.GetRootItem()

        # the tree might not have the root item at all
        if rootItem:
            self.UnselectAllChildren(rootItem)
        

    # Recursive function !
    # To stop we must have crt_item<last_item
    # Algorithm :
    # Tag all next children, when no more children,
    # Move to parent (not to tag)
    # Keep going... if we found last_item, we stop.

    def TagNextChildren(self, crt_item, last_item, select):
        """Used internally."""

        parent = crt_item.GetParent()

        if parent == None: # This is root item
            return self.TagAllChildrenUntilLast(crt_item, last_item, select)

        children = parent.GetChildren()
        index = children.index(crt_item)
        
        count = len(children)
        
        for n in xrange(index+1, count):
            if self.TagAllChildrenUntilLast(children[n], last_item, select):
                return True

        return self.TagNextChildren(parent, last_item, select)


    def TagAllChildrenUntilLast(self, crt_item, last_item, select):
        """Used internally."""

        crt_item.SetHilight(select)
        self.RefreshLine(crt_item)

        if crt_item == last_item:
            return True

        if crt_item.HasChildren():        
            for child in crt_item.GetChildren():
                if self.TagAllChildrenUntilLast(child, last_item, select):
                    return True
            
        return False


    def SelectItemRange(self, item1, item2):
        """Selects all the items between item1 and item2."""
        
        self._select_me = None

        # item2 is not necessary after item1
        # choice first' and 'last' between item1 and item2
        first = (item1.GetY() < item2.GetY() and [item1] or [item2])[0]
        last = (item1.GetY() < item2.GetY() and [item2] or [item1])[0]

        select = self._current.IsSelected()

        if self.TagAllChildrenUntilLast(first, last, select):
            return

        self.TagNextChildren(first, last, select)


    def DoSelectItem(self, item, unselect_others=True, extended_select=False):
        """Actually selects/unselects an item, sending a EVT_TREE_SEL_CHANGED event."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        self._select_me = None

        is_single = not (self.GetTreeStyle() & TR_MULTIPLE)

        # to keep going anyhow !!!
        if is_single:
            if item.IsSelected():
                return # nothing to do
            unselect_others = True
            extended_select = False
        
        elif unselect_others and item.IsSelected():
        
            # selection change if there is more than one item currently selected
            if len(self.GetSelections()) == 1:
                return

        event = TreeEvent(wxEVT_TREE_SEL_CHANGING, self.GetId())
        event._item = item
        event._itemOld = self._current
        event.SetEventObject(self)
        # TODO : Here we don't send any selection mode yet !

        if self.GetEventHandler().ProcessEvent(event) and not event.IsAllowed():
            return

        parent = self.GetItemParent(item)
        while parent:
            if not self.IsExpanded(parent):
                self.Expand(parent)

            parent = self.GetItemParent(parent)
        
        # ctrl press
        if unselect_others:
            if is_single:
                self.Unselect() # to speed up thing
            else:
                self.UnselectAll()

        # shift press
        if extended_select:
            if not self._current:
                self._current = self._key_current = self.GetRootItem()
            
            # don't change the mark (self._current)
            self.SelectItemRange(self._current, item)
        
        else:
        
            select = True # the default

            # Check if we need to toggle hilight (ctrl mode)
            if not unselect_others:
                select = not item.IsSelected()

            self._current = self._key_current = item
            self._current.SetHilight(select)
            self.RefreshLine(self._current)
        
        # This can cause idle processing to select the root
        # if no item is selected, so it must be after the
        # selection is set
        self.EnsureVisible(item)

        event.SetEventType(wxEVT_TREE_SEL_CHANGED)
        self.GetEventHandler().ProcessEvent(event)

        # Handles hypertext items
        if self.IsItemHyperText(item):
            event = TreeEvent(wxEVT_TREE_ITEM_HYPERLINK, self.GetId())
            event._item = item
            self.GetEventHandler().ProcessEvent(event)


    def SelectItem(self, item, select=True):
        """Selects/deselects an item."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
        
        if select:
        
            self.DoSelectItem(item, not self.HasFlag(TR_MULTIPLE))
        
        else: # deselect
        
            item.SetHilight(False)
            self.RefreshLine(item)

    
    def FillArray(self, item, array=[]):
        """
        Internal function. Used to populate an array of selected items when
        the style TR_MULTIPLE is used.
        """

        if not array:
            array = []
            
        if item.IsSelected():
            array.append(item)

        if item.HasChildren():    
            for child in item.GetChildren():
                array = self.FillArray(child, array)
        
        return array
    

    def GetSelections(self):
        """
        Returns a list of selected items. This can be used only if CustomTreeCtrl has
        the TR_MULTIPLE style set.
        """

        array = []
        idRoot = self.GetRootItem()
        if idRoot:
            array = self.FillArray(idRoot, array)
        
        #else: the tree is empty, so no selections

        return array


    def EnsureVisible(self, item):
        """Ensure that an item is visible in CustomTreeCtrl."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        # first expand all parent branches
        parent = item.GetParent()

        if self.HasFlag(TR_HIDE_ROOT):
            while parent and parent != self._anchor:
                self.Expand(parent)
                parent = parent.GetParent()
        else:
            while parent:
                self.Expand(parent)
                parent = parent.GetParent()
            
        self.ScrollTo(item)


    def ScrollTo(self, item):
        """Scrolls the specified item into view."""

        if not item:
            return

        # We have to call this here because the label in
        # question might just have been added and no screen
        # update taken place.
        if self._dirty:
            if wx.Platform in ["__WXMSW__", "__WXMAC__"]:
                self.Update()
        else:
            wx.YieldIfNeeded()

        # now scroll to the item
        item_y = item.GetY()
        start_x, start_y = self.GetViewStart()
        start_y *= _PIXELS_PER_UNIT

        client_w, client_h = self.GetClientSize()

        x, y = 0, 0

        if item_y < start_y+3:
        
            # going down
            x, y = self._anchor.GetSize(x, y, self)
            y += _PIXELS_PER_UNIT + 2 # one more scrollbar unit + 2 pixels
            x += _PIXELS_PER_UNIT + 2 # one more scrollbar unit + 2 pixels
            x_pos = self.GetScrollPos(wx.HORIZONTAL)
            # Item should appear at top
            self.SetScrollbars(_PIXELS_PER_UNIT, _PIXELS_PER_UNIT, x/_PIXELS_PER_UNIT, y/_PIXELS_PER_UNIT, x_pos, item_y/_PIXELS_PER_UNIT)
        
        elif item_y+self.GetLineHeight(item) > start_y+client_h:
        
            # going up
            x, y = self._anchor.GetSize(x, y, self)
            y += _PIXELS_PER_UNIT + 2 # one more scrollbar unit + 2 pixels
            x += _PIXELS_PER_UNIT + 2 # one more scrollbar unit + 2 pixels
            item_y += _PIXELS_PER_UNIT+2
            x_pos = self.GetScrollPos(wx.HORIZONTAL)
            # Item should appear at bottom
            self.SetScrollbars(_PIXELS_PER_UNIT, _PIXELS_PER_UNIT, x/_PIXELS_PER_UNIT, y/_PIXELS_PER_UNIT, x_pos, (item_y+self.GetLineHeight(item)-client_h)/_PIXELS_PER_UNIT )


    def OnCompareItems(self, item1, item2):
        """
        Returns whether 2 items have the same text.
        Override this function in the derived class to change the sort order of the items
        in the CustomTreeCtrl. The function should return a negative, zero or positive
        value if the first item is less than, equal to or greater than the second one.

        The base class version compares items alphabetically.
        """

        return self.GetItemText(item1) == self.GetItemText(item2)


    def SortChildren(self, item):
        """
        Sorts the children of the given item using OnCompareItems method of CustomTreeCtrl. 
        You should override that method to change the sort order (the default is ascending
        case-sensitive alphabetical order).
        """

        if not item:
            raise "\nERROR: Invalid Tree Item. "

        children = item.GetChildren()
        
        if len(children) > 1:
            self._dirty = True
            children.sort(self.OnCompareItems)
        

    def GetImageList(self):
        """Returns the normal image list."""

        return self._imageListNormal


    def GetButtonsImageList(self):
        """Returns the buttons image list (from which application-defined button images are taken)."""

        return self._imageListButtons


    def GetStateImageList(self):
        """Returns the state image list (from which application-defined state images are taken)."""

        return self._imageListState


    def GetImageListCheck(self):
        """Returns the image list used to build the check/radio buttons."""

        return self._imageListCheck        


    def CalculateLineHeight(self):
        """Calculates the height of a line."""

        dc = wx.ClientDC(self)
        self._lineHeight = dc.GetCharHeight() 

        if self._imageListNormal:
        
            # Calculate a self._lineHeight value from the normal Image sizes.
            # May be toggle off. Then CustomTreeCtrl will spread when
            # necessary (which might look ugly).
            n = self._imageListNormal.GetImageCount()

            for i in xrange(n):
            
                width, height = self._imageListNormal.GetSize(i)

                if height > self._lineHeight:
                    self._lineHeight = height
            
        if self._imageListButtons:
        
            # Calculate a self._lineHeight value from the Button image sizes.
            # May be toggle off. Then CustomTreeCtrl will spread when
            # necessary (which might look ugly).
            n = self._imageListButtons.GetImageCount()

            for i in xrange(n):
            
                width, height = self._imageListButtons.GetSize(i)

                if height > self._lineHeight:
                    self._lineHeight = height

        if self._imageListCheck:
        
            # Calculate a self._lineHeight value from the check/radio image sizes.
            # May be toggle off. Then CustomTreeCtrl will spread when
            # necessary (which might look ugly).
            n = self._imageListCheck.GetImageCount()

            for i in xrange(n):
            
                width, height = self._imageListCheck.GetSize(i)

                if height > self._lineHeight:
                    self._lineHeight = height
        
        if self._lineHeight < 30:
            self._lineHeight += 2                 # at least 2 pixels
        else:
            self._lineHeight += self._lineHeight/10   # otherwise 10% extra spacing


    def SetImageList(self, imageList):
        """Sets the normal image list."""

        if self._ownsImageListNormal:
            del self._imageListNormal
            
        self._imageListNormal = imageList
        self._ownsImageListNormal = False
        self._dirty = True
        # Don't do any drawing if we're setting the list to NULL,
        # since we may be in the process of deleting the tree control.
        if imageList:
            self.CalculateLineHeight()

        # We gray out the image list to use the grayed icons with disabled items
        self._grayedImageList = wx.ImageList(16, 16, True, 0)
        
        for ii in xrange(imageList.GetImageCount()):
            
            bmp = imageList.GetBitmap(ii)
            image = wx.ImageFromBitmap(bmp)
            image = GrayOut(image)
            newbmp = wx.BitmapFromImage(image)
            self._grayedImageList.Add(newbmp)
        

    def SetStateImageList(self, imageList):
        """Sets the state image list (from which application-defined state images are taken)."""
        
        if self._ownsImageListState:
            del self._imageListState

        self._imageListState = imageList
        self._ownsImageListState = False


    def SetButtonsImageList(self, imageList):
        """Sets the buttons image list (from which application-defined button images are taken)."""

        if self._ownsImageListButtons:
            del self._imageListButtons
            
        self._imageListButtons = imageList
        self._ownsImageListButtons = False
        self._dirty = True
        self.CalculateLineHeight()


    def SetImageListCheck(self, sizex, sizey, imglist=None):
        """Sets the check image list."""

        if imglist is None:
            
            self._imageListCheck = wx.ImageList(sizex, sizey)
            self._imageListCheck.Add(GetCheckedBitmap())
            self._imageListCheck.Add(GetNotCheckedBitmap())
            self._imageListCheck.Add(GetFlaggedBitmap())
            self._imageListCheck.Add(GetNotFlaggedBitmap())

        else:

            sizex, sizey = imglist.GetSize(0)
            self._imageListCheck = imglist

        # We gray out the image list to use the grayed icons with disabled items
        self._grayedCheckList = wx.ImageList(sizex, sizey, True, 0)
        
        for ii in xrange(self._imageListCheck.GetImageCount()):
            
            bmp = self._imageListCheck.GetBitmap(ii)
            image = wx.ImageFromBitmap(bmp)
            image = GrayOut(image)
            newbmp = wx.BitmapFromImage(image)
            self._grayedCheckList.Add(newbmp)

        self._dirty = True

        if imglist:
            self.CalculateLineHeight()


    def AssignImageList(self, imageList):
        """Assigns the normal image list."""

        self.SetImageList(imageList)
        self._ownsImageListNormal = True


    def AssignStateImageList(self, imageList):
        """Assigns the state image list."""

        self.SetStateImageList(imageList)
        self._ownsImageListState = True


    def AssignButtonsImageList(self, imageList):
        """Assigns the button image list."""

        self.SetButtonsImageList(imageList)
        self._ownsImageListButtons = True


# -----------------------------------------------------------------------------
# helpers
# -----------------------------------------------------------------------------

    def AdjustMyScrollbars(self):
        """Adjust the wx.ScrolledWindow scrollbars."""

        if self._anchor:
        
            x, y = self._anchor.GetSize(0, 0, self)
            y += _PIXELS_PER_UNIT + 2 # one more scrollbar unit + 2 pixels
            x += _PIXELS_PER_UNIT + 2 # one more scrollbar unit + 2 pixels
            x_pos = self.GetScrollPos(wx.HORIZONTAL)
            y_pos = self.GetScrollPos(wx.VERTICAL)
            self.SetScrollbars(_PIXELS_PER_UNIT, _PIXELS_PER_UNIT, x/_PIXELS_PER_UNIT, y/_PIXELS_PER_UNIT, x_pos, y_pos)
        
        else:
        
            self.SetScrollbars(0, 0, 0, 0)
    

    def GetLineHeight(self, item):
        """Returns the line height for the given item."""

        if self.GetTreeStyle() & TR_HAS_VARIABLE_ROW_HEIGHT:
            return item.GetHeight()
        else:
            return self._lineHeight


    def DrawVerticalGradient(self, dc, rect, hasfocus):
        """Gradient fill from colour 1 to colour 2 from top to bottom."""

        dc.DrawRectangleRect(rect)
        border = self._borderPen.GetWidth()
        
        dc.SetPen(wx.TRANSPARENT_PEN)

        # calculate gradient coefficients
        if hasfocus:
            col2 = self._secondcolour
            col1 = self._firstcolour
        else:
            col2 = self._hilightUnfocusedBrush.GetColour()
            col1 = self._hilightUnfocusedBrush2.GetColour()

        r1, g1, b1 = int(col1.Red()), int(col1.Green()), int(col1.Blue())
        r2, g2, b2 = int(col2.Red()), int(col2.Green()), int(col2.Blue())

        flrect = float(rect.height)

        rstep = float((r2 - r1)) / flrect
        gstep = float((g2 - g1)) / flrect
        bstep = float((b2 - b1)) / flrect

        rf, gf, bf = 0, 0, 0
        
        for y in xrange(rect.y+border, rect.y + rect.height-border):
            currCol = (r1 + rf, g1 + gf, b1 + bf)                
            dc.SetBrush(wx.Brush(currCol, wx.SOLID))
            dc.DrawRectangle(rect.x+border, y, rect.width-2*border, 1)
            rf = rf + rstep
            gf = gf + gstep
            bf = bf + bstep
        

    def DrawHorizontalGradient(self, dc, rect, hasfocus):
        """Gradient fill from colour 1 to colour 2 from left to right."""

        dc.DrawRectangleRect(rect)
        border = self._borderPen.GetWidth()
        
        dc.SetPen(wx.TRANSPARENT_PEN)

        # calculate gradient coefficients

        if hasfocus:
            col2 = self._secondcolour
            col1 = self._firstcolour
        else:
            col2 = self._hilightUnfocusedBrush.GetColour()
            col1 = self._hilightUnfocusedBrush2.GetColour()

        r1, g1, b1 = int(col1.Red()), int(col1.Green()), int(col1.Blue())
        r2, g2, b2 = int(col2.Red()), int(col2.Green()), int(col2.Blue())

        flrect = float(rect.width)

        rstep = float((r2 - r1)) / flrect
        gstep = float((g2 - g1)) / flrect
        bstep = float((b2 - b1)) / flrect

        rf, gf, bf = 0, 0, 0
        
        for x in xrange(rect.x+border, rect.x + rect.width-border):
            currCol = (int(r1 + rf), int(g1 + gf), int(b1 + bf))
            dc.SetBrush(wx.Brush(currCol, wx.SOLID))
            dc.DrawRectangle(x, rect.y+border, 1, rect.height-2*border)
            rf = rf + rstep
            gf = gf + gstep
            bf = bf + bstep
            

    def DrawVistaRectangle(self, dc, rect, hasfocus):
        """Draw the selected item(s) with the Windows Vista style."""

        if hasfocus:
            
            outer = _rgbSelectOuter
            inner = _rgbSelectInner
            top = _rgbSelectTop
            bottom = _rgbSelectBottom

        else:
            
            outer = _rgbNoFocusOuter
            inner = _rgbNoFocusInner
            top = _rgbNoFocusTop
            bottom = _rgbNoFocusBottom

        oldpen = dc.GetPen()
        oldbrush = dc.GetBrush()

        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.SetPen(wx.Pen(outer))
        dc.DrawRoundedRectangleRect(rect, 3)
        rect.Deflate(1, 1)
        dc.SetPen(wx.Pen(inner))
        dc.DrawRoundedRectangleRect(rect, 2)
        rect.Deflate(1, 1)

        r1, g1, b1 = int(top.Red()), int(top.Green()), int(top.Blue())
        r2, g2, b2 = int(bottom.Red()), int(bottom.Green()), int(bottom.Blue())

        flrect = float(rect.height)

        rstep = float((r2 - r1)) / flrect
        gstep = float((g2 - g1)) / flrect
        bstep = float((b2 - b1)) / flrect

        rf, gf, bf = 0, 0, 0
        dc.SetPen(wx.TRANSPARENT_PEN)
        
        for y in xrange(rect.y, rect.y + rect.height):
            currCol = (r1 + rf, g1 + gf, b1 + bf)
            dc.SetBrush(wx.Brush(currCol, wx.SOLID))
            dc.DrawRectangle(rect.x, y, rect.width, 1)
            rf = rf + rstep
            gf = gf + gstep
            bf = bf + bstep
        
        dc.SetPen(oldpen)
        dc.SetBrush(oldbrush)


    def PaintItem(self, item, dc):
        """Actually paint an item."""

        attr = item.GetAttributes()
        
        if attr and attr.HasFont():
            dc.SetFont(attr.GetFont())
        elif item.IsBold():
            dc.SetFont(self._boldFont)
        if item.IsHyperText():
            dc.SetFont(self.GetHyperTextFont())
            if item.GetVisited():
                dc.SetTextForeground(self.GetHyperTextVisitedColour())
            else:
                dc.SetTextForeground(self.GetHyperTextNewColour())

        text_w, text_h, dummy = dc.GetMultiLineTextExtent(item.GetText())

        image = item.GetCurrentImage()
        checkimage = item.GetCurrentCheckedImage()
        image_w, image_h = 0, 0

        if image != _NO_IMAGE:
        
            if self._imageListNormal:
            
                image_w, image_h = self._imageListNormal.GetSize(image)
                image_w += 4
            
            else:
            
                image = _NO_IMAGE

        if item.GetType() != 0:
            wcheck, hcheck = self._imageListCheck.GetSize(item.GetType())
            wcheck += 4
        else:
            wcheck, hcheck = 0, 0

        total_h = self.GetLineHeight(item)
        drawItemBackground = False

        if item.IsSelected():
        
            # under mac selections are only a rectangle in case they don't have the focus
            if wx.Platform == "__WXMAC__":
                if not self._hasFocus:
                    dc.SetBrush(wx.TRANSPARENT_BRUSH) 
                    dc.SetPen(wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHT), 1, wx.SOLID)) 
                else:
                    dc.SetBrush(self._hilightBrush) 
            else:
                    dc.SetBrush((self._hasFocus and [self._hilightBrush] or [self._hilightUnfocusedBrush])[0])
                    drawItemBackground = True
        else:
            if attr and attr.HasBackgroundColour():
                drawItemBackground = True
                colBg = attr.GetBackgroundColour()
            else:
                colBg = self._backgroundColour
            
            dc.SetBrush(wx.Brush(colBg, wx.SOLID))
            dc.SetPen(wx.TRANSPARENT_PEN)
        
        offset = (self.HasFlag(TR_ROW_LINES) and [1] or [0])[0]

        if self.HasFlag(TR_FULL_ROW_HIGHLIGHT):

            oldpen = dc.GetPen()
            dc.SetPen(wx.TRANSPARENT_PEN)
            x = 0
            w, h = self.GetSize()

            itemrect = wx.Rect(x, item.GetY()+offset, w, total_h-offset)
            
            if item.IsSelected():
                if self._usegradients:
                    if self._gradientstyle == 0:   # Horizontal
                        self.DrawHorizontalGradient(dc, itemrect, self._hasFocus)
                    else:                          # Vertical
                        self.DrawVerticalGradient(dc, itemrect, self._hasFocus)
                elif self._vistaselection:
                    self.DrawVistaRectangle(dc, itemrect, self._hasFocus)
                else:
                    dc.DrawRectangleRect(itemrect)

            dc.SetPen(oldpen)
        
        else:
        
            if item.IsSelected() and image != _NO_IMAGE:
            
                # If it's selected, and there's an image, then we should
                # take care to leave the area under the image painted in the
                # background colour.

                wnd = item.GetWindow()
                wndx = 0
                if wnd:
                    wndx, wndy = item.GetWindowSize()

                itemrect = wx.Rect(item.GetX() + wcheck + image_w - 2, item.GetY()+offset,
                                   item.GetWidth() - image_w - wcheck + 2 - wndx, total_h-offset)
                
                if self._usegradients:
                    if self._gradientstyle == 0:   # Horizontal
                        self.DrawHorizontalGradient(dc, itemrect, self._hasFocus)
                    else:                          # Vertical
                        self.DrawVerticalGradient(dc, itemrect, self._hasFocus)
                elif self._vistaselection:
                    self.DrawVistaRectangle(dc, itemrect, self._hasFocus)
                else:
                    dc.DrawRectangleRect(itemrect)
                            
            # On GTK+ 2, drawing a 'normal' background is wrong for themes that
            # don't allow backgrounds to be customized. Not drawing the background,
            # except for custom item backgrounds, works for both kinds of theme.
            elif drawItemBackground:

                minusicon = wcheck + image_w - 2
                itemrect = wx.Rect(item.GetX()+minusicon, item.GetY()+offset, item.GetWidth()-minusicon, total_h-offset)
                                
                if self._usegradients and self._hasFocus:
                    if self._gradientstyle == 0:   # Horizontal
                        self.DrawHorizontalGradient(dc, itemrect, self._hasFocus)
                    else:                          # Vertical
                        self.DrawVerticalGradient(dc, itemrect, self._hasFocus)
                else:
                    dc.DrawRectangleRect(itemrect)
                        
        if image != _NO_IMAGE:
        
            dc.SetClippingRegion(item.GetX(), item.GetY(), wcheck+image_w-2, total_h)
            if item.IsEnabled():
                imglist = self._imageListNormal
            else:
                imglist = self._grayedImageList

            imglist.Draw(image, dc,
                         item.GetX() + wcheck,
                         item.GetY() + ((total_h > image_h) and [(total_h-image_h)/2] or [0])[0],
                         wx.IMAGELIST_DRAW_TRANSPARENT)
            
            dc.DestroyClippingRegion()

        if wcheck:
            if item.IsEnabled():
                imglist = self._imageListCheck
            else:
                imglist = self._grayedCheckList
                
            imglist.Draw(checkimage, dc,
                         item.GetX(),
                         item.GetY() + ((total_h > hcheck) and [(total_h-hcheck)/2] or [0])[0],
                         wx.IMAGELIST_DRAW_TRANSPARENT)

        dc.SetBackgroundMode(wx.TRANSPARENT)
        extraH = ((total_h > text_h) and [(total_h - text_h)/2] or [0])[0]

        textrect = wx.Rect(wcheck + image_w + item.GetX(), item.GetY() + extraH, text_w, text_h)
        
        if not item.IsEnabled():
            foreground = dc.GetTextForeground()
            dc.SetTextForeground(self._disabledColour)
            dc.DrawLabel(item.GetText(), textrect)
            dc.SetTextForeground(foreground)
        else:
            dc.DrawLabel(item.GetText(), textrect)

        wnd = item.GetWindow()
        if wnd:
            wndx = wcheck + image_w + item.GetX() + text_w + 4
            xa, ya = self.CalcScrolledPosition((0, item.GetY()))
            if not wnd.IsShown():
                wnd.Show()
            if wnd.GetPosition() != (wndx, ya):
                wnd.SetPosition((wndx, ya))

        # restore normal font
        dc.SetFont(self._normalFont)
        

    # Now y stands for the top of the item, whereas it used to stand for middle !
    def PaintLevel(self, item, dc, level, y):
        """Paint a level of CustomTreeCtrl."""

        x = level*self._indent
        
        if not self.HasFlag(TR_HIDE_ROOT):
        
            x += self._indent
        
        elif level == 0:
        
            # always expand hidden root
            origY = y
            children = item.GetChildren()
            count = len(children)
            
            if count > 0:
                n = 0
                while n < count:
                    oldY = y
                    y = self.PaintLevel(children[n], dc, 1, y)
                    n = n + 1

                if not self.HasFlag(TR_NO_LINES) and self.HasFlag(TR_LINES_AT_ROOT) and count > 0:
                
                    # draw line down to last child
                    origY += self.GetLineHeight(children[0])>>1
                    oldY += self.GetLineHeight(children[n-1])>>1
                    dc.DrawLine(3, origY, 3, oldY)
                
            return y
        
        item.SetX(x+self._spacing)
        item.SetY(y)

        h = self.GetLineHeight(item)
        y_top = y
        y_mid = y_top + (h>>1)
        y += h

        exposed_x = dc.LogicalToDeviceX(0)
        exposed_y = dc.LogicalToDeviceY(y_top)

        if self.IsExposed(exposed_x, exposed_y, 10000, h):  # 10000 = very much
            if wx.Platform == "__WXMAC__":
                # don't draw rect outline if we already have the
                # background color under Mac
                pen = ((item.IsSelected() and self._hasFocus) and [self._borderPen] or [wx.TRANSPARENT_PEN])[0]
            else:
                pen = self._borderPen

            if item.IsSelected():
                if (wx.Platform == "__WXMAC__" and self._hasFocus):
                    colText = wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHTTEXT)
                else:
                    colText = wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHTTEXT)
            else:
                attr = item.GetAttributes()
                if attr and attr.HasTextColour():
                    colText = attr.GetTextColour()
                else:
                    colText = self.GetForegroundColour()

            if self._vistaselection:
                colText = wx.BLACK
            
            # prepare to draw
            dc.SetTextForeground(colText)
            dc.SetPen(pen)
            oldpen = pen

            # draw
            self.PaintItem(item, dc)

            if self.HasFlag(TR_ROW_LINES):
            
                # if the background colour is white, choose a
                # contrasting color for the lines
                medium_grey = wx.Pen(wx.Colour(200, 200, 200))
                dc.SetPen(((self.GetBackgroundColour() == wx.WHITE) and [medium_grey] or [wx.WHITE_PEN])[0])
                dc.DrawLine(0, y_top, 10000, y_top)
                dc.DrawLine(0, y, 10000, y)
            
            # restore DC objects
            dc.SetBrush(wx.WHITE_BRUSH)
            dc.SetTextForeground(wx.BLACK)

            if not self.HasFlag(TR_NO_LINES):
            
                # draw the horizontal line here
                dc.SetPen(self._dottedPen)
                x_start = x
                if x > self._indent:
                    x_start -= self._indent
                elif self.HasFlag(TR_LINES_AT_ROOT):
                    x_start = 3
                dc.DrawLine(x_start, y_mid, x + self._spacing, y_mid)
                dc.SetPen(oldpen)            

            # should the item show a button?
            if item.HasPlus() and self.HasButtons():
            
                if self._imageListButtons:
                
                    # draw the image button here
                    image_h = 0
                    image_w = 0
                    image = (item.IsExpanded() and [TreeItemIcon_Expanded] or [TreeItemIcon_Normal])[0]
                    if item.IsSelected():
                        image += TreeItemIcon_Selected - TreeItemIcon_Normal

                    image_w, image_h = self._imageListButtons.GetSize(image)
                    xx = x - image_w/2
                    yy = y_mid - image_h/2

                    dc.SetClippingRegion(xx, yy, image_w, image_h)
                    self._imageListButtons.Draw(image, dc, xx, yy,
                                                wx.IMAGELIST_DRAW_TRANSPARENT)
                    dc.DestroyClippingRegion()
                    
                else: # no custom buttons

                    if self._windowStyle & TR_TWIST_BUTTONS:
                        # We draw something like the Mac twist buttons
                        
                        dc.SetPen(wx.BLACK_PEN)
                        dc.SetBrush(self._hilightBrush)
                        button = [wx.Point(), wx.Point(), wx.Point()]
                        
                        if item.IsExpanded():
                            button[0].x = x - 5
                            button[0].y = y_mid - 3
                            button[1].x = x + 5
                            button[1].y = button[0].y
                            button[2].x = x
                            button[2].y = button[0].y + 6
                        else:
                            button[0].x = x - 3
                            button[0].y = y_mid - 5
                            button[1].x = button[0].x
                            button[1].y = y_mid + 5
                            button[2].x = button[0].x + 5
                            button[2].y = y_mid
                        
                        dc.DrawPolygon(button)

                    else:
                        # These are the standard wx.TreeCtrl buttons as wx.RendererNative knows
                        
                        wImage = 9
                        hImage = 9

                        flag = 0

                        if item.IsExpanded():
                            flag |= _CONTROL_EXPANDED
                        if item == self._underMouse:
                            flag |= _CONTROL_CURRENT

                        self._drawingfunction(self, dc, wx.Rect(x - wImage/2, y_mid - hImage/2,wImage, hImage), flag)
                
        if item.IsExpanded():
        
            children = item.GetChildren()
            count = len(children)
            
            if count > 0:
            
                n = 0
                level = level + 1

                while n < count:
                    oldY = y
                    y = self.PaintLevel(children[n], dc, level, y)
                    n = n + 1
                    
                if not self.HasFlag(TR_NO_LINES) and count > 0:
                
                    # draw line down to last child
                    oldY += self.GetLineHeight(children[n-1])>>1
                    if self.HasButtons():
                        y_mid += 5

                    # Only draw the portion of the line that is visible, in case it is huge
                    xOrigin, yOrigin = dc.GetDeviceOrigin()
                    yOrigin = abs(yOrigin)
                    width, height = self.GetClientSize()

                    # Move end points to the begining/end of the view?
                    if y_mid < yOrigin:
                        y_mid = yOrigin
                    if oldY > yOrigin + height:
                        oldY = yOrigin + height

                    # after the adjustments if y_mid is larger than oldY then the line
                    # isn't visible at all so don't draw anything
                    if y_mid < oldY:
                        dc.SetPen(self._dottedPen)
                        dc.DrawLine(x, y_mid, x, oldY)
                
        return y


# -----------------------------------------------------------------------------
# wxWidgets callbacks
# -----------------------------------------------------------------------------

    def OnPaint(self, event):
        """Handles the wx.EVT_PAINT event."""

        dc = wx.PaintDC(self)
        self.PrepareDC(dc)

        if not self._anchor:
            return

        dc.SetFont(self._normalFont)
        dc.SetPen(self._dottedPen)
            
        y = 2
        self.PaintLevel(self._anchor, dc, 0, y)
        

    def OnEraseBackground(self, event):
        """Handles the wx.EVT_ERASE_BACKGROUND event."""

        # Can we actually do something here (or in OnPaint()) To Handle
        # background images that are stretchable or always centered?
        # I tried but I get enormous flickering...
        
        if not self._backgroundImage:
            event.Skip()
            return

        if self._imageStretchStyle == _StyleTile:
            dc = event.GetDC()

            if not dc:
                dc = wx.ClientDC(self)
                rect = self.GetUpdateRegion().GetBox()
                dc.SetClippingRect(rect)

            self.TileBackground(dc)


    def TileBackground(self, dc):
        """Tiles the background image to fill all the available area."""

        sz = self.GetClientSize()
        w = self._backgroundImage.GetWidth()
        h = self._backgroundImage.GetHeight()

        x = 0

        while x < sz.width:
            y = 0

            while y < sz.height:
                dc.DrawBitmap(self._backgroundImage, x, y, True)
                y = y + h

            x = x + w        
        

    def OnSetFocus(self, event):
        """Handles the wx.EVT_SET_FOCUS event."""

        self._hasFocus = True
        self.RefreshSelected()
        event.Skip()


    def OnKillFocus(self, event):
        """Handles the wx.EVT_KILL_FOCUS event."""

        self._hasFocus = False
        self.RefreshSelected()
        event.Skip()


    def OnKeyDown(self, event):
        """Handles the wx.EVT_CHAR event, sending a EVT_TREE_KEY_DOWN event."""

        te = TreeEvent(wxEVT_TREE_KEY_DOWN, self.GetId())
        te._evtKey = event
        te.SetEventObject(self)
        
        if self.GetEventHandler().ProcessEvent(te):
            # intercepted by the user code
            return

        if self._current is None or self._key_current is None:
        
            event.Skip()
            return
        
        # how should the selection work for this event?
        is_multiple, extended_select, unselect_others = EventFlagsToSelType(self.GetTreeStyle(), event.ShiftDown(), event.CmdDown())

        # + : Expand
        # - : Collaspe
        # * : Expand all/Collapse all
        # ' ' | return : activate
        # up    : go up (not last children!)
        # down  : go down
        # left  : go to parent
        # right : open if parent and go next
        # home  : go to root
        # end   : go to last item without opening parents
        # alnum : start or continue searching for the item with this prefix
        
        keyCode = event.GetKeyCode()

        if keyCode in [ord("+"), wx.WXK_ADD]:       # "+"
            if self._current.HasPlus() and not self.IsExpanded(self._current) and self.IsEnabled(self._current):
                self.Expand(self._current)
                
        elif keyCode in [ord("*"), wx.WXK_MULTIPLY]:  # "*"
            if not self.IsExpanded(self._current) and self.IsEnabled(self._current):
                # expand all
                self.ExpandAll(self._current)

        elif keyCode in [ord("-"), wx.WXK_SUBTRACT]:  # "-"
            if self.IsExpanded(self._current):
                self.Collapse(self._current)
            
        elif keyCode == wx.WXK_MENU:
            # Use the item's bounding rectangle to determine position for the event
            itemRect = self.GetBoundingRect(self._current, True)
            event = TreeEvent(wxEVT_TREE_ITEM_MENU, self.GetId())
            event._item = self._current
            # Use the left edge, vertical middle
            event._pointDrag = wx.Point(ItemRect.GetX(), ItemRect.GetY() + ItemRect.GetHeight()/2)
            event.SetEventObject(self)
            self.GetEventHandler().ProcessEvent(event)
                
        elif keyCode in [wx.WXK_RETURN, wx.WXK_SPACE]:

            if not self.IsEnabled(self._current):
                event.Skip()
                return
            
            if not event.HasModifiers():
                event = TreeEvent(wxEVT_TREE_ITEM_ACTIVATED, self.GetId())
                event._item = self._current
                event.SetEventObject(self)
                self.GetEventHandler().ProcessEvent(event)

                if keyCode == wx.WXK_SPACE and self.GetItemType(self._current) > 0:
                    checked = not self.IsItemChecked(self._current)
                    self.CheckItem(self._current, checked)
        
            # in any case, also generate the normal key event for this key,
            # even if we generated the ACTIVATED event above: this is what
            # wxMSW does and it makes sense because you might not want to
            # process ACTIVATED event at all and handle Space and Return
            # directly (and differently) which would be impossible otherwise
            event.Skip()

        # up goes to the previous sibling or to the last
        # of its children if it's expanded
        elif keyCode == wx.WXK_UP:
            prev = self.GetPrevSibling(self._key_current)
            if not prev:
                prev = self.GetItemParent(self._key_current)
                if prev == self.GetRootItem() and self.HasFlag(TR_HIDE_ROOT):
                    return
                
                if prev:
                    current = self._key_current
                    # TODO: Huh?  If we get here, we'd better be the first child of our parent.  How else could it be?
                    if current == self.GetFirstChild(prev)[0] and self.IsEnabled(prev):
                        # otherwise we return to where we came from
                        self.DoSelectItem(prev, unselect_others, extended_select)
                        self._key_current = prev
                
            else:
                current = self._key_current
                
                # We are going to another parent node
                while self.IsExpanded(prev) and self.HasChildren(prev):
                    child = self.GetLastChild(prev)
                    if child:
                        prev = child
                        current = prev
                
                # Try to get the previous siblings and see if they are active
                while prev and not self.IsEnabled(prev):
                    prev = self.GetPrevSibling(prev)

                if not prev:
                    # No previous siblings active: go to the parent and up
                    prev = self.GetItemParent(current)
                    while prev and not self.IsEnabled(prev):
                        prev = self.GetItemParent(prev)
                        
                if prev:
                    self.DoSelectItem(prev, unselect_others, extended_select)
                    self._key_current = prev

        # left arrow goes to the parent
        elif keyCode == wx.WXK_LEFT:
            
            prev = self.GetItemParent(self._current)
            if prev == self.GetRootItem() and self.HasFlag(TR_HIDE_ROOT):            
                # don't go to root if it is hidden
                prev = self.GetPrevSibling(self._current)

            if self.IsExpanded(self._current):
                self.Collapse(self._current)
            else:
                if prev and self.IsEnabled(prev):
                    self.DoSelectItem(prev, unselect_others, extended_select)
                
        elif keyCode == wx.WXK_RIGHT:
            # this works the same as the down arrow except that we
            # also expand the item if it wasn't expanded yet
            if self.IsExpanded(self._current) and self.HasChildren(self._current):
                child, cookie = self.GetFirstChild(self._key_current)
                if self.IsEnabled(child):
                    self.DoSelectItem(child, unselect_others, extended_select)
                    self._key_current = child
            else:
                self.Expand(self._current)
            # fall through

        elif keyCode == wx.WXK_DOWN:
            if self.IsExpanded(self._key_current) and self.HasChildren(self._key_current):

                child = self.GetNextActiveItem(self._key_current)
                
                if child:
                    self.DoSelectItem(child, unselect_others, extended_select)
                    self._key_current = child   
                
            else:
                
                next = self.GetNextSibling(self._key_current)
    
                if not next:
                    current = self._key_current
                    while current and not next:
                        current = self.GetItemParent(current)
                        if current:
                            next = self.GetNextSibling(current)
                            if not self.IsEnabled(next):
                                next = None

                else:
                    while next and not self.IsEnabled(next):
                        next = self.GetNext(next)
                    
                if next:
                    self.DoSelectItem(next, unselect_others, extended_select)
                    self._key_current = next
                    

        # <End> selects the last visible tree item
        elif keyCode == wx.WXK_END:
            
            last = self.GetRootItem()

            while last and self.IsExpanded(last):
            
                lastChild = self.GetLastChild(last)

                # it may happen if the item was expanded but then all of
                # its children have been deleted - so IsExpanded() returned
                # true, but GetLastChild() returned invalid item
                if not lastChild:
                    break

                last = lastChild
            
            if last and self.IsEnabled(last):
            
                self.DoSelectItem(last, unselect_others, extended_select)
                
        # <Home> selects the root item
        elif keyCode == wx.WXK_HOME:
                
            prev = self.GetRootItem()
            
            if not prev:
                return

            if self.HasFlag(TR_HIDE_ROOT):
                prev, cookie = self.GetFirstChild(prev)
                if not prev:
                    return

            if self.IsEnabled(prev):
                self.DoSelectItem(prev, unselect_others, extended_select)
        
        else:
            
            if not event.HasModifiers() and ((keyCode >= ord('0') and keyCode <= ord('9')) or \
                                             (keyCode >= ord('a') and keyCode <= ord('z')) or \
                                             (keyCode >= ord('A') and keyCode <= ord('Z'))):
            
                # find the next item starting with the given prefix
                ch = chr(keyCode)
                id = self.FindItem(self._current, self._findPrefix + ch)
                
                if not id:
                    # no such item
                    return

                if self.IsEnabled(id):                
                    self.SelectItem(id)
                self._findPrefix += ch

                # also start the timer to reset the current prefix if the user
                # doesn't press any more alnum keys soon -- we wouldn't want
                # to use this prefix for a new item search
                if not self._findTimer:
                    self._findTimer = TreeFindTimer(self)
                
                self._findTimer.Start(_DELAY, wx.TIMER_ONE_SHOT)
            
            else:
            
                event.Skip()


    def GetNextActiveItem(self, item, down=True):
        """Returns the next active item. Used Internally at present. """
        
        if down:
            sibling = self.GetNextSibling
        else:
            sibling = self.GetPrevSibling
                
        if self.GetItemType(item) == 2 and not self.IsItemChecked(item):
            # Is an unchecked radiobutton... all its children are inactive
            # try to get the next/previous sibling
            found = 0                 

            while 1:
                child = sibling(item)
                if (child and self.IsEnabled(child)) or not child:
                    break
                item = child

        else:
            # Tha's not a radiobutton... but some of its children can be
            # inactive
            child, cookie = self.GetFirstChild(item)
            while child and not self.IsEnabled(child):
                child, cookie = self.GetNextChild(item, cookie)
                
        if child and self.IsEnabled(child):
            return child
            
        return None
    

    def HitTest(self, point, flags=0):
        """
        Calculates which (if any) item is under the given point, returning the tree item
        at this point plus extra information flags. Flags is a bitlist of the following:

        TREE_HITTEST_ABOVE            above the client area
        TREE_HITTEST_BELOW            below the client area
        TREE_HITTEST_NOWHERE          no item has been hit
        TREE_HITTEST_ONITEMBUTTON     on the button associated to an item
        TREE_HITTEST_ONITEMICON       on the icon associated to an item
        TREE_HITTEST_ONITEMCHECKICON  on the check/radio icon, if present
        TREE_HITTEST_ONITEMINDENT     on the indent associated to an item
        TREE_HITTEST_ONITEMLABEL      on the label (string) associated to an item
        TREE_HITTEST_ONITEMRIGHT      on the right of the label associated to an item
        TREE_HITTEST_TOLEFT           on the left of the client area
        TREE_HITTEST_TORIGHT          on the right of the client area
        TREE_HITTEST_ONITEMUPPERPART  on the upper part (first half) of the item
        TREE_HITTEST_ONITEMLOWERPART  on the lower part (second half) of the item
        TREE_HITTEST_ONITEM           anywhere on the item

        Note: both the item (if any, None otherwise) and the flag are always returned as a tuple.
        """
        
        w, h = self.GetSize()
        flags = 0
        
        if point.x < 0:
            flags |= TREE_HITTEST_TOLEFT
        if point.x > w:
            flags |= TREE_HITTEST_TORIGHT
        if point.y < 0:
            flags |= TREE_HITTEST_ABOVE
        if point.y > h:
            flags |= TREE_HITTEST_BELOW

        if flags:
            return None, flags
 
        if self._anchor == None:
            flags = TREE_HITTEST_NOWHERE
            return None, flags
        
        hit, flags = self._anchor.HitTest(self.CalcUnscrolledPosition(point), self, flags, 0)

        if hit == None:        
            flags = TREE_HITTEST_NOWHERE
            return None, flags

        if not self.IsEnabled(hit):
            return None, flags

        return hit, flags


    def GetBoundingRect(self, item, textOnly=False):
        """Gets the bounding rectangle of the item."""

        if not item:
            raise "\nERROR: Invalid Tree Item. "
    
        i = item

        startX, startY = self.GetViewStart()
        rect = wx.Rect()

        rect.x = i.GetX() - startX*_PIXELS_PER_UNIT
        rect.y = i.GetY() - startY*_PIXELS_PER_UNIT
        rect.width = i.GetWidth()
        rect.height = self.GetLineHeight(i)

        return rect


    def Edit(self, item):
        """
        Internal function. Starts the editing of an item label, sending a
        EVT_TREE_BEGIN_LABEL_EDIT event.
        """

        te = TreeEvent(wxEVT_TREE_BEGIN_LABEL_EDIT, self.GetId())
        te._item = item
        te.SetEventObject(self)
        if self.GetEventHandler().ProcessEvent(te) and not te.IsAllowed():
            # vetoed by user
            return
    
        # We have to call this here because the label in
        # question might just have been added and no screen
        # update taken place.
        if self._dirty:
            if wx.Platform in ["__WXMSW__", "__WXMAC__"]:
                self.Update()
            else:
                wx.YieldIfNeeded()

        if self._textCtrl != None and item != self._textCtrl.item():
            self._textCtrl.StopEditing()

        self._textCtrl = TreeTextCtrl(self, item=item)
        self._textCtrl.SetFocus()

 
    def GetEditControl(self):
        """
        Returns a pointer to the edit TextCtrl if the item is being edited or
        None otherwise (it is assumed that no more than one item may be edited
        simultaneously).
        """
        
        return self._textCtrl


    def OnRenameAccept(self, item, value):
        """
        Called by TreeTextCtrl, to accept the changes and to send the
        EVT_TREE_END_LABEL_EDIT event.
        """

        le = TreeEvent(wxEVT_TREE_END_LABEL_EDIT, self.GetId())
        le._item = item
        le.SetEventObject(self)
        le._label = value
        le._editCancelled = False

        return not self.GetEventHandler().ProcessEvent(le) or le.IsAllowed()
    

    def OnRenameCancelled(self, item):
        """
        Called by TreeTextCtrl, to cancel the changes and to send the
        EVT_TREE_END_LABEL_EDIT event.
        """

        # let owner know that the edit was cancelled
        le = TreeEvent(wxEVT_TREE_END_LABEL_EDIT, self.GetId())
        le._item = item
        le.SetEventObject(self)
        le._label = ""
        le._editCancelled = True

        self.GetEventHandler().ProcessEvent(le)


    def OnRenameTimer(self):
        """The timer for renaming has expired. Start editing."""
        
        self.Edit(self._current)


    def OnMouse(self, event):
        """Handles a bunch of wx.EVT_MOUSE_EVENTS events."""

        if not self._anchor:
            return

        pt = self.CalcUnscrolledPosition(event.GetPosition())

        # Is the mouse over a tree item button?
        flags = 0
        thisItem, flags = self._anchor.HitTest(pt, self, flags, 0)
        underMouse = thisItem
        underMouseChanged = underMouse != self._underMouse

        if underMouse and (flags & TREE_HITTEST_ONITEMBUTTON) and not event.LeftIsDown() and \
           not self._isDragging and (not self._renameTimer or not self._renameTimer.IsRunning()):
            underMouse = underMouse
        else:
            underMouse = None

        if underMouse != self._underMouse:
            if self._underMouse:
                # unhighlight old item
                self._underMouse = None
             
            self._underMouse = underMouse

        # Determines what item we are hovering over and need a tooltip for
        hoverItem = thisItem

        # We do not want a tooltip if we are dragging, or if the rename timer is running
        if underMouseChanged and not self._isDragging and (not self._renameTimer or not self._renameTimer.IsRunning()):
            
            if hoverItem is not None:
                # Ask the tree control what tooltip (if any) should be shown
                hevent = TreeEvent(wxEVT_TREE_ITEM_GETTOOLTIP, self.GetId())
                hevent._item = hoverItem
                hevent.SetEventObject(self)

                if self.GetEventHandler().ProcessEvent(hevent) and hevent.IsAllowed():
                    self.SetToolTip(hevent._label)

                if hoverItem.IsHyperText() and (flags & TREE_HITTEST_ONITEMLABEL) and hoverItem.IsEnabled():
                    self.SetCursor(wx.StockCursor(wx.CURSOR_HAND))
                    self._isonhyperlink = True
                else:
                    if self._isonhyperlink:
                        self.SetCursor(wx.StockCursor(wx.CURSOR_ARROW))
                        self._isonhyperlink = False
                
        # we process left mouse up event (enables in-place edit), right down
        # (pass to the user code), left dbl click (activate item) and
        # dragging/moving events for items drag-and-drop

        if not (event.LeftDown() or event.LeftUp() or event.RightDown() or event.LeftDClick() or \
                event.Dragging() or ((event.Moving() or event.RightUp()) and self._isDragging)):
        
            event.Skip()
            return
                    
        flags = 0
        item, flags = self._anchor.HitTest(pt, self, flags, 0)

        if event.Dragging() and not self._isDragging and ((flags & TREE_HITTEST_ONITEMICON) or (flags & TREE_HITTEST_ONITEMLABEL)):
        
            if self._dragCount == 0:
                self._dragStart = pt

            self._countDrag = 0
            self._dragCount = self._dragCount + 1

            if self._dragCount != 3:
                # wait until user drags a bit further...
                return
            
            command = (event.RightIsDown() and [wxEVT_TREE_BEGIN_RDRAG] or [wxEVT_TREE_BEGIN_DRAG])[0]

            nevent = TreeEvent(command, self.GetId())
            nevent._item = self._current
            nevent.SetEventObject(self)
            newpt = self.CalcScrolledPosition(pt)
            nevent.SetPoint(newpt)

            # by default the dragging is not supported, the user code must
            # explicitly allow the event for it to take place
            nevent.Veto()

            if self.GetEventHandler().ProcessEvent(nevent) and nevent.IsAllowed():
                
                # we're going to drag this item
                self._isDragging = True

                # remember the old cursor because we will change it while
                # dragging
                self._oldCursor = self._cursor

                # in a single selection control, hide the selection temporarily
                if not (self.GetTreeStyle() & TR_MULTIPLE):
                    self._oldSelection = self.GetSelection()

                    if self._oldSelection:
                    
                        self._oldSelection.SetHilight(False)
                        self.RefreshLine(self._oldSelection)
                else:
                    selections = self.GetSelections()
                    if len(selections) == 1:
                        self._oldSelection = selections[0]
                        self._oldSelection.SetHilight(False)
                        self.RefreshLine(self._oldSelection)

                if self._dragImage:
                    del self._dragImage

                # Create the custom draw image from the icons and the text of the item                    
                self._dragImage = DragImage(self, self._current)
                self._dragImage.BeginDrag(wx.Point(0,0), self)
                self._dragImage.Show()
                self._dragImage.Move(self.CalcScrolledPosition(pt))
            
        elif event.Dragging() and self._isDragging:

            self._dragImage.Move(self.CalcScrolledPosition(pt))

            if self._countDrag == 0 and item:
                self._oldItem = item

            if item != self._dropTarget:
                    
                # unhighlight the previous drop target
                if self._dropTarget:
                    self._dropTarget.SetHilight(False)
                    self.RefreshLine(self._dropTarget)
                if item:
                    item.SetHilight(True)
                    self.RefreshLine(item)
                    self._countDrag = self._countDrag + 1
                self._dropTarget = item

                self.Update()

            if self._countDrag >= 3:
                # Here I am trying to avoid ugly repainting problems... hope it works
                self.RefreshLine(self._oldItem)
                self._countDrag = 0

        elif (event.LeftUp() or event.RightUp()) and self._isDragging:

            if self._dragImage:
                self._dragImage.EndDrag()

            if self._dropTarget:
                self._dropTarget.SetHilight(False)
                
            if self._oldSelection:
            
                self._oldSelection.SetHilight(True)
                self.RefreshLine(self._oldSelection)
                self._oldSelection = None
            
            # generate the drag end event
            event = TreeEvent(wxEVT_TREE_END_DRAG, self.GetId())
            event._item = item
            event._pointDrag = self.CalcScrolledPosition(pt)
            event.SetEventObject(self)

            self.GetEventHandler().ProcessEvent(event)

            self._isDragging = False
            self._dropTarget = None
            
            self.SetCursor(self._oldCursor)

            if wx.Platform in ["__WXMSW__", "__WXMAC__"]:
                self.Refresh()
            else:
                # Probably this is not enough on GTK. Try a Refresh() if it does not work.
                wx.YieldIfNeeded()
        
        else:

            # If we got to this point, we are not dragging or moving the mouse.
            # Because the code in carbon/toplevel.cpp will only set focus to the tree
            # if we skip for EVT_LEFT_DOWN, we MUST skip this event here for focus to work.
            # We skip even if we didn't hit an item because we still should
            # restore focus to the tree control even if we didn't exactly hit an item.
            if event.LeftDown():
                self._hasFocus = True
                self.SetFocusIgnoringChildren()
                event.Skip()
            
            # here we process only the messages which happen on tree items

            self._dragCount = 0

            if item == None:
                if self._textCtrl != None and item != self._textCtrl.item():
                    self._textCtrl.StopEditing()
                return  # we hit the blank area

            if event.RightDown():
                
                if self._textCtrl != None and item != self._textCtrl.item():
                    self._textCtrl.StopEditing()

                self._hasFocus = True
                self.SetFocusIgnoringChildren()
                    
                # If the item is already selected, do not update the selection.
                # Multi-selections should not be cleared if a selected item is clicked.
                if not self.IsSelected(item):
                
                    self.DoSelectItem(item, True, False)

                nevent = TreeEvent(wxEVT_TREE_ITEM_RIGHT_CLICK, self.GetId())
                nevent._item = item
                nevent._pointDrag = self.CalcScrolledPosition(pt)
                nevent.SetEventObject(self)
                event.Skip(not self.GetEventHandler().ProcessEvent(nevent))

                # Consistent with MSW (for now), send the ITEM_MENU *after*
                # the RIGHT_CLICK event. TODO: This behaviour may change.
                nevent2 = TreeEvent(wxEVT_TREE_ITEM_MENU, self.GetId())
                nevent2._item = item
                nevent2._pointDrag = self.CalcScrolledPosition(pt)
                nevent2.SetEventObject(self)
                self.GetEventHandler().ProcessEvent(nevent2)
            
            elif event.LeftUp():
            
                # this facilitates multiple-item drag-and-drop

                if self.HasFlag(TR_MULTIPLE):
                
                    selections = self.GetSelections()

                    if len(selections) > 1 and not event.CmdDown() and not event.ShiftDown():
                    
                        self.DoSelectItem(item, True, False)
                    
                if self._lastOnSame:
                
                    if item == self._current and (flags & TREE_HITTEST_ONITEMLABEL) and self.HasFlag(TR_EDIT_LABELS):
                    
                        if self._renameTimer:
                        
                            if self._renameTimer.IsRunning():
                                
                                self._renameTimer.Stop()
                        
                        else:
                        
                            self._renameTimer = TreeRenameTimer(self)
                        
                        self._renameTimer.Start(_DELAY, True)
                    
                    self._lastOnSame = False
                
            
            else: # !RightDown() && !LeftUp() ==> LeftDown() || LeftDClick()

                if not item or not item.IsEnabled():
                    if self._textCtrl != None and item != self._textCtrl.item():
                        self._textCtrl.StopEditing()
                    return

                if self._textCtrl != None and item != self._textCtrl.item():
                    self._textCtrl.StopEditing()

                self._hasFocus = True
                self.SetFocusIgnoringChildren()
                
                if event.LeftDown():
                
                    self._lastOnSame = item == self._current
                    
                if flags & TREE_HITTEST_ONITEMBUTTON:
                
                    # only toggle the item for a single click, double click on
                    # the button doesn't do anything (it toggles the item twice)
                    if event.LeftDown():
                    
                        self.Toggle(item)
                    
                    # don't select the item if the button was clicked
                    return

                if item.GetType() > 0 and (flags & TREE_HITTEST_ONITEMCHECKICON):

                    if event.LeftDown():
                        
                        self.CheckItem(item, not self.IsItemChecked(item))
                        
                    return                                            

                # clear the previously selected items, if the
                # user clicked outside of the present selection.
                # otherwise, perform the deselection on mouse-up.
                # this allows multiple drag and drop to work.
                # but if Cmd is down, toggle selection of the clicked item
                if not self.IsSelected(item) or event.CmdDown():

                    if flags & TREE_HITTEST_ONITEM:
                        # how should the selection work for this event?
                        if item.IsHyperText():
                            self.SetItemVisited(item, True)
                        
                        is_multiple, extended_select, unselect_others = EventFlagsToSelType(self.GetTreeStyle(),
                                                                                            event.ShiftDown(),
                                                                                            event.CmdDown())

                        self.DoSelectItem(item, unselect_others, extended_select)
                
                # For some reason, Windows isn't recognizing a left double-click,
                # so we need to simulate it here.  Allow 200 milliseconds for now.
                if event.LeftDClick():
                
                    # double clicking should not start editing the item label
                    if self._renameTimer:
                        self._renameTimer.Stop()

                    self._lastOnSame = False

                    # send activate event first
                    nevent = TreeEvent(wxEVT_TREE_ITEM_ACTIVATED, self.GetId())
                    nevent._item = item
                    nevent._pointDrag = self.CalcScrolledPosition(pt)
                    nevent.SetEventObject(self)
                    if not self.GetEventHandler().ProcessEvent(nevent):
                    
                        # if the user code didn't process the activate event,
                        # handle it ourselves by toggling the item when it is
                        # double clicked
##                        if item.HasPlus():
                        self.Toggle(item)
                        

    def OnInternalIdle(self, event):
        """Performs operations in idle time (essentially drawing)."""

        # Check if we need to select the root item
        # because nothing else has been selected.
        # Delaying it means that we can invoke event handlers
        # as required, when a first item is selected.
        if not self.HasFlag(TR_MULTIPLE) and not self.GetSelection():
        
            if self._select_me:
                self.SelectItem(self._select_me)
            elif self.GetRootItem():
                self.SelectItem(self.GetRootItem())
        
        # after all changes have been done to the tree control,
        # we actually redraw the tree when everything is over

        if not self._dirty:
            return
        if self._freezeCount:
            return

        self._dirty = False

        self.CalculatePositions()
        self.Refresh()
        self.AdjustMyScrollbars()

#        event.Skip()        


    def CalculateSize(self, item, dc):
        """Calculates overall position and size of an item."""

        attr = item.GetAttributes()

        if attr and attr.HasFont():
            dc.SetFont(attr.GetFont())
        elif item.IsBold():
            dc.SetFont(self._boldFont)
        else:
            dc.SetFont(self._normalFont)

        text_w, text_h, dummy = dc.GetMultiLineTextExtent(item.GetText())
        text_h+=2

        # restore normal font
        dc.SetFont(self._normalFont)

        image_w, image_h = 0, 0
        image = item.GetCurrentImage()

        if image != _NO_IMAGE:
        
            if self._imageListNormal:
            
                image_w, image_h = self._imageListNormal.GetSize(image)
                image_w += 4

        total_h = ((image_h > text_h) and [image_h] or [text_h])[0]

        checkimage = item.GetCurrentCheckedImage()
        if checkimage is not None:
            wcheck, hcheck = self._imageListCheck.GetSize(checkimage)
            wcheck += 4
        else:
            wcheck = 0           

        if total_h < 30:
            total_h += 2            # at least 2 pixels
        else:
            total_h += total_h/10   # otherwise 10% extra spacing

        if total_h > self._lineHeight:
            self._lineHeight = total_h

        if not item.GetWindow():
            item.SetWidth(image_w+text_w+wcheck+2)
            item.SetHeight(total_h)
        else:
            item.SetWidth(item.GetWindowSize()[0]+image_w+text_w+wcheck+2)


    def CalculateLevel(self, item, dc, level, y):
        """Calculates the level of an item."""

        x = level*self._indent
        
        if not self.HasFlag(TR_HIDE_ROOT):
        
            x += self._indent
        
        elif level == 0:
        
            # a hidden root is not evaluated, but its
            # children are always calculated
            children = item.GetChildren()
            count = len(children)
            level = level + 1
            for n in xrange(count):
                y = self.CalculateLevel(children[n], dc, level, y)  # recurse
                
            return y
        
        self.CalculateSize(item, dc)

        # set its position
        item.SetX(x+self._spacing)
        item.SetY(y)
        y += self.GetLineHeight(item)

        if not item.IsExpanded():
            # we don't need to calculate collapsed branches
            return y

        children = item.GetChildren()
        count = len(children)
        level = level + 1
        for n in xrange(count):
            y = self.CalculateLevel(children[n], dc, level, y)  # recurse
        
        return y
    

    def CalculatePositions(self):
        """Calculates all the positions of the visible items."""

        if not self._anchor:
            return

        dc = wx.ClientDC(self)
        self.PrepareDC(dc)

        dc.SetFont(self._normalFont)
        dc.SetPen(self._dottedPen)
        y = 2
        y = self.CalculateLevel(self._anchor, dc, 0, y) # start recursion


    def RefreshSubtree(self, item):
        """Refreshes a damaged subtree of an item."""

        if self._dirty:
            return
        if self._freezeCount:
            return

        client = self.GetClientSize()

        rect = wx.Rect()
        x, rect.y = self.CalcScrolledPosition(0, item.GetY())
        rect.width = client.x
        rect.height = client.y

        self.Refresh(True, rect)
        self.AdjustMyScrollbars()


    def RefreshLine(self, item):
        """Refreshes a damaged item line."""

        if self._dirty:
            return
        if self._freezeCount:
            return

        rect = wx.Rect()
        x, rect.y = self.CalcScrolledPosition(0, item.GetY())
        rect.width = self.GetClientSize().x
        rect.height = self.GetLineHeight(item)

        self.Refresh(True, rect)


    def RefreshSelected(self):
        """Refreshes a damaged selected item line."""

        if self._freezeCount:
            return

        # TODO: this is awfully inefficient, we should keep the list of all
        #       selected items internally, should be much faster
        if self._anchor:
            self.RefreshSelectedUnder(self._anchor)


    def RefreshSelectedUnder(self, item):
        """Refreshes the selected items under the given item."""

        if self._freezeCount:
            return

        if item.IsSelected():
            self.RefreshLine(item)

        children = item.GetChildren()
        for child in children:
            self.RefreshSelectedUnder(child)
    

    def Freeze(self):
        """Freeze CustomTreeCtrl."""

        self._freezeCount = self._freezeCount + 1


    def Thaw(self):
        """Thaw CustomTreeCtrl."""

        if self._freezeCount == 0:
            raise "\nERROR: Thawing Unfrozen Tree Control?"

        self._freezeCount = self._freezeCount - 1
        
        if not self._freezeCount:
            self.Refresh()
    

    # ----------------------------------------------------------------------------
    # changing colours: we need to refresh the tree control
    # ----------------------------------------------------------------------------

    def SetBackgroundColour(self, colour):
        """Changes the background colour of CustomTreeCtrl."""

        if not wx.Window.SetBackgroundColour(self, colour):
            return False

        if self._freezeCount:
            return True

        self.Refresh()

        return True


    def SetForegroundColour(self, colour):
        """Changes the foreground colour of CustomTreeCtrl."""

        if not wx.Window.SetForegroundColour(self, colour):
            return False

        if self._freezeCount:
            return True

        self.Refresh()

        return True

    
    def OnGetToolTip(self, event):
        """
        Process the tooltip event, to speed up event processing. Does not actually
        get a tooltip.
        """

        event.Veto()


    def DoGetBestSize(self):
        """Something is better than nothing..."""
        
        # something is better than nothing...
        # 100x80 is what the MSW version will get from the default
        # wxControl::DoGetBestSize

        return wx.Size(100, 80)


    def GetClassDefaultAttributes(self):
        """Gets the class default attributes."""

        attr = wx.VisualAttributes()
        attr.colFg = wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWTEXT)
        attr.colBg = wx.SystemSettings_GetColour(wx.SYS_COLOUR_LISTBOX)
        attr.font  = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        return attr


