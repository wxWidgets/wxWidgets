# --------------------------------------------------------------------------- #
# FLATNOTEBOOK Widget wxPython IMPLEMENTATION
#
# Original C++ Code From Eran. You Can Find It At:
#
# http://wxforum.shadonet.com/viewtopic.php?t=5761&start=0
#
# License: wxWidgets license
#
#
# Python Code By:
#
# Andrea Gavana, @ 02 Oct 2006
# Latest Revision: 12 Oct 2006, 20.00 GMT
#
#
# For All Kind Of Problems, Requests Of Enhancements And Bug Reports, Please
# Write To Me At:
#
# andrea.gavana@gmail.com
# gavana@kpo.kz
#
# Or, Obviously, To The wxPython Mailing List!!!
#
#
# End Of Comments
# --------------------------------------------------------------------------- #

"""
The FlatNotebook is a full implementation of the wx.Notebook, and designed to be
a drop-in replacement for wx.Notebook. The API functions are similar so one can
expect the function to behave in the same way. 

Some features:

  - The buttons are highlighted a la Firefox style
  - The scrolling is done for bulks of tabs (so, the scrolling is faster and better)
  - The buttons area is never overdrawn by tabs (unlike many other implementations I saw)
  - It is a generic control
  - Currently there are 4 differnt styles - VC8, VC 71, Standard and Fancy
  - Mouse middle click can be used to close tabs
  - A function to add right click menu for tabs (simple as SetRightClickMenu)
  - All styles has bottom style as well (they can be drawn in the bottom of screen)
  - An option to hide 'X' button or navigation buttons (separately)
  - Gradient coloring of the selected tabs and border
  - Support for drag 'n' drop of tabs, both in the same notebook or to another notebook
  - Possibility to have closing button on the active tab directly
  - Support for disabled tabs
  - Colours for active/inactive tabs, and captions
  - Background of tab area can be painted in gradient (VC8 style only)
  - Colourful tabs - a random gentle colour is generated for each new tab (very cool, VC8 style only)


And much more.


License And Version:

FlatNotebook Is Freeware And Distributed Under The wxPython License. 

Latest Revision: Andrea Gavana @ 12 Oct 2006, 20.00 GMT

Version 2.0.

@undocumented: FNB_HEIGHT_SPACER, VERTICAL_BORDER_PADDING, VC8_SHAPE_LEN,
    wxEVT*, left_arrow_*, right_arrow*, x_button*, down_arrow*,
    FNBDragInfo, FNBDropTarget, GetMondrian*
"""

__docformat__ = "epytext"


#----------------------------------------------------------------------
# Beginning Of FLATNOTEBOOK wxPython Code
#----------------------------------------------------------------------

import wx
import random
import math
import weakref
import cPickle
    
# Check for the new method in 2.7 (not present in 2.6.3.3)
if wx.VERSION_STRING < "2.7":
    wx.Rect.Contains = lambda self, point: wx.Rect.Inside(self, point)

FNB_HEIGHT_SPACER = 10

# Use Visual Studio 2003 (VC7.1) style for tabs
FNB_VC71 = 1
"""Use Visual Studio 2003 (VC7.1) style for tabs"""

# Use fancy style - square tabs filled with gradient coloring
FNB_FANCY_TABS = 2
"""Use fancy style - square tabs filled with gradient coloring"""

# Draw thin border around the page
FNB_TABS_BORDER_SIMPLE = 4
"""Draw thin border around the page"""

# Do not display the 'X' button
FNB_NO_X_BUTTON = 8
"""Do not display the 'X' button"""

# Do not display the Right / Left arrows
FNB_NO_NAV_BUTTONS = 16
"""Do not display the right/left arrows"""

# Use the mouse middle button for cloing tabs
FNB_MOUSE_MIDDLE_CLOSES_TABS = 32
"""Use the mouse middle button for cloing tabs"""

# Place tabs at bottom - the default is to place them
# at top
FNB_BOTTOM = 64
"""Place tabs at bottom - the default is to place them at top"""

# Disable dragging of tabs
FNB_NODRAG = 128
"""Disable dragging of tabs"""

# Use Visual Studio 2005 (VC8) style for tabs
FNB_VC8 = 256
"""Use Visual Studio 2005 (VC8) style for tabs"""

# Place 'X' on a tab
FNB_X_ON_TAB = 512
"""Place 'X' close button on the active tab"""

FNB_BACKGROUND_GRADIENT = 1024
"""Use gradients to paint the tabs background"""

FNB_COLORFUL_TABS = 2048
"""Use colourful tabs (VC8 style only)"""

# Style to close tab using double click - styles 1024, 2048 are reserved
FNB_DCLICK_CLOSES_TABS = 4096
"""Style to close tab using double click"""

FNB_SMART_TABS = 8192
"""Use Smart Tabbing, like Alt+Tab on Windows"""

FNB_DROPDOWN_TABS_LIST = 16384
"""Use a dropdown menu on the left in place of the arrows"""

FNB_ALLOW_FOREIGN_DND = 32768
"""Allows drag 'n' drop operations between different L{FlatNotebook}s"""

VERTICAL_BORDER_PADDING = 4

# Button size is a 16x16 xpm bitmap
BUTTON_SPACE = 16
"""Button size is a 16x16 xpm bitmap"""

VC8_SHAPE_LEN = 16

MASK_COLOR  = wx.Colour(0, 128, 128)
"""Mask colour for the arrow bitmaps"""

# Button status
FNB_BTN_PRESSED = 2
"""Navigation button is pressed"""
FNB_BTN_HOVER = 1
"""Navigation button is hovered"""
FNB_BTN_NONE = 0
"""No navigation"""

# Hit Test results
FNB_TAB = 1             # On a tab
"""Indicates mouse coordinates inside a tab"""
FNB_X = 2               # On the X button
"""Indicates mouse coordinates inside the I{X} region"""
FNB_TAB_X = 3           # On the 'X' button (tab's X button)
"""Indicates mouse coordinates inside the I{X} region in a tab"""
FNB_LEFT_ARROW = 4      # On the rotate left arrow button
"""Indicates mouse coordinates inside the left arrow region"""
FNB_RIGHT_ARROW = 5     # On the rotate right arrow button
"""Indicates mouse coordinates inside the right arrow region"""
FNB_DROP_DOWN_ARROW = 6 # On the drop down arrow button
"""Indicates mouse coordinates inside the drop down arrow region"""
FNB_NOWHERE = 0         # Anywhere else
"""Indicates mouse coordinates not on any tab of the notebook"""

FNB_DEFAULT_STYLE = FNB_MOUSE_MIDDLE_CLOSES_TABS
"""L{FlatNotebook} default style"""

# FlatNotebook Events:
# wxEVT_FLATNOTEBOOK_PAGE_CHANGED: Event Fired When You Switch Page;
# wxEVT_FLATNOTEBOOK_PAGE_CHANGING: Event Fired When You Are About To Switch
# Pages, But You Can Still "Veto" The Page Changing By Avoiding To Call
# event.Skip() In Your Event Handler;
# wxEVT_FLATNOTEBOOK_PAGE_CLOSING: Event Fired When A Page Is Closing, But
# You Can Still "Veto" The Page Changing By Avoiding To Call event.Skip()
# In Your Event Handler;
# wxEVT_FLATNOTEBOOK_PAGE_CLOSED: Event Fired When A Page Is Closed.
# wxEVT_FLATNOTEBOOK_PAGE_CONTEXT_MENU: Event Fired When A Menu Pops-up In A Tab.

wxEVT_FLATNOTEBOOK_PAGE_CHANGED = wx.NewEventType()
wxEVT_FLATNOTEBOOK_PAGE_CHANGING = wx.NewEventType()
wxEVT_FLATNOTEBOOK_PAGE_CLOSING = wx.NewEventType()
wxEVT_FLATNOTEBOOK_PAGE_CLOSED = wx.NewEventType()
wxEVT_FLATNOTEBOOK_PAGE_CONTEXT_MENU = wx.NewEventType()

#-----------------------------------#
#        FlatNotebookEvent
#-----------------------------------#

EVT_FLATNOTEBOOK_PAGE_CHANGED = wx.PyEventBinder(wxEVT_FLATNOTEBOOK_PAGE_CHANGED, 1)
"""Notify client objects when the active page in L{FlatNotebook} 
has changed."""
EVT_FLATNOTEBOOK_PAGE_CHANGING = wx.PyEventBinder(wxEVT_FLATNOTEBOOK_PAGE_CHANGING, 1)
"""Notify client objects when the active page in L{FlatNotebook} 
is about to change."""
EVT_FLATNOTEBOOK_PAGE_CLOSING = wx.PyEventBinder(wxEVT_FLATNOTEBOOK_PAGE_CLOSING, 1)
"""Notify client objects when a page in L{FlatNotebook} is closing."""
EVT_FLATNOTEBOOK_PAGE_CLOSED = wx.PyEventBinder(wxEVT_FLATNOTEBOOK_PAGE_CLOSED, 1)
"""Notify client objects when a page in L{FlatNotebook} has been closed."""
EVT_FLATNOTEBOOK_PAGE_CONTEXT_MENU = wx.PyEventBinder(wxEVT_FLATNOTEBOOK_PAGE_CONTEXT_MENU, 1)
"""Notify client objects when a pop-up menu should appear next to a tab."""


# Some icons in XPM format

left_arrow_disabled_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #555555",
    "# c #000000",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````.```````",
    "```````..```````",
    "``````.`.```````",
    "`````.``.```````",
    "````.```.```````",
    "`````.``.```````",
    "``````.`.```````",
    "```````..```````",
    "````````.```````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````"
    ]

x_button_pressed_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #4766e0",
    "# c #9e9ede",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "`..............`",
    "`.############.`",
    "`.############.`",
    "`.############.`",
    "`.###aa####aa#.`",
    "`.####aa##aa##.`",
    "`.#####aaaa###.`",
    "`.######aa####.`",
    "`.#####aaaa###.`",
    "`.####aa##aa##.`",
    "`.###aa####aa#.`",
    "`.############.`",
    "`..............`",
    "````````````````",
    "````````````````"
    ]


left_arrow_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #555555",
    "# c #000000",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````.```````",
    "```````..```````",
    "``````...```````",
    "`````....```````",
    "````.....```````",
    "`````....```````",
    "``````...```````",
    "```````..```````",
    "````````.```````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````"
    ]

x_button_hilite_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #4766e0",
    "# c #c9dafb",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "`..............`",
    "`.############.`",
    "`.############.`",
    "`.##aa####aa##.`",
    "`.###aa##aa###.`",
    "`.####aaaa####.`",
    "`.#####aa#####.`",
    "`.####aaaa####.`",
    "`.###aa##aa###.`",
    "`.##aa####aa##.`",
    "`.############.`",
    "`.############.`",
    "`..............`",
    "````````````````",
    "````````````````"
    ]

x_button_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #555555",
    "# c #000000",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````..````..````",
    "`````..``..`````",
    "``````....``````",
    "```````..```````",
    "``````....``````",
    "`````..``..`````",
    "````..````..````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````"
    ]

left_arrow_pressed_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #4766e0",
    "# c #9e9ede",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "`..............`",
    "`.############.`",
    "`.############.`",
    "`.#######a####.`",
    "`.######aa####.`",
    "`.#####aaa####.`",
    "`.####aaaa####.`",
    "`.###aaaaa####.`",
    "`.####aaaa####.`",
    "`.#####aaa####.`",
    "`.######aa####.`",
    "`.#######a####.`",
    "`..............`",
    "````````````````",
    "````````````````"
    ]

left_arrow_hilite_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #4766e0",
    "# c #c9dafb",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "`..............`",
    "`.############.`",
    "`.######a#####.`",
    "`.#####aa#####.`",
    "`.####aaa#####.`",
    "`.###aaaa#####.`",
    "`.##aaaaa#####.`",
    "`.###aaaa#####.`",
    "`.####aaa#####.`",
    "`.#####aa#####.`",
    "`.######a#####.`",
    "`.############.`",
    "`..............`",
    "````````````````",
    "````````````````"
    ]

right_arrow_disabled_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #555555",
    "# c #000000",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "````````````````",
    "````````````````",
    "```````.````````",
    "```````..```````",
    "```````.`.``````",
    "```````.``.`````",
    "```````.```.````",
    "```````.``.`````",
    "```````.`.``````",
    "```````..```````",
    "```````.````````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````"
    ]

right_arrow_hilite_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #4766e0",
    "# c #c9dafb",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "`..............`",
    "`.############.`",
    "`.####a#######.`",
    "`.####aa######.`",
    "`.####aaa#####.`",
    "`.####aaaa####.`",
    "`.####aaaaa###.`",
    "`.####aaaa####.`",
    "`.####aaa#####.`",
    "`.####aa######.`",
    "`.####a#######.`",
    "`.############.`",
    "`..............`",
    "````````````````",
    "````````````````"
    ]

right_arrow_pressed_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #4766e0",
    "# c #9e9ede",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "`..............`",
    "`.############.`",
    "`.############.`",
    "`.#####a######.`",
    "`.#####aa#####.`",
    "`.#####aaa####.`",
    "`.#####aaaa###.`",
    "`.#####aaaaa##.`",
    "`.#####aaaa###.`",
    "`.#####aaa####.`",
    "`.#####aa#####.`",
    "`.#####a######.`",
    "`..............`",
    "````````````````",
    "````````````````"
    ]


right_arrow_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #555555",
    "# c #000000",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "````````````````",
    "````````````````",
    "```````.````````",
    "```````..```````",
    "```````...``````",
    "```````....`````",
    "```````.....````",
    "```````....`````",
    "```````...``````",
    "```````..```````",
    "```````.````````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````"
    ]

down_arrow_hilite_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #4766e0",
    "# c #c9dafb",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "``.............`",
    "``.###########.`",
    "``.###########.`",
    "``.###########.`",
    "``.#aaaaaaaaa#.`",
    "``.##aaaaaaa##.`",
    "``.###aaaaa###.`",
    "``.####aaa####.`",
    "``.#####a#####.`",
    "``.###########.`",
    "``.###########.`",
    "``.###########.`",
    "``.............`",
    "````````````````",
    "````````````````"
    ]

down_arrow_pressed_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #4766e0",
    "# c #9e9ede",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "``.............`",
    "``.###########.`",
    "``.###########.`",
    "``.###########.`",
    "``.###########.`",
    "``.###########.`",
    "``.#aaaaaaaaa#.`",
    "``.##aaaaaaa##.`",
    "``.###aaaaa###.`",
    "``.####aaa####.`",
    "``.#####a#####.`",
    "``.###########.`",
    "``.............`",
    "````````````````",
    "````````````````"
    ]


down_arrow_xpm = [
    "    16    16        8            1",
    "` c #008080",
    ". c #000000",
    "# c #000000",
    "a c #000000",
    "b c #000000",
    "c c #000000",
    "d c #000000",
    "e c #000000",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````.........```",
    "`````.......````",
    "``````.....`````",
    "```````...``````",
    "````````.```````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````",
    "````````````````"
    ]


#----------------------------------------------------------------------
def GetMondrianData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00 \x00\x00\x00 \x08\x06\x00\
\x00\x00szz\xf4\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\x00qID\
ATX\x85\xed\xd6;\n\x800\x10E\xd1{\xc5\x8d\xb9r\x97\x16\x0b\xad$\x8a\x82:\x16\
o\xda\x84pB2\x1f\x81Fa\x8c\x9c\x08\x04Z{\xcf\xa72\xbcv\xfa\xc5\x08 \x80r\x80\
\xfc\xa2\x0e\x1c\xe4\xba\xfaX\x1d\xd0\xde]S\x07\x02\xd8>\xe1wa-`\x9fQ\xe9\
\x86\x01\x04\x10\x00\\(Dk\x1b-\x04\xdc\x1d\x07\x14\x98;\x0bS\x7f\x7f\xf9\x13\
\x04\x10@\xf9X\xbe\x00\xc9 \x14K\xc1<={\x00\x00\x00\x00IEND\xaeB`\x82' 


def GetMondrianBitmap():
    return wx.BitmapFromImage(GetMondrianImage().Scale(16, 16))


def GetMondrianImage():
    import cStringIO
    stream = cStringIO.StringIO(GetMondrianData())
    return wx.ImageFromStream(stream)


def GetMondrianIcon():
    icon = wx.EmptyIcon()
    icon.CopyFromBitmap(GetMondrianBitmap())
    return icon
#----------------------------------------------------------------------


def LightColour(color, percent):
    """ Brighten input colour by percent. """
    
    end_color = wx.WHITE
    
    rd = end_color.Red() - color.Red()
    gd = end_color.Green() - color.Green()
    bd = end_color.Blue() - color.Blue()

    high = 100

    # We take the percent way of the color from color -. white
    i = percent
    r = color.Red() + ((i*rd*100)/high)/100
    g = color.Green() + ((i*gd*100)/high)/100
    b = color.Blue() + ((i*bd*100)/high)/100
    return wx.Colour(r, g, b)


def RandomColour(): 
    """ Creates a random colour. """
    
    r = random.randint(0, 255) # Random value betweem 0-255
    g = random.randint(0, 255) # Random value betweem 0-255
    b = random.randint(0, 255) # Random value betweem 0-255

    return wx.Colour(r, g, b)


def PaintStraightGradientBox(dc, rect, startColor, endColor, vertical=True):
    """ Draws a gradient colored box from startColor to endColor. """

    rd = endColor.Red() - startColor.Red()
    gd = endColor.Green() - startColor.Green()
    bd = endColor.Blue() - startColor.Blue()

    # Save the current pen and brush
    savedPen = dc.GetPen()
    savedBrush = dc.GetBrush()

    if vertical:
        high = rect.GetHeight()-1
    else:
        high = rect.GetWidth()-1

    if high < 1:
        return

    for i in xrange(high+1):
    
        r = startColor.Red() + ((i*rd*100)/high)/100
        g = startColor.Green() + ((i*gd*100)/high)/100
        b = startColor.Blue() + ((i*bd*100)/high)/100

        p = wx.Pen(wx.Colour(r, g, b))
        dc.SetPen(p)

        if vertical:
            dc.DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i)
        else:
            dc.DrawLine(rect.x+i, rect.y, rect.x+i, rect.y+rect.height)
    
    # Restore the pen and brush
    dc.SetPen(savedPen)
    dc.SetBrush(savedBrush)


# ---------------------------------------------------------------------------- #
# Class FNBDropSource
# Gives Some Custom UI Feedback during the DnD Operations
# ---------------------------------------------------------------------------- #

class FNBDropSource(wx.DropSource):
    """
    Give some custom UI feedback during the drag and drop operation in this
    function. It is called on each mouse move, so your implementation must
    not be too slow.
    """
    
    def __init__(self, win):
        """ Default class constructor. Used internally. """
        
        wx.DropSource.__init__(self, win)
        self._win = win


    def GiveFeedback(self, effect):
        """ Provides user with a nice feedback when tab is being dragged. """

        self._win.DrawDragHint()
        return False


# ---------------------------------------------------------------------------- #
# Class FNBDragInfo
# Stores All The Information To Allow Drag And Drop Between Different
# FlatNotebooks.
# ---------------------------------------------------------------------------- #

class FNBDragInfo:

    _map = weakref.WeakValueDictionary()

    def __init__(self, container, pageindex):
        """ Default class constructor. """
        
        self._id = id(container)
        FNBDragInfo._map[self._id] = container
        self._pageindex = pageindex


    def GetContainer(self):
        """ Returns the L{FlatNotebook} page (usually a panel). """
        
        return FNBDragInfo._map.get(self._id, None)


    def GetPageIndex(self):
        """ Returns the page index associated with a page. """

        return self._pageindex        


# ---------------------------------------------------------------------------- #
# Class FNBDropTarget
# Simply Used To Handle The OnDrop() Method When Dragging And Dropping Between
# Different FlatNotebooks.
# ---------------------------------------------------------------------------- #

class FNBDropTarget(wx.DropTarget):

    def __init__(self, parent):
        """ Default class constructor. """
        
        wx.DropTarget.__init__(self)

        self._parent = parent
        self._dataobject = wx.CustomDataObject(wx.CustomDataFormat("FlatNotebook"))
        self.SetDataObject(self._dataobject)


    def OnData(self, x, y, dragres):
        """ Handles the OnData() method to call the real DnD routine. """
        
        if not self.GetData():
            return wx.DragNone

        draginfo = self._dataobject.GetData()
        drginfo = cPickle.loads(draginfo)
        
        return self._parent.OnDropTarget(x, y, drginfo.GetPageIndex(), drginfo.GetContainer())


# ---------------------------------------------------------------------------- #
# Class PageInfo
# Contains parameters for every FlatNotebook page
# ---------------------------------------------------------------------------- #

class PageInfo:
    """
    This class holds all the information (caption, image, etc...) belonging to a
    single tab in L{FlatNotebook}.
    """
    
    def __init__(self, caption="", imageindex=-1, tabangle=0, enabled=True):
        """
        Default Class Constructor.

        Parameters:
        @param caption: the tab caption;
        @param imageindex: the tab image index based on the assigned (set) wx.ImageList (if any);
        @param tabangle: the tab angle (only on standard tabs, from 0 to 15 degrees);
        @param enabled: sets enabled or disabled the tab.
        """

        self._strCaption = caption
        self._TabAngle = tabangle
        self._ImageIndex = imageindex
        self._bEnabled = enabled
        self._pos = wx.Point(-1, -1)
        self._size = wx.Size(-1, -1)
        self._region = wx.Region()
        self._xRect = wx.Rect()
        self._color = None


    def SetCaption(self, value):
        """ Sets the tab caption. """
        
        self._strCaption = value


    def GetCaption(self):
        """ Returns the tab caption. """

        return self._strCaption


    def SetPosition(self, value):
        """ Sets the tab position. """

        self._pos = value


    def GetPosition(self):
        """ Returns the tab position. """

        return self._pos


    def SetSize(self, value):
        """ Sets the tab size. """

        self._size = value


    def GetSize(self):
        """ Returns the tab size. """

        return self._size


    def SetTabAngle(self, value):
        """ Sets the tab header angle (0 <= tab <= 15 degrees). """

        self._TabAngle = min(45, value)


    def GetTabAngle(self):
        """ Returns the tab angle. """

        return self._TabAngle

    
    def SetImageIndex(self, value):
        """ Sets the tab image index. """

        self._ImageIndex = value


    def GetImageIndex(self):
        """ Returns the tab umage index. """

        return self._ImageIndex


    def GetEnabled(self):
        """ Returns whether the tab is enabled or not. """

        return self._bEnabled 


    def Enable(self, enabled):
        """ Sets the tab enabled or disabled. """

        self._bEnabled = enabled 


    def SetRegion(self, points=[]):
        """ Sets the tab region. """
        
        self._region = wx.RegionFromPoints(points) 


    def GetRegion(self):
        """ Returns the tab region. """

        return self._region  


    def SetXRect(self, xrect):
        """ Sets the button 'X' area rect. """

        self._xRect = xrect 


    def GetXRect(self):
        """ Returns the button 'X' area rect. """

        return self._xRect 


    def GetColour(self):
        """ Returns the tab colour. """

        return self._color 


    def SetColour(self, color):
        """ Sets the tab colour. """

        self._color = color 


# ---------------------------------------------------------------------------- #
# Class FlatNotebookEvent
# ---------------------------------------------------------------------------- #

class FlatNotebookEvent(wx.PyCommandEvent):
    """
    This events will be sent when a EVT_FLATNOTEBOOK_PAGE_CHANGED,
    EVT_FLATNOTEBOOK_PAGE_CHANGING, EVT_FLATNOTEBOOK_PAGE_CLOSING,
    EVT_FLATNOTEBOOK_PAGE_CLOSED and EVT_FLATNOTEBOOK_PAGE_CONTEXT_MENU is
    mapped in the parent.
    """
        
    def __init__(self, eventType, id=1, nSel=-1, nOldSel=-1):
        """ Default class constructor. """

        wx.PyCommandEvent.__init__(self, eventType, id)
        self._eventType = eventType

        self.notify = wx.NotifyEvent(eventType, id)


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


    def SetSelection(self, nSel):
        """ Sets event selection. """
        
        self._selection = nSel
        

    def SetOldSelection(self, nOldSel):
        """ Sets old event selection. """
        
        self._oldselection = nOldSel


    def GetSelection(self):
        """ Returns event selection. """
        
        return self._selection
        

    def GetOldSelection(self):
        """ Returns old event selection """
        
        return self._oldselection


# ---------------------------------------------------------------------------- #
# Class TabNavigatorWindow
# ---------------------------------------------------------------------------- #

class TabNavigatorWindow(wx.Dialog):
    """
    This class is used to create a modal dialog that enables "Smart Tabbing",
    similar to what you would get by hitting Alt+Tab on Windows.
    """

    def __init__(self, parent=None):
        """ Default class constructor. Used internally."""

        wx.Dialog.__init__(self, parent, wx.ID_ANY, "", style=0)

        self._selectedItem = -1
        self._indexMap = []
        
        self._bmp = GetMondrianBitmap()

        sz = wx.BoxSizer(wx.VERTICAL)
        
        self._listBox = wx.ListBox(self, wx.ID_ANY, wx.DefaultPosition, wx.Size(200, 150), [], wx.LB_SINGLE | wx.NO_BORDER)
        
        mem_dc = wx.MemoryDC()
        mem_dc.SelectObject(wx.EmptyBitmap(1,1))
        font = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        font.SetWeight(wx.BOLD)
        mem_dc.SetFont(font)

        panelHeight = mem_dc.GetCharHeight()
        panelHeight += 4 # Place a spacer of 2 pixels

        # Out signpost bitmap is 24 pixels
        if panelHeight < 24:
            panelHeight = 24
        
        self._panel = wx.Panel(self, wx.ID_ANY, wx.DefaultPosition, wx.Size(200, panelHeight))

        sz.Add(self._panel)
        sz.Add(self._listBox, 1, wx.EXPAND)
        
        self.SetSizer(sz)

        # Connect events to the list box
        self._listBox.Bind(wx.EVT_KEY_UP, self.OnKeyUp)
        self._listBox.Bind(wx.EVT_NAVIGATION_KEY, self.OnNavigationKey)
        self._listBox.Bind(wx.EVT_LISTBOX_DCLICK, self.OnItemSelected)
        
        # Connect paint event to the panel
        self._panel.Bind(wx.EVT_PAINT, self.OnPanelPaint)
        self._panel.Bind(wx.EVT_ERASE_BACKGROUND, self.OnPanelEraseBg)

        self.SetBackgroundColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE))
        self._listBox.SetBackgroundColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE))
        self.PopulateListControl(parent)
        
        self.GetSizer().Fit(self)
        self.GetSizer().SetSizeHints(self)
        self.GetSizer().Layout()
        self.Centre()


    def OnKeyUp(self, event):
        """Handles the wx.EVT_KEY_UP for the L{TabNavigatorWindow}."""
        
        if event.GetKeyCode() == wx.WXK_CONTROL:
            self.CloseDialog()


    def OnNavigationKey(self, event):
        """Handles the wx.EVT_NAVIGATION_KEY for the L{TabNavigatorWindow}. """

        selected = self._listBox.GetSelection()
        bk = self.GetParent()
        maxItems = bk.GetPageCount()
            
        if event.GetDirection():
        
            # Select next page
            if selected == maxItems - 1:
                itemToSelect = 0
            else:
                itemToSelect = selected + 1
        
        else:
        
            # Previous page
            if selected == 0:
                itemToSelect = maxItems - 1
            else:
                itemToSelect = selected - 1
        
        self._listBox.SetSelection(itemToSelect)


    def PopulateListControl(self, book):
        """Populates the L{TabNavigatorWindow} listbox with a list of tabs."""

        selection = book.GetSelection()
        count = book.GetPageCount()
        
        self._listBox.Append(book.GetPageText(selection))
        self._indexMap.append(selection)
        
        prevSel = book.GetPreviousSelection()
        
        if prevSel != wx.NOT_FOUND:
        
            # Insert the previous selection as second entry 
            self._listBox.Append(book.GetPageText(prevSel))
            self._indexMap.append(prevSel)
        
        for c in xrange(count):
        
            # Skip selected page
            if c == selection:
                continue

            # Skip previous selected page as well
            if c == prevSel:
                continue

            self._listBox.Append(book.GetPageText(c))
            self._indexMap.append(c)

        # Select the next entry after the current selection
        self._listBox.SetSelection(0)
        dummy = wx.NavigationKeyEvent()
        dummy.SetDirection(True)
        self.OnNavigationKey(dummy)


    def OnItemSelected(self, event):
        """Handles the wx.EVT_LISTBOX_DCLICK event for the wx.ListBox inside L{TabNavigatorWindow}. """

    	self.CloseDialog()


    def CloseDialog(self):
        """Closes the L{TabNavigatorWindow} dialog, setting selection in L{FlatNotebook}."""

        bk = self.GetParent()
        self._selectedItem = self._listBox.GetSelection()
        iter = self._indexMap[self._selectedItem]
        bk._pages.FireEvent(iter)
        self.EndModal(wx.ID_OK)
        

    def OnPanelPaint(self, event):
        """Handles the wx.EVT_PAINT event for L{TabNavigatorWindow} top panel. """

        dc = wx.PaintDC(self._panel)
        rect = self._panel.GetClientRect()

        bmp = wx.EmptyBitmap(rect.width, rect.height)

        mem_dc = wx.MemoryDC()
        mem_dc.SelectObject(bmp)

        endColour = wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW)
        startColour = LightColour(endColour, 50)
        PaintStraightGradientBox(mem_dc, rect, startColour, endColour)

        # Draw the caption title and place the bitmap
        # get the bitmap optimal position, and draw it
        bmpPt, txtPt = wx.Point(), wx.Point()
        bmpPt.y = (rect.height - self._bmp.GetHeight())/2
        bmpPt.x = 3
        mem_dc.DrawBitmap(self._bmp, bmpPt.x, bmpPt.y, True)

        # get the text position, and draw it
        font = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        font.SetWeight(wx.BOLD)
        mem_dc.SetFont(font)
        fontHeight = mem_dc.GetCharHeight()
        
        txtPt.x = bmpPt.x + self._bmp.GetWidth() + 4
        txtPt.y = (rect.height - fontHeight)/2
        mem_dc.SetTextForeground(wx.WHITE)
        mem_dc.DrawText("Opened tabs:", txtPt.x, txtPt.y)
        mem_dc.SelectObject(wx.NullBitmap)
        
        dc.DrawBitmap(bmp, 0, 0)


    def OnPanelEraseBg(self, event):
        """Handles the wx.EVT_ERASE_BACKGROUND event for L{TabNavigatorWindow} top panel. """

        pass


# ---------------------------------------------------------------------------- #
# Class FNBRenderer
# ---------------------------------------------------------------------------- #

class FNBRenderer:
    """
    Parent class for the 4 renderers defined: I{Standard}, I{VC71}, I{Fancy}
    and I{VC8}. This class implements the common methods of all 4 renderers.
    @undocumented: _GetBitmap*
    """

    def __init__(self):
        """Default class constructor. """
        
        self._tabXBgBmp = wx.EmptyBitmap(16, 16)
        self._xBgBmp = wx.EmptyBitmap(16, 14)
        self._leftBgBmp = wx.EmptyBitmap(16, 14)
        self._rightBgBmp = wx.EmptyBitmap(16, 14)
        self._tabHeight = None


    def GetLeftButtonPos(self, pageContainer):
        """ Returns the left button position in the navigation area. """

        pc = pageContainer
        style = pc.GetParent().GetWindowStyleFlag()
        rect = pc.GetClientRect()
        clientWidth = rect.width
        
        if style & FNB_NO_X_BUTTON:
            return clientWidth - 38
        else:
            return clientWidth - 54



    def GetRightButtonPos(self, pageContainer):
        """ Returns the right button position in the navigation area. """

        pc = pageContainer
        style = pc.GetParent().GetWindowStyleFlag()
        rect = pc.GetClientRect()
        clientWidth = rect.width
        
        if style & FNB_NO_X_BUTTON:
            return clientWidth - 22
        else:
            return clientWidth - 38


    def GetDropArrowButtonPos(self, pageContainer):
        """ Returns the drop down button position in the navigation area. """

        return self.GetRightButtonPos(pageContainer)


    def GetXPos(self, pageContainer):
        """ Returns the 'X' button position in the navigation area. """

        pc = pageContainer
        style = pc.GetParent().GetWindowStyleFlag()
        rect = pc.GetClientRect()
        clientWidth = rect.width
        
        if style & FNB_NO_X_BUTTON:
            return clientWidth
        else:
            return clientWidth - 22


    def GetButtonsAreaLength(self, pageContainer):
        """ Returns the navigation area width. """

        pc = pageContainer
        style = pc.GetParent().GetWindowStyleFlag()

        # ''
        if style & FNB_NO_NAV_BUTTONS and style & FNB_NO_X_BUTTON and not style & FNB_DROPDOWN_TABS_LIST:
            return 0

        # 'x'        
        elif style & FNB_NO_NAV_BUTTONS and not style & FNB_NO_X_BUTTON and not style & FNB_DROPDOWN_TABS_LIST:
            return 22
        
        # '<>'
        if not style & FNB_NO_NAV_BUTTONS and style & FNB_NO_X_BUTTON and not style & FNB_DROPDOWN_TABS_LIST:
            return 53 - 16
        
        # 'vx'
        if style & FNB_DROPDOWN_TABS_LIST and not style & FNB_NO_X_BUTTON:
            return 22 + 16

        # 'v'
        if style & FNB_DROPDOWN_TABS_LIST and style & FNB_NO_X_BUTTON:
            return 22

        # '<>x'
        return 53


    def DrawLeftArrow(self, pageContainer, dc):
        """ Draw the left navigation arrow. """

        pc = pageContainer
        
        style = pc.GetParent().GetWindowStyleFlag()
        if style & FNB_NO_NAV_BUTTONS:
            return

        # Make sure that there are pages in the container
        if not pc._pagesInfoVec:
            return

        # Set the bitmap according to the button status
        if pc._nLeftButtonStatus == FNB_BTN_HOVER:
            arrowBmp = wx.BitmapFromXPMData(left_arrow_hilite_xpm)
        elif pc._nLeftButtonStatus == FNB_BTN_PRESSED:
            arrowBmp = wx.BitmapFromXPMData(left_arrow_pressed_xpm)
        else:
            arrowBmp = wx.BitmapFromXPMData(left_arrow_xpm)

        if pc._nFrom == 0:
            # Handle disabled arrow
            arrowBmp = wx.BitmapFromXPMData(left_arrow_disabled_xpm)
        
        arrowBmp.SetMask(wx.Mask(arrowBmp, MASK_COLOR))

        # Erase old bitmap
        posx = self.GetLeftButtonPos(pc)
        dc.DrawBitmap(self._leftBgBmp, posx, 6)

        # Draw the new bitmap
        dc.DrawBitmap(arrowBmp, posx, 6, True)


    def DrawRightArrow(self, pageContainer, dc):
        """ Draw the right navigation arrow. """

        pc = pageContainer
        
        style = pc.GetParent().GetWindowStyleFlag()
        if style & FNB_NO_NAV_BUTTONS:
            return

        # Make sure that there are pages in the container
        if not pc._pagesInfoVec:
            return

        # Set the bitmap according to the button status
        if pc._nRightButtonStatus == FNB_BTN_HOVER:        
            arrowBmp = wx.BitmapFromXPMData(right_arrow_hilite_xpm)
        elif pc._nRightButtonStatus == FNB_BTN_PRESSED:
            arrowBmp = wx.BitmapFromXPMData(right_arrow_pressed_xpm)
        else:
            arrowBmp = wx.BitmapFromXPMData(right_arrow_xpm)

        # Check if the right most tab is visible, if it is
        # don't rotate right anymore
        if pc._pagesInfoVec[-1].GetPosition() != wx.Point(-1, -1):
            arrowBmp = wx.BitmapFromXPMData(right_arrow_disabled_xpm)
        
        arrowBmp.SetMask(wx.Mask(arrowBmp, MASK_COLOR))

        # erase old bitmap
        posx = self.GetRightButtonPos(pc)
        dc.DrawBitmap(self._rightBgBmp, posx, 6)

        # Draw the new bitmap
        dc.DrawBitmap(arrowBmp, posx, 6, True)


    def DrawDropDownArrow(self, pageContainer, dc):
        """ Draws the drop-down arrow in the navigation area. """

        pc = pageContainer
        
        # Check if this style is enabled
        style = pc.GetParent().GetWindowStyleFlag()
        if not style & FNB_DROPDOWN_TABS_LIST:
            return

        # Make sure that there are pages in the container
        if not pc._pagesInfoVec:
            return

        if pc._nArrowDownButtonStatus == FNB_BTN_HOVER:
            downBmp = wx.BitmapFromXPMData(down_arrow_hilite_xpm)
        elif pc._nArrowDownButtonStatus == FNB_BTN_PRESSED:
            downBmp = wx.BitmapFromXPMData(down_arrow_pressed_xpm)
        else:
            downBmp = wx.BitmapFromXPMData(down_arrow_xpm)

        downBmp.SetMask(wx.Mask(downBmp, MASK_COLOR))

        # erase old bitmap
        posx = self.GetDropArrowButtonPos(pc)
        dc.DrawBitmap(self._xBgBmp, posx, 6)

        # Draw the new bitmap
        dc.DrawBitmap(downBmp, posx, 6, True)


    def DrawX(self, pageContainer, dc):
        """ Draw the 'X' navigation button in the navigation area. """

        pc = pageContainer
        
        # Check if this style is enabled
        style = pc.GetParent().GetWindowStyleFlag()
        if style & FNB_NO_X_BUTTON:
            return

        # Make sure that there are pages in the container
        if not pc._pagesInfoVec:
            return

        # Set the bitmap according to the button status
        if pc._nXButtonStatus == FNB_BTN_HOVER:
            xbmp = wx.BitmapFromXPMData(x_button_hilite_xpm)
        elif pc._nXButtonStatus == FNB_BTN_PRESSED:
            xbmp = wx.BitmapFromXPMData(x_button_pressed_xpm)
        else:
            xbmp = wx.BitmapFromXPMData(x_button_xpm)

        xbmp.SetMask(wx.Mask(xbmp, MASK_COLOR))
        
        # erase old bitmap
        posx = self.GetXPos(pc) 
        dc.DrawBitmap(self._xBgBmp, posx, 6)

        # Draw the new bitmap
        dc.DrawBitmap(xbmp, posx, 6, True)


    def DrawTabX(self, pageContainer, dc, rect, tabIdx, btnStatus):
        """ Draws the 'X' in the selected tab. """

        pc = pageContainer
        if not pc.HasFlag(FNB_X_ON_TAB):
            return

        # We draw the 'x' on the active tab only
        if tabIdx != pc.GetSelection() or tabIdx < 0:
            return

        # Set the bitmap according to the button status
        
        if btnStatus == FNB_BTN_HOVER:
            xBmp = wx.BitmapFromXPMData(x_button_hilite_xpm)
        elif btnStatus == FNB_BTN_PRESSED:
            xBmp = wx.BitmapFromXPMData(x_button_pressed_xpm)
        else:
            xBmp = wx.BitmapFromXPMData(x_button_xpm)

        # Set the masking
        xBmp.SetMask(wx.Mask(xBmp, MASK_COLOR))

        # erase old button
        dc.DrawBitmap(self._tabXBgBmp, rect.x, rect.y)

        # Draw the new bitmap
        dc.DrawBitmap(xBmp, rect.x, rect.y, True)

        # Update the vector
        rr = wx.Rect(rect.x, rect.y, 14, 13)
        pc._pagesInfoVec[tabIdx].SetXRect(rr)


    def _GetBitmap(self, dc, rect, bmp):

        mem_dc = wx.MemoryDC()
        mem_dc.SelectObject(bmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)
        return bmp


    def DrawTabsLine(self, pageContainer, dc):
        """ Draws a line over the tabs. """

        pc = pageContainer
        
        clntRect = pc.GetClientRect()
        clientRect3 = wx.Rect(0, 0, clntRect.width, clntRect.height)

        if pc.HasFlag(FNB_BOTTOM):
        
            clientRect = wx.Rect(0, 2, clntRect.width, clntRect.height - 2)
            clientRect2 = wx.Rect(0, 1, clntRect.width, clntRect.height - 1)
        
        else:
        
            clientRect = wx.Rect(0, 0, clntRect.width, clntRect.height - 2)
            clientRect2 = wx.Rect(0, 0, clntRect.width, clntRect.height - 1)
        
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.SetPen(wx.Pen(pc.GetSingleLineBorderColour()))
        dc.DrawRectangleRect(clientRect2)
        dc.DrawRectangleRect(clientRect3)

        dc.SetPen(wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW)))
        dc.DrawRectangleRect(clientRect)

        if not pc.HasFlag(FNB_TABS_BORDER_SIMPLE):
        
            dc.SetPen(wx.Pen((pc.HasFlag(FNB_VC71) and [wx.Colour(247, 243, 233)] or [pc._tabAreaColor])[0]))
            dc.DrawLine(0, 0, 0, clientRect.height+1)
            
            if pc.HasFlag(FNB_BOTTOM):
            
                dc.DrawLine(0, clientRect.height+1, clientRect.width, clientRect.height+1)
            
            else:
                
                dc.DrawLine(0, 0, clientRect.width, 0)
                
            dc.DrawLine(clientRect.width - 1, 0, clientRect.width - 1, clientRect.height+1)


    def CalcTabWidth(self, pageContainer, tabIdx, tabHeight):
        """ Calculates the width of the input tab. """

        pc = pageContainer
        dc = wx.MemoryDC()
        dc.SelectObject(wx.EmptyBitmap(1,1))

        boldFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        boldFont.SetWeight(wx.FONTWEIGHT_BOLD)

        if pc.IsDefaultTabs():
            shapePoints = int(tabHeight*math.tan(float(pc._pagesInfoVec[tabIdx].GetTabAngle())/180.0*math.pi))

        # Calculate the text length using the bold font, so when selecting a tab
        # its width will not change
        dc.SetFont(boldFont)
        width, pom = dc.GetTextExtent(pc.GetPageText(tabIdx))

        # Set a minimum size to a tab
        if width < 20:
            width = 20

        tabWidth = 2*pc._pParent.GetPadding() + width

        # Style to add a small 'x' button on the top right
        # of the tab
        if pc.HasFlag(FNB_X_ON_TAB) and tabIdx == pc.GetSelection():
            # The xpm image that contains the 'x' button is 9 pixels
            spacer = 9
            if pc.HasFlag(FNB_VC8):
                spacer = 4

            tabWidth += pc._pParent.GetPadding() + spacer
        
        if pc.IsDefaultTabs():
            # Default style
            tabWidth += 2*shapePoints

        hasImage = pc._ImageList != None and pc._pagesInfoVec[tabIdx].GetImageIndex() != -1

        # For VC71 style, we only add the icon size (16 pixels)
        if hasImage:
        
            if not pc.IsDefaultTabs():
                tabWidth += 16 + pc._pParent.GetPadding()
            else:
                # Default style
                tabWidth += 16 + pc._pParent.GetPadding() + shapePoints/2
        
        return tabWidth


    def CalcTabHeight(self, pageContainer):
        """ Calculates the height of the input tab. """

        if self._tabHeight:
            return self._tabHeight

        pc = pageContainer
        dc = wx.MemoryDC()
        dc.SelectObject(wx.EmptyBitmap(1,1))

        # For GTK it seems that we must do this steps in order
        # for the tabs will get the proper height on initialization
        # on MSW, preforming these steps yields wierd results
        normalFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        boldFont = normalFont

        if "__WXGTK__" in wx.PlatformInfo:
            boldFont.SetWeight(wx.FONTWEIGHT_BOLD)
            dc.SetFont(boldFont)

        height = dc.GetCharHeight()
        
        tabHeight = height + FNB_HEIGHT_SPACER # We use 8 pixels as padding
        if "__WXGTK__" in wx.PlatformInfo:
            # On GTK the tabs are should be larger
            tabHeight += 6

        self._tabHeight = tabHeight
        
        return tabHeight


    def DrawTabs(self, pageContainer, dc):
        """ Actually draws the tabs in L{FlatNotebook}."""

        pc = pageContainer
        if "__WXMAC__" in wx.PlatformInfo:
            # Works well on MSW & GTK, however this lines should be skipped on MAC
            if not pc._pagesInfoVec or pc._nFrom >= len(pc._pagesInfoVec):
                pc.Hide()
                return
            
        # Get the text hight
        tabHeight = self.CalcTabHeight(pageContainer)
        style = pc.GetParent().GetWindowStyleFlag()

        # Calculate the number of rows required for drawing the tabs
        rect = pc.GetClientRect()
        clientWidth = rect.width

        # Set the maximum client size
        pc.SetSizeHints(self.GetButtonsAreaLength(pc), tabHeight)
        borderPen = wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW))

        if style & FNB_VC71:
            backBrush = wx.Brush(wx.Colour(247, 243, 233))
        else:
            backBrush = wx.Brush(pc._tabAreaColor)

        noselBrush = wx.Brush(wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNFACE))
        selBrush = wx.Brush(pc._activeTabColor)

        size = pc.GetSize()

        # Background
        dc.SetTextBackground((style & FNB_VC71 and [wx.Colour(247, 243, 233)] or [pc.GetBackgroundColour()])[0])
        dc.SetTextForeground(pc._activeTextColor)
        dc.SetBrush(backBrush)

        # If border style is set, set the pen to be border pen
        if pc.HasFlag(FNB_TABS_BORDER_SIMPLE):
            dc.SetPen(borderPen)
        else:
            colr = (pc.HasFlag(FNB_VC71) and [wx.Colour(247, 243, 233)] or [pc.GetBackgroundColour()])[0]
            dc.SetPen(wx.Pen(colr))
        
        dc.DrawRectangle(0, 0, size.x, size.y)

        # Take 3 bitmaps for the background for the buttons
        
        mem_dc = wx.MemoryDC()
        #---------------------------------------
        # X button
        #---------------------------------------
        rect = wx.Rect(self.GetXPos(pc), 6, 16, 14)
        mem_dc.SelectObject(self._xBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)

        #---------------------------------------
        # Right button
        #---------------------------------------
        rect = wx.Rect(self.GetRightButtonPos(pc), 6, 16, 14)
        mem_dc.SelectObject(self._rightBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)

        #---------------------------------------
        # Left button
        #---------------------------------------
        rect = wx.Rect(self.GetLeftButtonPos(pc), 6, 16, 14)
        mem_dc.SelectObject(self._leftBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)

        # We always draw the bottom/upper line of the tabs
        # regradless the style
        dc.SetPen(borderPen)
        self.DrawTabsLine(pc, dc)

        # Restore the pen
        dc.SetPen(borderPen)

        if pc.HasFlag(FNB_VC71):
        
            greyLineYVal  = (pc.HasFlag(FNB_BOTTOM) and [0] or [size.y - 2])[0]
            whiteLineYVal = (pc.HasFlag(FNB_BOTTOM) and [3] or [size.y - 3])[0]

            pen = wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE))
            dc.SetPen(pen)

            # Draw thik grey line between the windows area and
            # the tab area
            for num in xrange(3):
                dc.DrawLine(0, greyLineYVal + num, size.x, greyLineYVal + num)

            wbPen = (pc.HasFlag(FNB_BOTTOM) and [wx.BLACK_PEN] or [wx.WHITE_PEN])[0]
            dc.SetPen(wbPen)
            dc.DrawLine(1, whiteLineYVal, size.x - 1, whiteLineYVal)

            # Restore the pen
            dc.SetPen(borderPen)
        
        # Draw labels
        normalFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        boldFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        boldFont.SetWeight(wx.FONTWEIGHT_BOLD)
        dc.SetFont(boldFont)

        posx = pc._pParent.GetPadding()

        # Update all the tabs from 0 to 'pc._nFrom' to be non visible
        for i in xrange(pc._nFrom):
        
            pc._pagesInfoVec[i].SetPosition(wx.Point(-1, -1))
            pc._pagesInfoVec[i].GetRegion().Clear()

        count = pc._nFrom
        
        #----------------------------------------------------------
        # Go over and draw the visible tabs
        #----------------------------------------------------------
        for i in xrange(pc._nFrom, len(pc._pagesInfoVec)):
        
            dc.SetPen(borderPen)
            dc.SetBrush((i==pc.GetSelection() and [selBrush] or [noselBrush])[0])

            # Now set the font to the correct font
            dc.SetFont((i==pc.GetSelection() and [boldFont] or [normalFont])[0])

            # Add the padding to the tab width
            # Tab width:
            # +-----------------------------------------------------------+
            # | PADDING | IMG | IMG_PADDING | TEXT | PADDING | x |PADDING |
            # +-----------------------------------------------------------+
            tabWidth = self.CalcTabWidth(pageContainer, i, tabHeight)

            # Check if we can draw more
            if posx + tabWidth + self.GetButtonsAreaLength(pc) >= clientWidth:
                break

            count = count + 1
            
            # By default we clean the tab region
            pc._pagesInfoVec[i].GetRegion().Clear()

            # Clean the 'x' buttn on the tab.
            # A 'Clean' rectangle, is a rectangle with width or height
            # with values lower than or equal to 0
            pc._pagesInfoVec[i].GetXRect().SetSize(wx.Size(-1, -1))

            # Draw the tab (border, text, image & 'x' on tab)
            self.DrawTab(pc, dc, posx, i, tabWidth, tabHeight, pc._nTabXButtonStatus)

            # Restore the text forground
            dc.SetTextForeground(pc._activeTextColor)

            # Update the tab position & size
            posy = (pc.HasFlag(FNB_BOTTOM) and [0] or [VERTICAL_BORDER_PADDING])[0]

            pc._pagesInfoVec[i].SetPosition(wx.Point(posx, posy))
            pc._pagesInfoVec[i].SetSize(wx.Size(tabWidth, tabHeight))
            posx += tabWidth
        
        # Update all tabs that can not fit into the screen as non-visible
        for i in xrange(count, len(pc._pagesInfoVec)):
            pc._pagesInfoVec[i].SetPosition(wx.Point(-1, -1))
            pc._pagesInfoVec[i].GetRegion().Clear()
        
        # Draw the left/right/close buttons
        # Left arrow
        self.DrawLeftArrow(pc, dc)
        self.DrawRightArrow(pc, dc)
        self.DrawX(pc, dc)
        self.DrawDropDownArrow(pc, dc)


    def DrawDragHint(self, pc, tabIdx):
        """
        Draws tab drag hint, the default implementation is to do nothing.
        You can override this function to provide a nice feedback to user.
        """
        
        pass


# ---------------------------------------------------------------------------- #
# Class FNBRendererMgr
# A manager that handles all the renderers defined below and calls the
# appropriate one when drawing is needed
# ---------------------------------------------------------------------------- #

class FNBRendererMgr:
    """
    This class represents a manager that handles all the 4 renderers defined
    and calls the appropriate one when drawing is needed.
    """

    def __init__(self):
        """ Default class constructor. """
        
        # register renderers

        self._renderers = {}        
        self._renderers.update({-1: FNBRendererDefault()})
        self._renderers.update({FNB_VC71: FNBRendererVC71()})
        self._renderers.update({FNB_FANCY_TABS: FNBRendererFancy()})
        self._renderers.update({FNB_VC8: FNBRendererVC8()})


    def GetRenderer(self, style):
        """ Returns the current renderer based on the style selected. """

        # since we dont have a style for default tabs, we 
        # test for all others - FIXME: add style for default tabs
        if not style & FNB_VC71 and not style & FNB_VC8 and not style & FNB_FANCY_TABS:
            return self._renderers[-1]

        if style & FNB_VC71:
            return self._renderers[FNB_VC71]

        if style & FNB_FANCY_TABS:
            return self._renderers[FNB_FANCY_TABS]

        if style & FNB_VC8:
            return self._renderers[FNB_VC8]

        # the default is to return the default renderer
        return self._renderers[-1]


#------------------------------------------
# Default renderer 
#------------------------------------------

class FNBRendererDefault(FNBRenderer):
    """
    This class handles the drawing of tabs using the I{Standard} renderer.
    """
    
    def __init__(self):
        """ Default class constructor. """

        FNBRenderer.__init__(self)
        

    def DrawTab(self, pageContainer, dc, posx, tabIdx, tabWidth, tabHeight, btnStatus):
        """ Draws a tab using the I{Standard} style. """

        # Default style
        borderPen = wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW))
        pc = pageContainer 

        tabPoints = [wx.Point() for ii in xrange(7)]
        tabPoints[0].x = posx
        tabPoints[0].y = (pc.HasFlag(FNB_BOTTOM) and [2] or [tabHeight - 2])[0]

        tabPoints[1].x = int(posx+(tabHeight-2)*math.tan(float(pc._pagesInfoVec[tabIdx].GetTabAngle())/180.0*math.pi))
        tabPoints[1].y = (pc.HasFlag(FNB_BOTTOM) and [tabHeight - (VERTICAL_BORDER_PADDING+2)] or [(VERTICAL_BORDER_PADDING+2)])[0]

        tabPoints[2].x = tabPoints[1].x+2
        tabPoints[2].y = (pc.HasFlag(FNB_BOTTOM) and [tabHeight - VERTICAL_BORDER_PADDING] or [VERTICAL_BORDER_PADDING])[0]

        tabPoints[3].x = int(posx+tabWidth-(tabHeight-2)*math.tan(float(pc._pagesInfoVec[tabIdx].GetTabAngle())/180.0*math.pi))-2
        tabPoints[3].y = (pc.HasFlag(FNB_BOTTOM) and [tabHeight - VERTICAL_BORDER_PADDING] or [VERTICAL_BORDER_PADDING])[0]

        tabPoints[4].x = tabPoints[3].x+2
        tabPoints[4].y = (pc.HasFlag(FNB_BOTTOM) and [tabHeight - (VERTICAL_BORDER_PADDING+2)] or [(VERTICAL_BORDER_PADDING+2)])[0]

        tabPoints[5].x = int(tabPoints[4].x+(tabHeight-2)*math.tan(float(pc._pagesInfoVec[tabIdx].GetTabAngle())/180.0*math.pi))
        tabPoints[5].y = (pc.HasFlag(FNB_BOTTOM) and [2] or [tabHeight - 2])[0]

        tabPoints[6].x = tabPoints[0].x
        tabPoints[6].y = tabPoints[0].y
        
        if tabIdx == pc.GetSelection():
        
            # Draw the tab as rounded rectangle
            dc.DrawPolygon(tabPoints)
        
        else:
        
            if tabIdx != pc.GetSelection() - 1:
            
                # Draw a vertical line to the right of the text
                pt1x = tabPoints[5].x
                pt1y = (pc.HasFlag(FNB_BOTTOM) and [4] or [tabHeight - 6])[0]
                pt2x = tabPoints[5].x
                pt2y = (pc.HasFlag(FNB_BOTTOM) and [tabHeight - 4] or [4])[0]
                dc.DrawLine(pt1x, pt1y, pt2x, pt2y)

        if tabIdx == pc.GetSelection():
        
            savePen = dc.GetPen()
            whitePen = wx.Pen(wx.WHITE)
            whitePen.SetWidth(1)
            dc.SetPen(whitePen)

            secPt = wx.Point(tabPoints[5].x + 1, tabPoints[5].y)
            dc.DrawLine(tabPoints[0].x, tabPoints[0].y, secPt.x, secPt.y)

            # Restore the pen
            dc.SetPen(savePen)
        
        # -----------------------------------
        # Text and image drawing
        # -----------------------------------

        # Text drawing offset from the left border of the
        # rectangle
        
        # The width of the images are 16 pixels
        padding = pc.GetParent().GetPadding()
        shapePoints = int(tabHeight*math.tan(float(pc._pagesInfoVec[tabIdx].GetTabAngle())/180.0*math.pi))
        hasImage = pc._pagesInfoVec[tabIdx].GetImageIndex() != -1
        imageYCoord = (pc.HasFlag(FNB_BOTTOM) and [6] or [8])[0]

        if hasImage:
            textOffset = 2*pc._pParent._nPadding + 16 + shapePoints/2 
        else:
            textOffset = pc._pParent._nPadding + shapePoints/2 

        textOffset += 2

        if tabIdx != pc.GetSelection():
        
            # Set the text background to be like the vertical lines
            dc.SetTextForeground(pc._pParent.GetNonActiveTabTextColour())
        
        if hasImage:
        
            imageXOffset = textOffset - 16 - padding
            pc._ImageList.Draw(pc._pagesInfoVec[tabIdx].GetImageIndex(), dc,
                                     posx + imageXOffset, imageYCoord,
                                     wx.IMAGELIST_DRAW_TRANSPARENT, True)
        
        dc.DrawText(pc.GetPageText(tabIdx), posx + textOffset, imageYCoord)

        # draw 'x' on tab (if enabled)
        if pc.HasFlag(FNB_X_ON_TAB) and tabIdx == pc.GetSelection():
        
            textWidth, textHeight = dc.GetTextExtent(pc.GetPageText(tabIdx))
            tabCloseButtonXCoord = posx + textOffset + textWidth + 1

            # take a bitmap from the position of the 'x' button (the x on tab button)
            # this bitmap will be used later to delete old buttons
            tabCloseButtonYCoord = imageYCoord
            x_rect = wx.Rect(tabCloseButtonXCoord, tabCloseButtonYCoord, 16, 16)
            self._tabXBgBmp = self._GetBitmap(dc, x_rect, self._tabXBgBmp)

            # Draw the tab
            self.DrawTabX(pc, dc, x_rect, tabIdx, btnStatus)            
        

#------------------------------------------------------------------
# Visual studio 7.1 
#------------------------------------------------------------------

class FNBRendererVC71(FNBRenderer):
    """
    This class handles the drawing of tabs using the I{VC71} renderer.
    """

    def __init__(self):
        """ Default class constructor. """

        FNBRenderer.__init__(self)


    def DrawTab(self, pageContainer, dc, posx, tabIdx, tabWidth, tabHeight, btnStatus):
        """ Draws a tab using the I{VC71} style. """

        # Visual studio 7.1 style
        borderPen = wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW))
        pc = pageContainer

        dc.SetPen((tabIdx == pc.GetSelection() and [wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE))] or [borderPen])[0])
        dc.SetBrush((tabIdx == pc.GetSelection() and [wx.Brush(wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE))] or [wx.Brush(wx.Colour(247, 243, 233))])[0])

        if tabIdx == pc.GetSelection():
        
            posy = (pc.HasFlag(FNB_BOTTOM) and [0] or [VERTICAL_BORDER_PADDING])[0]
            tabH = (pc.HasFlag(FNB_BOTTOM) and [tabHeight - 5] or [tabHeight - 3])[0]
            dc.DrawRectangle(posx, posy, tabWidth, tabH) 

            # Draw a black line on the left side of the
            # rectangle
            dc.SetPen(wx.BLACK_PEN)

            blackLineY1 = VERTICAL_BORDER_PADDING
            blackLineY2 = tabH
            dc.DrawLine(posx + tabWidth, blackLineY1, posx + tabWidth, blackLineY2)

            # To give the tab more 3D look we do the following
            # Incase the tab is on top,
            # Draw a thik white line on topof the rectangle
            # Otherwise, draw a thin (1 pixel) black line at the bottom

            pen = wx.Pen((pc.HasFlag(FNB_BOTTOM) and [wx.BLACK] or [wx.WHITE])[0])
            dc.SetPen(pen)
            whiteLinePosY = (pc.HasFlag(FNB_BOTTOM) and [blackLineY2] or [VERTICAL_BORDER_PADDING ])[0]
            dc.DrawLine(posx , whiteLinePosY, posx + tabWidth + 1, whiteLinePosY)

            # Draw a white vertical line to the left of the tab
            dc.SetPen(wx.WHITE_PEN)
            if not pc.HasFlag(FNB_BOTTOM):
                blackLineY2 += 1
                
            dc.DrawLine(posx, blackLineY1, posx, blackLineY2)
        
        else:
        
            # We dont draw a rectangle for non selected tabs, but only
            # vertical line on the left

            blackLineY1 = (pc.HasFlag(FNB_BOTTOM) and [VERTICAL_BORDER_PADDING + 2] or [VERTICAL_BORDER_PADDING + 1])[0]
            blackLineY2 = pc.GetSize().y - 5 
            dc.DrawLine(posx + tabWidth, blackLineY1, posx + tabWidth, blackLineY2)
        
        # -----------------------------------
        # Text and image drawing
        # -----------------------------------

        # Text drawing offset from the left border of the
        # rectangle
        
        # The width of the images are 16 pixels
        padding = pc.GetParent().GetPadding()
        hasImage = pc._pagesInfoVec[tabIdx].GetImageIndex() != -1
        imageYCoord = (pc.HasFlag(FNB_BOTTOM) and [5] or [8])[0]

        if hasImage:
            textOffset = 2*pc._pParent._nPadding + 16
        else:
            textOffset = pc._pParent._nPadding

        if tabIdx != pc.GetSelection():
        
            # Set the text background to be like the vertical lines
            dc.SetTextForeground(pc._pParent.GetNonActiveTabTextColour())
        
        if hasImage:
        
            imageXOffset = textOffset - 16 - padding
            pc._ImageList.Draw(pc._pagesInfoVec[tabIdx].GetImageIndex(), dc,
                                     posx + imageXOffset, imageYCoord,
                                     wx.IMAGELIST_DRAW_TRANSPARENT, True)
        
        dc.DrawText(pc.GetPageText(tabIdx), posx + textOffset, imageYCoord)
        
        # draw 'x' on tab (if enabled)
        if pc.HasFlag(FNB_X_ON_TAB) and tabIdx == pc.GetSelection():
        
            textWidth, textHeight = dc.GetTextExtent(pc.GetPageText(tabIdx))
            tabCloseButtonXCoord = posx + textOffset + textWidth + 1

            # take a bitmap from the position of the 'x' button (the x on tab button)
            # this bitmap will be used later to delete old buttons
            tabCloseButtonYCoord = imageYCoord
            x_rect = wx.Rect(tabCloseButtonXCoord, tabCloseButtonYCoord, 16, 16)
            self._tabXBgBmp = self._GetBitmap(dc, x_rect, self._tabXBgBmp)

            # Draw the tab
            self.DrawTabX(pc, dc, x_rect, tabIdx, btnStatus)                    


#------------------------------------------------------------------
# Fancy style
#------------------------------------------------------------------

class FNBRendererFancy(FNBRenderer):
    """
    This class handles the drawing of tabs using the I{Fancy} renderer.
    """

    def __init__(self):
        """ Default class constructor. """

        FNBRenderer.__init__(self)


    def DrawTab(self, pageContainer, dc, posx, tabIdx, tabWidth, tabHeight, btnStatus):
        """ Draws a tab using the I{Fancy} style, similar to VC71 but with gradients. """

        # Fancy tabs - like with VC71 but with the following differences:
        # - The Selected tab is colored with gradient color
        borderPen = wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW))
        pc = pageContainer

        pen = (tabIdx == pc.GetSelection() and [wx.Pen(pc._pParent.GetBorderColour())] or [wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE))])[0]

        if tabIdx == pc.GetSelection():
        
            posy = (pc.HasFlag(FNB_BOTTOM) and [2] or [VERTICAL_BORDER_PADDING])[0]
            th = tabHeight - 5

            rect = wx.Rect(posx, posy, tabWidth, th)

            col2 = (pc.HasFlag(FNB_BOTTOM) and [pc._pParent.GetGradientColourTo()] or [pc._pParent.GetGradientColourFrom()])[0]
            col1 = (pc.HasFlag(FNB_BOTTOM) and [pc._pParent.GetGradientColourFrom()] or [pc._pParent.GetGradientColourTo()])[0]

            PaintStraightGradientBox(dc, rect, col1, col2)
            dc.SetBrush(wx.TRANSPARENT_BRUSH)
            dc.SetPen(pen)
            dc.DrawRectangleRect(rect)

            # erase the bottom/top line of the rectangle
            dc.SetPen(wx.Pen(pc._pParent.GetGradientColourFrom()))
            if pc.HasFlag(FNB_BOTTOM):
                dc.DrawLine(rect.x, 2, rect.x + rect.width, 2)
            else:
                dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, rect.y + rect.height - 1)
        
        else:
        
            # We dont draw a rectangle for non selected tabs, but only
            # vertical line on the left
            dc.SetPen(borderPen)
            dc.DrawLine(posx + tabWidth, VERTICAL_BORDER_PADDING + 3, posx + tabWidth, tabHeight - 4)
        

        # -----------------------------------
        # Text and image drawing
        # -----------------------------------

        # Text drawing offset from the left border of the
        # rectangle
        
        # The width of the images are 16 pixels
        padding = pc.GetParent().GetPadding()
        hasImage = pc._pagesInfoVec[tabIdx].GetImageIndex() != -1
        imageYCoord = (pc.HasFlag(FNB_BOTTOM) and [6] or [8])[0]

        if hasImage:
            textOffset = 2*pc._pParent._nPadding + 16
        else:
            textOffset = pc._pParent._nPadding 

        textOffset += 2

        if tabIdx != pc.GetSelection():
        
            # Set the text background to be like the vertical lines
            dc.SetTextForeground(pc._pParent.GetNonActiveTabTextColour())
        
        if hasImage:
        
            imageXOffset = textOffset - 16 - padding
            pc._ImageList.Draw(pc._pagesInfoVec[tabIdx].GetImageIndex(), dc,
                                     posx + imageXOffset, imageYCoord,
                                     wx.IMAGELIST_DRAW_TRANSPARENT, True)
        
        dc.DrawText(pc.GetPageText(tabIdx), posx + textOffset, imageYCoord)
        
        # draw 'x' on tab (if enabled)
        if pc.HasFlag(FNB_X_ON_TAB) and tabIdx == pc.GetSelection():
        
            textWidth, textHeight = dc.GetTextExtent(pc.GetPageText(tabIdx))
            tabCloseButtonXCoord = posx + textOffset + textWidth + 1

            # take a bitmap from the position of the 'x' button (the x on tab button)
            # this bitmap will be used later to delete old buttons
            tabCloseButtonYCoord = imageYCoord
            x_rect = wx.Rect(tabCloseButtonXCoord, tabCloseButtonYCoord, 16, 16)
            self._tabXBgBmp = self._GetBitmap(dc, x_rect, self._tabXBgBmp)

            # Draw the tab
            self.DrawTabX(pc, dc, x_rect, tabIdx, btnStatus)            
        

#------------------------------------------------------------------
# Visual studio 2005 (VS8)
#------------------------------------------------------------------
class FNBRendererVC8(FNBRenderer):    
    """
    This class handles the drawing of tabs using the I{VC8} renderer.
    """

    def __init__(self):
        """ Default class constructor. """

        FNBRenderer.__init__(self)
        self._first = True
        self._factor = 1

        
    def DrawTabs(self, pageContainer, dc):
        """ Draws all the tabs using VC8 style. Overloads The DrawTabs method in parent class. """

        pc = pageContainer

        if "__WXMAC__" in wx.PlatformInfo:
            # Works well on MSW & GTK, however this lines should be skipped on MAC
            if not pc._pagesInfoVec or pc._nFrom >= len(pc._pagesInfoVec):
                pc.Hide()
                return
            
        # Get the text hight
        tabHeight = self.CalcTabHeight(pageContainer)

        # Set the font for measuring the tab height
        normalFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        boldFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        boldFont.SetWeight(wx.FONTWEIGHT_BOLD)

        # Calculate the number of rows required for drawing the tabs
        rect = pc.GetClientRect()

        # Set the maximum client size
        pc.SetSizeHints(self.GetButtonsAreaLength(pc), tabHeight)
        borderPen = wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW))

        # Create brushes
        backBrush = wx.Brush(pc._tabAreaColor)
        noselBrush = wx.Brush(wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNFACE))
        selBrush = wx.Brush(pc._activeTabColor)
        size = pc.GetSize()

        # Background
        dc.SetTextBackground(pc.GetBackgroundColour())
        dc.SetTextForeground(pc._activeTextColor)
        
        # If border style is set, set the pen to be border pen
        if pc.HasFlag(FNB_TABS_BORDER_SIMPLE):
            dc.SetPen(borderPen)
        else:
            dc.SetPen(wx.TRANSPARENT_PEN)

        lightFactor = (pc.HasFlag(FNB_BACKGROUND_GRADIENT) and [70] or [0])[0]
        
        # For VC8 style, we color the tab area in gradient coloring
        lightcolour = LightColour(pc._tabAreaColor, lightFactor)
        PaintStraightGradientBox(dc, pc.GetClientRect(), pc._tabAreaColor, lightcolour)

        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.DrawRectangle(0, 0, size.x, size.y)

        # Take 3 bitmaps for the background for the buttons
        
        mem_dc = wx.MemoryDC()
        #---------------------------------------
        # X button
        #---------------------------------------
        rect = wx.Rect(self.GetXPos(pc), 6, 16, 14)
        mem_dc.SelectObject(self._xBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)

        #---------------------------------------
        # Right button
        #---------------------------------------
        rect = wx.Rect(self.GetRightButtonPos(pc), 6, 16, 14)
        mem_dc.SelectObject(self._rightBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)

        #---------------------------------------
        # Left button
        #---------------------------------------
        rect = wx.Rect(self.GetLeftButtonPos(pc), 6, 16, 14)
        mem_dc.SelectObject(self._leftBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)
    
        # We always draw the bottom/upper line of the tabs
        # regradless the style
        dc.SetPen(borderPen)
        self.DrawTabsLine(pc, dc)

        # Restore the pen
        dc.SetPen(borderPen)

        # Draw labels
        dc.SetFont(boldFont)

        # Update all the tabs from 0 to 'pc.self._nFrom' to be non visible
        for i in xrange(pc._nFrom):
        
            pc._pagesInfoVec[i].SetPosition(wx.Point(-1, -1))
            pc._pagesInfoVec[i].GetRegion().Clear()
        
        # Draw the visible tabs, in VC8 style, we draw them from right to left
        vTabsInfo = self.NumberTabsCanFit(pc)

        activeTabPosx = 0
        activeTabWidth = 0
        activeTabHeight = 0

        for cur in xrange(len(vTabsInfo)-1, -1, -1):
        
            # 'i' points to the index of the currently drawn tab
            # in pc.GetPageInfoVector() vector
            i = pc._nFrom + cur
            dc.SetPen(borderPen)
            dc.SetBrush((i==pc.GetSelection() and [selBrush] or [noselBrush])[0])

            # Now set the font to the correct font
            dc.SetFont((i==pc.GetSelection() and [boldFont] or [normalFont])[0])

            # Add the padding to the tab width
            # Tab width:
            # +-----------------------------------------------------------+
            # | PADDING | IMG | IMG_PADDING | TEXT | PADDING | x |PADDING |
            # +-----------------------------------------------------------+

            tabWidth = self.CalcTabWidth(pageContainer, i, tabHeight)
            posx = vTabsInfo[cur].x

            # By default we clean the tab region
            # incase we use the VC8 style which requires
            # the region, it will be filled by the function
            # drawVc8Tab
            pc._pagesInfoVec[i].GetRegion().Clear()
            
            # Clean the 'x' buttn on the tab 
            # 'Clean' rectanlge is a rectangle with width or height
            # with values lower than or equal to 0
            pc._pagesInfoVec[i].GetXRect().SetSize(wx.Size(-1, -1))

            # Draw the tab
            # Incase we are drawing the active tab
            # we need to redraw so it will appear on top
            # of all other tabs

            # when using the vc8 style, we keep the position of the active tab so we will draw it again later
            if i == pc.GetSelection() and pc.HasFlag(FNB_VC8):
            
                activeTabPosx = posx
                activeTabWidth = tabWidth
                activeTabHeight = tabHeight
            
            else:
            
                self.DrawTab(pc, dc, posx, i, tabWidth, tabHeight, pc._nTabXButtonStatus)
            
            # Restore the text forground
            dc.SetTextForeground(pc._activeTextColor)

            # Update the tab position & size
            pc._pagesInfoVec[i].SetPosition(wx.Point(posx, VERTICAL_BORDER_PADDING))
            pc._pagesInfoVec[i].SetSize(wx.Size(tabWidth, tabHeight))
        
        # Incase we are in VC8 style, redraw the active tab (incase it is visible)
        if pc.GetSelection() >= pc._nFrom and pc.GetSelection() < pc._nFrom + len(vTabsInfo):
        
            self.DrawTab(pc, dc, activeTabPosx, pc.GetSelection(), activeTabWidth, activeTabHeight, pc._nTabXButtonStatus)
        
        # Update all tabs that can not fit into the screen as non-visible
        for xx in xrange(pc._nFrom + len(vTabsInfo), len(pc._pagesInfoVec)):
        
            pc._pagesInfoVec[xx].SetPosition(wx.Point(-1, -1))
            pc._pagesInfoVec[xx].GetRegion().Clear()
        
        # Draw the left/right/close buttons 
        # Left arrow
        self.DrawLeftArrow(pc, dc)
        self.DrawRightArrow(pc, dc)
        self.DrawX(pc, dc)
        self.DrawDropDownArrow(pc, dc)


    def DrawTab(self, pageContainer, dc, posx, tabIdx, tabWidth, tabHeight, btnStatus):
        """ Draws a tab using VC8 style. """

        pc = pageContainer
        borderPen = wx.Pen(pc._pParent.GetBorderColour())
        tabPoints = [wx.Point() for ii in xrange(8)]

        # If we draw the first tab or the active tab, 
        # we draw a full tab, else we draw a truncated tab
        #
        #             X(2)                  X(3)
        #        X(1)                            X(4)
        #                                          
        #                                           X(5)
        #                                           
        # X(0),(7)                                  X(6)
        #
        #

        tabPoints[0].x = (pc.HasFlag(FNB_BOTTOM) and [posx] or [posx+self._factor])[0]
        tabPoints[0].y = (pc.HasFlag(FNB_BOTTOM) and [2] or [tabHeight - 3])[0]

        tabPoints[1].x = tabPoints[0].x + tabHeight - VERTICAL_BORDER_PADDING - 3 - self._factor
        tabPoints[1].y = (pc.HasFlag(FNB_BOTTOM) and [tabHeight - (VERTICAL_BORDER_PADDING+2)] or [(VERTICAL_BORDER_PADDING+2)])[0]

        tabPoints[2].x = tabPoints[1].x + 4
        tabPoints[2].y = (pc.HasFlag(FNB_BOTTOM) and [tabHeight - VERTICAL_BORDER_PADDING] or [VERTICAL_BORDER_PADDING])[0]

        tabPoints[3].x = tabPoints[2].x + tabWidth - 2
        tabPoints[3].y = (pc.HasFlag(FNB_BOTTOM) and [tabHeight - VERTICAL_BORDER_PADDING] or [VERTICAL_BORDER_PADDING])[0]

        tabPoints[4].x = tabPoints[3].x + 1
        tabPoints[4].y = (pc.HasFlag(FNB_BOTTOM) and [tabPoints[3].y - 1] or [tabPoints[3].y + 1])[0]

        tabPoints[5].x = tabPoints[4].x + 1
        tabPoints[5].y = (pc.HasFlag(FNB_BOTTOM) and [(tabPoints[4].y - 1)] or [tabPoints[4].y + 1])[0]

        tabPoints[6].x = tabPoints[2].x + tabWidth
        tabPoints[6].y = tabPoints[0].y

        tabPoints[7].x = tabPoints[0].x
        tabPoints[7].y = tabPoints[0].y

        pc._pagesInfoVec[tabIdx].SetRegion(tabPoints)

        # Draw the polygon
        br = dc.GetBrush()
        dc.SetBrush(wx.Brush((tabIdx == pc.GetSelection() and [pc._activeTabColor] or [pc._colorTo])[0]))
        dc.SetPen(wx.Pen((tabIdx == pc.GetSelection() and [wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW)] or [pc._colorBorder])[0]))
        dc.DrawPolygon(tabPoints)

        # Restore the brush
        dc.SetBrush(br)
        rect = pc.GetClientRect()

        if tabIdx != pc.GetSelection() and not pc.HasFlag(FNB_BOTTOM):
        
            # Top default tabs
            dc.SetPen(wx.Pen(pc._pParent.GetBorderColour()))
            lineY = rect.height
            curPen = dc.GetPen()
            curPen.SetWidth(1)
            dc.SetPen(curPen)
            dc.DrawLine(posx, lineY, posx+rect.width, lineY)
        
        # Incase we are drawing the selected tab, we draw the border of it as well
        # but without the bottom (upper line incase of wxBOTTOM)
        if tabIdx == pc.GetSelection():
        
            borderPen = wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW))
            dc.SetPen(borderPen)
            dc.SetBrush(wx.TRANSPARENT_BRUSH)
            dc.DrawPolygon(tabPoints)

            # Delete the bottom line (or the upper one, incase we use wxBOTTOM) 
            dc.SetPen(wx.WHITE_PEN)
            dc.DrawLine(tabPoints[0].x, tabPoints[0].y, tabPoints[6].x, tabPoints[6].y)

        self.FillVC8GradientColour(pc, dc, tabPoints, tabIdx == pc.GetSelection(), tabIdx)

        # Draw a thin line to the right of the non-selected tab
        if tabIdx != pc.GetSelection():
        
            dc.SetPen(wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE)))
            dc.DrawLine(tabPoints[4].x-1, tabPoints[4].y, tabPoints[5].x-1, tabPoints[5].y)
            dc.DrawLine(tabPoints[5].x-1, tabPoints[5].y, tabPoints[6].x-1, tabPoints[6].y)
        
        # Text drawing offset from the left border of the 
        # rectangle
        
        # The width of the images are 16 pixels
        vc8ShapeLen = tabHeight - VERTICAL_BORDER_PADDING - 2
        if pc.TabHasImage(tabIdx):
            textOffset = 2*pc._pParent.GetPadding() + 16 + vc8ShapeLen 
        else:
            textOffset = pc._pParent.GetPadding() + vc8ShapeLen

        # Draw the image for the tab if any
        imageYCoord = (pc.HasFlag(FNB_BOTTOM) and [6] or [8])[0]

        if pc.TabHasImage(tabIdx):
        
            imageXOffset = textOffset - 16 - pc._pParent.GetPadding()
            pc._ImageList.Draw(pc._pagesInfoVec[tabIdx].GetImageIndex(), dc,
                                     posx + imageXOffset, imageYCoord,
                                     wx.IMAGELIST_DRAW_TRANSPARENT, True)        

        boldFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
            
        # if selected tab, draw text in bold
        if tabIdx == pc.GetSelection():
            boldFont.SetWeight(wx.FONTWEIGHT_BOLD)
        
        dc.SetFont(boldFont)
        dc.DrawText(pc.GetPageText(tabIdx), posx + textOffset, imageYCoord)

        # draw 'x' on tab (if enabled)
        if pc.HasFlag(FNB_X_ON_TAB) and tabIdx == pc.GetSelection():

            textWidth, textHeight = dc.GetTextExtent(pc.GetPageText(tabIdx))
            tabCloseButtonXCoord = posx + textOffset + textWidth + 1

            # take a bitmap from the position of the 'x' button (the x on tab button)
            # this bitmap will be used later to delete old buttons
            tabCloseButtonYCoord = imageYCoord
            x_rect = wx.Rect(tabCloseButtonXCoord, tabCloseButtonYCoord, 16, 16)
            self._tabXBgBmp = self._GetBitmap(dc, x_rect, self._tabXBgBmp)
            # Draw the tab
            self.DrawTabX(pc, dc, x_rect, tabIdx, btnStatus)


    def FillVC8GradientColour(self, pageContainer, dc, tabPoints, bSelectedTab, tabIdx):
        """ Fills a tab with a gradient shading. """

        # calculate gradient coefficients
        pc = pageContainer

        if self._first:
            self._first = False
            pc._colorTo   = LightColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE), 0) 
            pc._colorFrom = LightColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE), 60)
        
        col2 = (pc.HasFlag(FNB_BOTTOM) and [pc._pParent.GetGradientColourTo()] or [pc._pParent.GetGradientColourFrom()])[0]
        col1 = (pc.HasFlag(FNB_BOTTOM) and [pc._pParent.GetGradientColourFrom()] or [pc._pParent.GetGradientColourTo()])[0]

        # If colorful tabs style is set, override the tab color
        if pc.HasFlag(FNB_COLORFUL_TABS):
        
            if not pc._pagesInfoVec[tabIdx].GetColour():
            
                # First time, generate color, and keep it in the vector
                tabColor = RandomColour()
                pc._pagesInfoVec[tabIdx].SetColour(tabColor)
            
            if pc.HasFlag(FNB_BOTTOM):
            
                col2 = LightColour(pc._pagesInfoVec[tabIdx].GetColour(), 50)
                col1 = LightColour(pc._pagesInfoVec[tabIdx].GetColour(), 80)
            
            else:
            
                col1 = LightColour(pc._pagesInfoVec[tabIdx].GetColour(), 50)
                col2 = LightColour(pc._pagesInfoVec[tabIdx].GetColour(), 80)
            
        size = abs(tabPoints[2].y - tabPoints[0].y) - 1

        rf, gf, bf = 0, 0, 0
        rstep = float(col2.Red() - col1.Red())/float(size)
        gstep = float(col2.Green() - col1.Green())/float(size)
        bstep = float(col2.Blue() - col1.Blue())/float(size)
        
        y = tabPoints[0].y 

        # If we are drawing the selected tab, we need also to draw a line 
        # from 0.tabPoints[0].x and tabPoints[6].x . end, we achieve this
        # by drawing the rectangle with transparent brush
        # the line under the selected tab will be deleted by the drwaing loop
        if bSelectedTab:
            self.DrawTabsLine(pc, dc)

        while 1:
        
            if pc.HasFlag(FNB_BOTTOM):
            
                if y > tabPoints[0].y + size:
                    break
            
            else:
            
                if y < tabPoints[0].y - size:
                    break
            
            currCol = wx.Colour(col1.Red() + rf, col1.Green() + gf, col1.Blue() + bf)

            dc.SetPen((bSelectedTab and [wx.Pen(pc._activeTabColor)] or [wx.Pen(currCol)])[0])
            startX = self.GetStartX(tabPoints, y, pc.GetParent().GetWindowStyleFlag()) 
            endX = self.GetEndX(tabPoints, y, pc.GetParent().GetWindowStyleFlag())
            dc.DrawLine(startX, y, endX, y)

            # Draw the border using the 'edge' point
            dc.SetPen(wx.Pen((bSelectedTab and [wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW)] or [pc._colorBorder])[0]))
            
            dc.DrawPoint(startX, y)
            dc.DrawPoint(endX, y)
            
            # Progress the color 
            rf += rstep
            gf += gstep
            bf += bstep

            if pc.HasFlag(FNB_BOTTOM):
                y = y + 1
            else:
                y = y - 1


    def GetStartX(self, tabPoints, y, style):
        """ Returns the x start position of a tab. """

        x1, x2, y1, y2 = 0.0, 0.0, 0.0, 0.0

        # We check the 3 points to the left

        bBottomStyle = (style & FNB_BOTTOM and [True] or [False])[0]
        match = False

        if bBottomStyle:
        
            for i in xrange(3):
                
                if y >= tabPoints[i].y and y < tabPoints[i+1].y:
                
                    x1 = tabPoints[i].x
                    x2 = tabPoints[i+1].x
                    y1 = tabPoints[i].y
                    y2 = tabPoints[i+1].y
                    match = True
                    break
                
        else:
        
            for i in xrange(3):
                
                if y <= tabPoints[i].y and y > tabPoints[i+1].y:
                
                    x1 = tabPoints[i].x
                    x2 = tabPoints[i+1].x
                    y1 = tabPoints[i].y
                    y2 = tabPoints[i+1].y
                    match = True
                    break
                
        if not match:
            return tabPoints[2].x

        # According to the equation y = ax + b => x = (y-b)/a
        # We know the first 2 points

        if x2 == x1:
            return x2
        else:
            a = (y2 - y1)/(x2 - x1)

        b = y1 - ((y2 - y1)/(x2 - x1))*x1

        if a == 0:
            return int(x1)

        x = (y - b)/a
        
        return int(x)


    def GetEndX(self, tabPoints, y, style):
        """ Returns the x end position of a tab. """

        x1, x2, y1, y2 = 0.0, 0.0, 0.0, 0.0

        # We check the 3 points to the left
        bBottomStyle = (style & FNB_BOTTOM and [True] or [False])[0]
        match = False

        if bBottomStyle:

            for i in xrange(7, 3, -1):
                
                if y >= tabPoints[i].y and y < tabPoints[i-1].y:
                
                    x1 = tabPoints[i].x
                    x2 = tabPoints[i-1].x
                    y1 = tabPoints[i].y
                    y2 = tabPoints[i-1].y
                    match = True
                    break
        
        else:
        
            for i in xrange(7, 3, -1):
                
                if y <= tabPoints[i].y and y > tabPoints[i-1].y:
                
                    x1 = tabPoints[i].x
                    x2 = tabPoints[i-1].x
                    y1 = tabPoints[i].y
                    y2 = tabPoints[i-1].y
                    match = True
                    break

        if not match:
            return tabPoints[3].x

        # According to the equation y = ax + b => x = (y-b)/a
        # We know the first 2 points

        # Vertical line
        if x1 == x2:
            return int(x1)
        
        a = (y2 - y1)/(x2 - x1)
        b = y1 - ((y2 - y1)/(x2 - x1))*x1

        if a == 0:
            return int(x1)

        x = (y - b)/a

        return int(x)


    def NumberTabsCanFit(self, pageContainer, fr=-1):
        """ Returns the number of tabs that can fit in the visible area. """

        pc = pageContainer
        
        rect = pc.GetClientRect()
        clientWidth = rect.width

        # Empty results
        vTabInfo = []
        tabHeight = self.CalcTabHeight(pageContainer)

        # The drawing starts from posx
        posx = pc._pParent.GetPadding()
        
        if fr < 0:
            fr = pc._nFrom

        for i in xrange(fr, len(pc._pagesInfoVec)):
        
            vc8glitch = tabHeight + FNB_HEIGHT_SPACER
            tabWidth = self.CalcTabWidth(pageContainer, i, tabHeight)

            if posx + tabWidth + vc8glitch + self.GetButtonsAreaLength(pc) >= clientWidth:
                break

            # Add a result to the returned vector
            tabRect = wx.Rect(posx, VERTICAL_BORDER_PADDING, tabWidth, tabHeight)
            vTabInfo.append(tabRect)

            # Advance posx
            posx += tabWidth + FNB_HEIGHT_SPACER
        
        return vTabInfo
    

# ---------------------------------------------------------------------------- #
# Class FlatNotebook
# ---------------------------------------------------------------------------- #

class FlatNotebook(wx.Panel):
    """
    Display one or more windows in a notebook.
    
    B{Events}:
        - B{EVT_FLATNOTEBOOK_PAGE_CHANGING}: sent when the active 
            page in the notebook is changing
        - B{EVT_FLATNOTEBOOK_PAGE_CHANGED}: sent when the active 
            page in the notebook has changed
        - B{EVT_FLATNOTEBOOK_PAGE_CLOSING}: sent when a page in the 
            notebook is closing
        - B{EVT_FLATNOTEBOOK_PAGE_CLOSED}: sent when a page in the 
            notebook has been closed
        - B{EVT_FLATNOTEBOOK_PAGE_CONTEXT_MENU}: sent when the user
            clicks a tab in the notebook with the right mouse
            button
    """
    
    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=0, name="FlatNotebook"):
        """
        Default class constructor.

        All the parameters are as in wxPython class construction, except the
        'style': this can be assigned to whatever combination of FNB_* styles.
        
        """

        self._bForceSelection = False
        self._nPadding = 6
        self._nFrom = 0
        style |= wx.TAB_TRAVERSAL
        self._pages = None
        self._windows = []
        self._popupWin = None

        wx.Panel.__init__(self, parent, id, pos, size, style)
        
        self._pages = PageContainer(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, style)

        self.Bind(wx.EVT_NAVIGATION_KEY, self.OnNavigationKey)

        self.Init()


    def Init(self):
        """ Initializes all the class attributes. """
        
        self._pages._colorBorder = wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW)

        self._mainSizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self._mainSizer)

        # The child panels will inherit this bg color, so leave it at the default value
        #self.SetBackgroundColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_APPWORKSPACE))

        # Set default page height
        dc = wx.ClientDC(self)
        
        if "__WXGTK__" in wx.PlatformInfo:
            # For GTK it seems that we must do this steps in order
            # for the tabs will get the proper height on initialization
            # on MSW, preforming these steps yields wierd results
            boldFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
            boldFont.SetWeight(wx.FONTWEIGHT_BOLD)
            dc.SetFont(boldFont)
        
        height = dc.GetCharHeight()

        tabHeight = height + FNB_HEIGHT_SPACER         # We use 8 pixels as padding
        
        if "__WXGTK__" in wx.PlatformInfo:
            tabHeight += 6
            
        self._pages.SetSizeHints(-1, tabHeight)
        # Add the tab container to the sizer
        self._mainSizer.Insert(0, self._pages, 0, wx.EXPAND)
        self._mainSizer.Layout()

        self._pages._nFrom = self._nFrom
        self._pDropTarget = FNBDropTarget(self)
        self.SetDropTarget(self._pDropTarget)


    def SetActiveTabTextColour(self, textColour):
        """ Sets the text colour for the active tab. """

        self._pages._activeTextColor = textColour


    def OnDropTarget(self, x, y, nTabPage, wnd_oldContainer):
        """ Handles the drop action from a DND operation. """

        return self._pages.OnDropTarget(x, y, nTabPage, wnd_oldContainer)


    def GetPreviousSelection(self):
        """ Returns the previous selection. """

        return self._pages._iPreviousActivePage


    def AddPage(self, page, text, select=True, imageId=-1):
        """
        Add a page to the L{FlatNotebook}.

        @param page: Specifies the new page.
        @param text: Specifies the text for the new page.
        @param select: Specifies whether the page should be selected.
        @param imageId: Specifies the optional image index for the new page.
        
        Return value:
        True if successful, False otherwise.
        """

        # sanity check
        if not page:
            return False

        # reparent the window to us
        page.Reparent(self)

        # Add tab
        bSelected = select or len(self._windows) == 0

        if bSelected:
            
            bSelected = False
            
            # Check for selection and send events
            oldSelection = self._pages._iActivePage
            tabIdx = len(self._windows)
            
            event = FlatNotebookEvent(wxEVT_FLATNOTEBOOK_PAGE_CHANGING, self.GetId())
            event.SetSelection(tabIdx)
            event.SetOldSelection(oldSelection)
            event.SetEventObject(self)
            
            if not self.GetEventHandler().ProcessEvent(event) or event.IsAllowed() or len(self._windows) == 0:
                bSelected = True            
        
        curSel = self._pages.GetSelection()

        if not self._pages.IsShown():
            self._pages.Show()

        self._pages.AddPage(text, bSelected, imageId)
        self._windows.append(page)

        self.Freeze()

        # Check if a new selection was made
        if bSelected:
        
            if curSel >= 0:
            
                # Remove the window from the main sizer
                self._mainSizer.Detach(self._windows[curSel])
                self._windows[curSel].Hide()
            
            if self.GetWindowStyleFlag() & FNB_BOTTOM:
            
                self._mainSizer.Insert(0, page, 1, wx.EXPAND)
            
            else:
            
                # We leave a space of 1 pixel around the window
                self._mainSizer.Add(page, 1, wx.EXPAND)

            # Fire a wxEVT_FLATNOTEBOOK_PAGE_CHANGED event
            event.SetEventType(wxEVT_FLATNOTEBOOK_PAGE_CHANGED)
            event.SetOldSelection(oldSelection)
            self.GetEventHandler().ProcessEvent(event)
            
        else:

            # Hide the page
            page.Hide()

        self.Thaw()        
        self._mainSizer.Layout()
        self.Refresh()

        return True        


    def SetImageList(self, imageList):
        """
        Sets the image list for the page control. It does not take ownership
        of the image list, you must delete it yourself.
        """

        self._pages.SetImageList(imageList)


    def GetImageList(self):
        """ Returns the associated image list. """
        
        return self._pages.GetImageList()


    def InsertPage(self, indx, page, text, select=True, imageId=-1):
        """
        Inserts a new page at the specified position.

        @param indx: Specifies the position of the new page.
        @param page: Specifies the new page.
        @param text: Specifies the text for the new page.
        @param select: Specifies whether the page should be selected.
        @param imageId: Specifies the optional image index for the new page.
        
        Return value:
        True if successful, False otherwise.
        """     

        # sanity check
        if not page:
            return False

        # reparent the window to us
        page.Reparent(self)

        if not self._windows:
        
            self.AddPage(page, text, select, imageId)
            return True

        # Insert tab
        bSelected = select or not self._windows
        curSel = self._pages.GetSelection()
        
        indx = max(0, min(indx, len(self._windows)))

        if indx <= len(self._windows):
        
            self._windows.insert(indx, page)
        
        else:
        
            self._windows.append(page)

        if bSelected:
        
            bSelected = False
            
            # Check for selection and send events
            oldSelection = self._pages._iActivePage
            
            event = FlatNotebookEvent(wxEVT_FLATNOTEBOOK_PAGE_CHANGING, self.GetId())
            event.SetSelection(indx)
            event.SetOldSelection(oldSelection)
            event.SetEventObject(self)
            
            if not self.GetEventHandler().ProcessEvent(event) or event.IsAllowed() or len(self._windows) == 0:
                bSelected = True            
        
        self._pages.InsertPage(indx, text, bSelected, imageId)
        
        if indx <= curSel:
            curSel = curSel + 1

        self.Freeze()

        # Check if a new selection was made
        if bSelected:
        
            if curSel >= 0:
            
                # Remove the window from the main sizer
                self._mainSizer.Detach(self._windows[curSel])
                self._windows[curSel].Hide()
            
            self._pages.SetSelection(indx)

            # Fire a wxEVT_FLATNOTEBOOK_PAGE_CHANGED event
            event.SetEventType(wxEVT_FLATNOTEBOOK_PAGE_CHANGED)
            event.SetOldSelection(oldSelection)
            self.GetEventHandler().ProcessEvent(event)
        
        else:
        
            # Hide the page
            page.Hide()

        self.Thaw()
        self._mainSizer.Layout()        
        self.Refresh()

        return True


    def SetSelection(self, page):
        """
        Sets the selection for the given page.
        The call to this function generates the page changing events
        """

        if page >= len(self._windows) or not self._windows:
            return

        # Support for disabed tabs
        if not self._pages.GetEnabled(page) and len(self._windows) > 1 and not self._bForceSelection:
            return

        curSel = self._pages.GetSelection()

        # program allows the page change
        self.Freeze()
        if curSel >= 0:
        
            # Remove the window from the main sizer
            self._mainSizer.Detach(self._windows[curSel])
            self._windows[curSel].Hide()
        
        if self.GetWindowStyleFlag() & FNB_BOTTOM:
        
            self._mainSizer.Insert(0, self._windows[page], 1, wx.EXPAND)
        
        else:
        
            # We leave a space of 1 pixel around the window
            self._mainSizer.Add(self._windows[page], 1, wx.EXPAND)
        
        self._windows[page].Show()
        self.Thaw()
        
        self._mainSizer.Layout()
        
        if page != self._pages._iActivePage:
            # there is a real page changing
            self._pages._iPreviousActivePage = self._pages._iActivePage

        self._pages._iActivePage = page
        self._pages.DoSetSelection(page)


    def DeletePage(self, page):
        """
        Deletes the specified page, and the associated window.
        The call to this function generates the page changing events.
        """

        if page >= len(self._windows) or page < 0:
            return

        # Fire a closing event
        event = FlatNotebookEvent(wxEVT_FLATNOTEBOOK_PAGE_CLOSING, self.GetId())
        event.SetSelection(page)
        event.SetEventObject(self)
        self.GetEventHandler().ProcessEvent(event)

        # The event handler allows it?
        if not event.IsAllowed():
            return

        self.Freeze()

        # Delete the requested page
        pageRemoved = self._windows[page]

        # If the page is the current window, remove it from the sizer
        # as well
        if page == self._pages.GetSelection():
            self._mainSizer.Detach(pageRemoved)
        
        # Remove it from the array as well
        self._windows.pop(page)

        # Now we can destroy it in wxWidgets use Destroy instead of delete
        pageRemoved.Destroy()

        self.Thaw()

        self._pages.DoDeletePage(page)
        self.Refresh()

        # Fire a closed event
        closedEvent = FlatNotebookEvent(wxEVT_FLATNOTEBOOK_PAGE_CLOSED, self.GetId())
        closedEvent.SetSelection(page)
        closedEvent.SetEventObject(self)
        self.GetEventHandler().ProcessEvent(closedEvent)


    def DeleteAllPages(self):
        """ Deletes all the pages. """

        if not self._windows:
            return False

        self.Freeze()
        
        for page in self._windows:
            page.Destroy()
        
        self._windows = []
        self.Thaw()

        # Clear the container of the tabs as well
        self._pages.DeleteAllPages()
        return True


    def GetCurrentPage(self):
        """ Returns the currently selected notebook page or None. """
        
        sel = self._pages.GetSelection()
        if sel < 0:
            return None

        return self._windows[sel]


    def GetPage(self, page):
        """ Returns the window at the given page position, or None. """

        if page >= len(self._windows):
            return None

        return self._windows[page]


    def GetPageIndex(self, win):
        """ Returns the index at which the window is found. """

        try:
            return self._windows.index(win)
        except:
            return -1


    def GetSelection(self):
        """ Returns the currently selected page, or -1 if none was selected. """
        
        return self._pages.GetSelection()


    def AdvanceSelection(self, forward=True):
        """
        Cycles through the tabs.
        The call to this function generates the page changing events.
        """

        self._pages.AdvanceSelection(forward)


    def GetPageCount(self):
        """ Returns the number of pages in the L{FlatNotebook} control. """

        return self._pages.GetPageCount()


    def OnNavigationKey(self, event):
        """ Handles the wx.EVT_NAVIGATION_KEY event for L{FlatNotebook}. """

        if event.IsWindowChange():
            if len(self._windows) == 0:
                return
            # change pages
            if self.HasFlag(FNB_SMART_TABS):
                if not self._popupWin:
                    self._popupWin = TabNavigatorWindow(self)
                    self._popupWin.SetReturnCode(wx.ID_OK)
                    self._popupWin.ShowModal()
                    self._popupWin.Destroy()
                    self._popupWin = None
                else:
                    # a dialog is already opened
                    self._popupWin.OnNavigationKey(event)
                    return
            else:
                # change pages
                self.AdvanceSelection(event.GetDirection())
        else:
            # pass to the parent
            if self.GetParent():
                event.SetCurrentFocus(self)
                self.GetParent().ProcessEvent(event)
            

    def GetPageShapeAngle(self, page_index):
        """ Returns the angle associated to a tab. """

        if page_index < 0 or page_index >= len(self._pages._pagesInfoVec):
            return None, False
        
        result = self._pages._pagesInfoVec[page_index].GetTabAngle()
        return result, True


    def SetPageShapeAngle(self, page_index, angle):
        """ Sets the angle associated to a tab. """

        if page_index < 0 or page_index >= len(self._pages._pagesInfoVec):
            return

        if angle > 15:
            return

        self._pages._pagesInfoVec[page_index].SetTabAngle(angle)


    def SetAllPagesShapeAngle(self, angle):
        """ Sets the angle associated to all the tab. """

        if angle > 15:
            return

        for ii in xrange(len(self._pages._pagesInfoVec)):
            self._pages._pagesInfoVec[ii].SetTabAngle(angle)
        
        self.Refresh()


    def GetPageBestSize(self):
        """ Return the page best size. """

        return self._pages.GetClientSize()


    def SetPageText(self, page, text):
        """ Sets the text for the given page. """

        bVal = self._pages.SetPageText(page, text)
        self._pages.Refresh()

        return bVal


    def SetPadding(self, padding):
        """
        Sets the amount of space around each page's icon and label, in pixels.
        NB: only the horizontal padding is considered.
        """

        self._nPadding = padding.GetWidth()


    def GetTabArea(self):
        """ Returns the associated page. """

        return self._pages


    def GetPadding(self):
        """ Returns the amount of space around each page's icon and label, in pixels. """
        
        return self._nPadding 


    def SetWindowStyleFlag(self, style):
        """ Sets the L{FlatNotebook} window style flags. """
            
        wx.Panel.SetWindowStyleFlag(self, style)
        renderer = self._pages._mgr.GetRenderer(self.GetWindowStyleFlag())
        renderer._tabHeight = None

        if self._pages:
        
            # For changing the tab position (i.e. placing them top/bottom)
            # refreshing the tab container is not enough
            self.SetSelection(self._pages._iActivePage)


    def RemovePage(self, page):
        """ Deletes the specified page, without deleting the associated window. """

        if page >= len(self._windows):
            return False

        # Fire a closing event
        event = FlatNotebookEvent(wxEVT_FLATNOTEBOOK_PAGE_CLOSING, self.GetId())
        event.SetSelection(page)
        event.SetEventObject(self)
        self.GetEventHandler().ProcessEvent(event)

        # The event handler allows it?
        if not event.IsAllowed():
            return False

        self.Freeze()

        # Remove the requested page
        pageRemoved = self._windows[page]

        # If the page is the current window, remove it from the sizer
        # as well
        if page == self._pages.GetSelection():
            self._mainSizer.Detach(pageRemoved)
        
        # Remove it from the array as well
        self._windows.pop(page)
        self.Thaw()

        self._pages.DoDeletePage(page)

        return True


    def SetRightClickMenu(self, menu):
        """ Sets the popup menu associated to a right click on a tab. """

        self._pages._pRightClickMenu = menu


    def GetPageText(self, nPage):
        """ Returns the tab caption. """

        return self._pages.GetPageText(nPage)


    def SetGradientColours(self, fr, to, border):
        """ Sets the gradient colours for the tab. """

        self._pages._colorFrom = fr
        self._pages._colorTo   = to
        self._pages._colorBorder = border


    def SetGradientColourFrom(self, fr):
        """ Sets the starting colour for the gradient. """

        self._pages._colorFrom = fr


    def SetGradientColourTo(self, to):
        """ Sets the ending colour for the gradient. """

        self._pages._colorTo = to


    def SetGradientColourBorder(self, border):
        """ Sets the tab border colour. """

        self._pages._colorBorder = border


    def GetGradientColourFrom(self):
        """ Gets first gradient colour. """

        return self._pages._colorFrom


    def GetGradientColourTo(self):
        """ Gets second gradient colour. """

        return self._pages._colorTo


    def GetGradientColourBorder(self):
        """ Gets the tab border colour. """

        return self._pages._colorBorder


    def GetBorderColour(self):
        """ Returns the border colour. """

        return self._pages._colorBorder
    

    def GetActiveTabTextColour(self):
        """ Get the active tab text colour. """

        return self._pages._activeTextColor


    def SetPageImage(self, page, image):
        """
        Sets the image index for the given page. Image is an index into the
        image list which was set with SetImageList.
        """

        self._pages.SetPageImage(page, image)


    def GetPageImage(self, nPage):
        """
        Returns the image index for the given page. Image is an index into the
        image list which was set with SetImageList.
        """

        return self._pages.GetPageImage(nPage)


    def GetEnabled(self, page):
        """ Returns whether a tab is enabled or not. """

        return self._pages.GetEnabled(page)


    def Enable(self, page, enabled=True):
        """ Enables or disables a tab. """

        if page >= len(self._windows):
            return

        self._windows[page].Enable(enabled)
        self._pages.Enable(page, enabled)


    def GetNonActiveTabTextColour(self):
        """ Returns the non active tabs text colour. """

        return self._pages._nonActiveTextColor


    def SetNonActiveTabTextColour(self, color):
        """ Sets the non active tabs text colour. """

        self._pages._nonActiveTextColor = color


    def SetTabAreaColour(self, color):
        """ Sets the area behind the tabs colour. """

        self._pages._tabAreaColor = color


    def GetTabAreaColour(self):
        """ Returns the area behind the tabs colour. """

        return self._pages._tabAreaColor


    def SetActiveTabColour(self, color):
        """ Sets the active tab colour. """

        self._pages._activeTabColor = color


    def GetActiveTabColour(self):
        """ Returns the active tab colour. """

        return self._pages._activeTabColor


# ---------------------------------------------------------------------------- #
# Class PageContainer
# Acts as a container for the pages you add to FlatNotebook
# ---------------------------------------------------------------------------- #

class PageContainer(wx.Panel):
    """
    This class acts as a container for the pages you add to L{FlatNotebook}.
    """

    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0):
        """ Default class constructor. """
        
        self._ImageList = None
        self._iActivePage = -1
        self._pDropTarget = None
        self._nLeftClickZone = FNB_NOWHERE
        self._iPreviousActivePage = -1

        self._pRightClickMenu = None
        self._nXButtonStatus = FNB_BTN_NONE
        self._nArrowDownButtonStatus = FNB_BTN_NONE
        self._pParent = parent
        self._nRightButtonStatus = FNB_BTN_NONE
        self._nLeftButtonStatus = FNB_BTN_NONE
        self._nTabXButtonStatus = FNB_BTN_NONE

        self._pagesInfoVec = []        

        self._colorTo = wx.SystemSettings_GetColour(wx.SYS_COLOUR_ACTIVECAPTION)
        self._colorFrom = wx.WHITE
        self._activeTabColor = wx.WHITE
        self._activeTextColor = wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNTEXT)
        self._nonActiveTextColor = wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW)
        self._tabAreaColor = wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNFACE)

        self._nFrom = 0
        self._isdragging = False

        # Set default page height, this is done according to the system font
        memDc = wx.MemoryDC()
        memDc.SelectObject(wx.EmptyBitmap(1,1))
    
        if "__WXGTK__" in wx.PlatformInfo:
            boldFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
            boldFont.SetWeight(wx.BOLD)
            memDc.SetFont(boldFont)

        height = memDc.GetCharHeight()
        tabHeight = height + FNB_HEIGHT_SPACER # We use 10 pixels as padding

        wx.Panel.__init__(self, parent, id, pos, wx.Size(size.x, tabHeight),
                          style|wx.NO_BORDER|wx.NO_FULL_REPAINT_ON_RESIZE)

        self._pDropTarget = FNBDropTarget(self)
        self.SetDropTarget(self._pDropTarget)
        self._mgr = FNBRendererMgr()

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightDown)
        self.Bind(wx.EVT_MIDDLE_DOWN, self.OnMiddleDown)
        self.Bind(wx.EVT_MOTION, self.OnMouseMove)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.OnMouseLeave)
        self.Bind(wx.EVT_ENTER_WINDOW, self.OnMouseEnterWindow)
        self.Bind(wx.EVT_LEFT_DCLICK, self.OnLeftDClick)


    def OnEraseBackground(self, event):
        """ Handles the wx.EVT_ERASE_BACKGROUND event for L{PageContainer} (does nothing)."""

        pass

    
    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for L{PageContainer}."""

        # Currently having problems with buffered DCs because of
        # recent changes.  Just do the buffering ourselves instead.
        dc = wx.BufferedPaintDC(self)
        #dc = wx.AutoBufferedPaintDC(self)
        #dc = wx.AutoBufferedPaintDCFactory(self)
##         size = self.GetSize()
##         bmp = wx.EmptyBitmap(*size)
##         dc = wx.MemoryDC()
##         dc.SelectObject(bmp)
        
        renderer = self._mgr.GetRenderer(self.GetParent().GetWindowStyleFlag())
        renderer.DrawTabs(self, dc)

##         pdc = wx.PaintDC(self)
##         pdc.Blit(0,0, size.width, size.height, dc, 0,0)


    def AddPage(self, caption, selected=True, imgindex=-1):
        """
        Add a page to the L{FlatNotebook}.

        @param window: Specifies the new page.
        @param caption: Specifies the text for the new page.
        @param selected: Specifies whether the page should be selected.
        @param imgindex: Specifies the optional image index for the new page.
        
        Return value:
        True if successful, False otherwise.
        """

        if selected:

            self._iPreviousActivePage = self._iActivePage        
            self._iActivePage = len(self._pagesInfoVec)
        
        # Create page info and add it to the vector
        pageInfo = PageInfo(caption, imgindex)
        self._pagesInfoVec.append(pageInfo)
        self.Refresh()


    def InsertPage(self, indx, text, selected=True, imgindex=-1):
        """
        Inserts a new page at the specified position.

        @param indx: Specifies the position of the new page.
        @param page: Specifies the new page.
        @param text: Specifies the text for the new page.
        @param select: Specifies whether the page should be selected.
        @param imgindex: Specifies the optional image index for the new page.
        
        Return value:
        True if successful, False otherwise.
        """     

        if selected:

            self._iPreviousActivePage = self._iActivePage        
            self._iActivePage = len(self._pagesInfoVec)
        
        self._pagesInfoVec.insert(indx, PageInfo(text, imgindex))
        
        self.Refresh()
        return True


    def OnSize(self, event):
        """ Handles the wx.EVT_SIZE events for L{PageContainer}. """

        self.Refresh() # Call on paint
        event.Skip()


    def OnMiddleDown(self, event):
        """ Handles the wx.EVT_MIDDLE_DOWN events for L{PageContainer}. """

        # Test if this style is enabled
        style = self.GetParent().GetWindowStyleFlag()
        
        if not style & FNB_MOUSE_MIDDLE_CLOSES_TABS:
            return

        where, tabIdx = self.HitTest(event.GetPosition())
        
        if where == FNB_TAB:
            self.DeletePage(tabIdx)
        
        event.Skip()


    def OnRightDown(self, event):
        """ Handles the wx.EVT_RIGHT_DOWN events for L{PageContainer}. """

        if self._pRightClickMenu:
        
            where, tabIdx = self.HitTest(event.GetPosition())

            if where in [FNB_TAB, FNB_TAB_X]:

                if self._pagesInfoVec[tabIdx].GetEnabled():
                    # Set the current tab to be active
                    self.SetSelection(tabIdx)

                    # If the owner has defined a context menu for the tabs,
                    # popup the right click menu
                    if self._pRightClickMenu:
                        self.PopupMenu(self._pRightClickMenu)
                    else:
                        # send a message to popup a custom menu
                        event = FlatNotebookEvent(wxEVT_FLATNOTEBOOK_PAGE_CONTEXT_MENU, self.GetParent().GetId())
                        event.SetSelection(tabIdx)
                        event.SetOldSelection(self._iActivePage)
                        event.SetEventObject(self.GetParent())
                        self.GetParent().GetEventHandler().ProcessEvent(event)
                
            event.Skip()


    def OnLeftDown(self, event):
        """ Handles the wx.EVT_LEFT_DOWN events for L{PageContainer}. """

        # Reset buttons status
        self._nXButtonStatus     = FNB_BTN_NONE
        self._nLeftButtonStatus  = FNB_BTN_NONE
        self._nRightButtonStatus = FNB_BTN_NONE
        self._nTabXButtonStatus  = FNB_BTN_NONE
        self._nArrowDownButtonStatus = FNB_BTN_NONE

        self._nLeftClickZone, tabIdx = self.HitTest(event.GetPosition())

        if self._nLeftClickZone == FNB_DROP_DOWN_ARROW:
            self._nArrowDownButtonStatus = FNB_BTN_PRESSED
            self.Refresh()
        elif self._nLeftClickZone == FNB_LEFT_ARROW:
            self._nLeftButtonStatus = FNB_BTN_PRESSED
            self.Refresh()
        elif self._nLeftClickZone == FNB_RIGHT_ARROW:
            self._nRightButtonStatus = FNB_BTN_PRESSED
            self.Refresh()
        elif self._nLeftClickZone == FNB_X:
            self._nXButtonStatus = FNB_BTN_PRESSED
            self.Refresh()
        elif self._nLeftClickZone == FNB_TAB_X:
            self._nTabXButtonStatus = FNB_BTN_PRESSED
            self.Refresh()

        elif self._nLeftClickZone == FNB_TAB:
            
            if self._iActivePage != tabIdx:
                
                # In case the tab is disabled, we dont allow to choose it
                if self._pagesInfoVec[tabIdx].GetEnabled():
                    self.FireEvent(tabIdx)


    def OnLeftUp(self, event):
        """ Handles the wx.EVT_LEFT_UP events for L{PageContainer}. """

        # forget the zone that was initially clicked
        self._nLeftClickZone = FNB_NOWHERE

        where, tabIdx = self.HitTest(event.GetPosition())
        
        if where == FNB_LEFT_ARROW:
            
            if self._nFrom == 0:
                return

            # Make sure that the button was pressed before
            if self._nLeftButtonStatus != FNB_BTN_PRESSED:
                return

            self._nLeftButtonStatus = FNB_BTN_HOVER

            # We scroll left with bulks of 5
            scrollLeft = self.GetNumTabsCanScrollLeft()

            self._nFrom -= scrollLeft
            if self._nFrom < 0:
                self._nFrom = 0

            self.Refresh()
            
        elif where == FNB_RIGHT_ARROW:
            
            if self._nFrom >= len(self._pagesInfoVec) - 1:
                return

            # Make sure that the button was pressed before
            if self._nRightButtonStatus != FNB_BTN_PRESSED:
                return

            self._nRightButtonStatus = FNB_BTN_HOVER

            # Check if the right most tab is visible, if it is
            # don't rotate right anymore
            if self._pagesInfoVec[-1].GetPosition() != wx.Point(-1, -1):
                return

            lastVisibleTab = self.GetLastVisibleTab()
            if lastVisibleTab < 0:
                # Probably the screen is too small for displaying even a single
                # tab, in this case we do nothing
                return

            self._nFrom += self.GetNumOfVisibleTabs()
            self.Refresh()
            
        elif where == FNB_X:
            
            # Make sure that the button was pressed before
            if self._nXButtonStatus != FNB_BTN_PRESSED:
                return

            self._nXButtonStatus = FNB_BTN_HOVER

            self.DeletePage(self._iActivePage)
            
        elif where == FNB_TAB_X:
            
            # Make sure that the button was pressed before
            if self._nTabXButtonStatus != FNB_BTN_PRESSED:
                return 

            self._nTabXButtonStatus = FNB_BTN_HOVER

            self.DeletePage(self._iActivePage)

        elif where == FNB_DROP_DOWN_ARROW:

            # Make sure that the button was pressed before
            if self._nArrowDownButtonStatus != FNB_BTN_PRESSED:
                return

            self._nArrowDownButtonStatus = FNB_BTN_NONE

            # Refresh the button status
            renderer = self._mgr.GetRenderer(self.GetParent().GetWindowStyleFlag())
            dc = wx.ClientDC(self)
            renderer.DrawDropDownArrow(self, dc)

            self.PopupTabsMenu()


    def HitTest(self, pt):
        """
        HitTest method for L{PageContainer}.
        Returns the flag (if any) and the hit page (if any).
        """

        style = self.GetParent().GetWindowStyleFlag()
        render = self._mgr.GetRenderer(style)

        fullrect = self.GetClientRect()
        btnLeftPos = render.GetLeftButtonPos(self)
        btnRightPos = render.GetRightButtonPos(self)
        btnXPos = render.GetXPos(self)
        
        tabIdx = -1
        
        if len(self._pagesInfoVec) == 0:
            return FNB_NOWHERE, tabIdx

        rect = wx.Rect(btnXPos, 8, 16, 16)
        if rect.Contains(pt):
            return (style & FNB_NO_X_BUTTON and [FNB_NOWHERE] or [FNB_X])[0], tabIdx

        rect = wx.Rect(btnRightPos, 8, 16, 16)
        if style & FNB_DROPDOWN_TABS_LIST:
            rect = wx.Rect(render.GetDropArrowButtonPos(self), 8, 16, 16)
            if rect.Contains(pt):
                return FNB_DROP_DOWN_ARROW, tabIdx

        if rect.Contains(pt):
            return (style & FNB_NO_NAV_BUTTONS and [FNB_NOWHERE] or [FNB_RIGHT_ARROW])[0], tabIdx

        rect = wx.Rect(btnLeftPos, 8, 16, 16)
        if rect.Contains(pt):
            return (style & FNB_NO_NAV_BUTTONS and [FNB_NOWHERE] or [FNB_LEFT_ARROW])[0], tabIdx

        # Test whether a left click was made on a tab
        bFoundMatch = False
        
        for cur in xrange(self._nFrom, len(self._pagesInfoVec)):

            pgInfo = self._pagesInfoVec[cur]

            if pgInfo.GetPosition() == wx.Point(-1, -1):
                continue

            if style & FNB_X_ON_TAB and cur == self.GetSelection():
                # 'x' button exists on a tab
                if self._pagesInfoVec[cur].GetXRect().Contains(pt):
                    return FNB_TAB_X, cur
                    
            if style & FNB_VC8:

                if self._pagesInfoVec[cur].GetRegion().Contains(pt.x, pt.y):
                    if bFoundMatch or cur == self.GetSelection():    
                        return FNB_TAB, cur

                    tabIdx = cur
                    bFoundMatch = True
                    
            else:

                tabRect = wx.Rect(pgInfo.GetPosition().x, pgInfo.GetPosition().y,
                                  pgInfo.GetSize().x, pgInfo.GetSize().y)
                
                if tabRect.Contains(pt):
                    # We have a match
                    return FNB_TAB, cur

        if bFoundMatch:
            return FNB_TAB, tabIdx

        if self._isdragging:
            # We are doing DND, so check also the region outside the tabs
            # try before the first tab
            pgInfo = self._pagesInfoVec[0]
            tabRect = wx.Rect(0, pgInfo.GetPosition().y, pgInfo.GetPosition().x, self.GetParent().GetSize().y)
            if tabRect.Contains(pt):
                return FNB_TAB, 0

            # try after the last tab
            pgInfo = self._pagesInfoVec[-1]
            startpos = pgInfo.GetPosition().x+pgInfo.GetSize().x
            tabRect = wx.Rect(startpos, pgInfo.GetPosition().y, fullrect.width-startpos, self.GetParent().GetSize().y)

            if tabRect.Contains(pt):
                return FNB_TAB, len(self._pagesInfoVec)        

        # Default
        return FNB_NOWHERE, -1


    def SetSelection(self, page):
        """ Sets the selected page. """

        book = self.GetParent()
        book.SetSelection(page)
        self.DoSetSelection(page)


    def DoSetSelection(self, page):
        """ Does the actual selection of a page. """

        if page < len(self._pagesInfoVec):
            #! fix for tabfocus
            da_page = self._pParent.GetPage(page)
    
            if da_page != None:
                da_page.SetFocus()
        
        if not self.IsTabVisible(page):
        
            if page == len(self._pagesInfoVec) - 1:
                # Incase the added tab is last,
                # the function IsTabVisible() will always return False
                # and thus will cause an evil behaviour that the new
                # tab will hide all other tabs, we need to check if the
                # new selected tab can fit to the current screen
                if not self.CanFitToScreen(page):
                    self._nFrom = page
                            
            else:

                if not self.CanFitToScreen(page):
                    # Redraw the tabs starting from page
                    self._nFrom = page
                    
        self.Refresh()


    def DeletePage(self, page):
        """ Delete the specified page from L{FlatNotebook}. """

        book = self.GetParent()
        book.DeletePage(page)
        book.Refresh()


    def IsTabVisible(self, page):
        """ Returns whether a tab is visible or not. """

        iLastVisiblePage = self.GetLastVisibleTab()
        return page <= iLastVisiblePage and page >= self._nFrom


    def DoDeletePage(self, page):
        """ Does the actual page deletion. """

        # Remove the page from the vector
        book = self.GetParent()
        self._pagesInfoVec.pop(page)

        # Thanks to Yiaanis AKA Mandrav
        if self._iActivePage >= page:
            self._iActivePage = self._iActivePage - 1
            self._iPreviousActivePage = -1

        # The delete page was the last first on the array,
        # but the book still has more pages, so we set the
        # active page to be the first one (0)
        if self._iActivePage < 0 and len(self._pagesInfoVec) > 0:
            self._iActivePage = 0
            self._iPreviousActivePage = -1

        # Refresh the tabs
        if self._iActivePage >= 0:
        
            book._bForceSelection = True

            # Check for selection and send event
            event = FlatNotebookEvent(wxEVT_FLATNOTEBOOK_PAGE_CHANGING, self.GetParent().GetId())
            event.SetSelection(self._iActivePage)
            event.SetOldSelection(self._iPreviousActivePage)
            event.SetEventObject(self.GetParent())

            book.SetSelection(self._iActivePage)
            book._bForceSelection = False

            # Fire a wxEVT_FLATNOTEBOOK_PAGE_CHANGED event
            event.SetEventType(wxEVT_FLATNOTEBOOK_PAGE_CHANGED)
            event.SetOldSelection(self._iPreviousActivePage)
            self.GetParent().GetEventHandler().ProcessEvent(event)            
        
        if not self._pagesInfoVec:        
            # Erase the page container drawings
            dc = wx.ClientDC(self)
            dc.Clear()
        

    def DeleteAllPages(self):
        """ Deletes all the pages. """

        self._iActivePage = -1
        self._iPreviousActivePage = -1
        self._nFrom = 0
        self._pagesInfoVec = []

        # Erase the page container drawings
        dc = wx.ClientDC(self)
        dc.Clear()


    def OnMouseMove(self, event):
        """ Handles the wx.EVT_MOTION for L{PageContainer}. """

        if self._pagesInfoVec and self.IsShown():
        
            xButtonStatus = self._nXButtonStatus
            xTabButtonStatus = self._nTabXButtonStatus
            rightButtonStatus = self._nRightButtonStatus
            leftButtonStatus = self._nLeftButtonStatus
            dropDownButtonStatus = self._nArrowDownButtonStatus
            
            style = self.GetParent().GetWindowStyleFlag()

            self._nXButtonStatus = FNB_BTN_NONE
            self._nRightButtonStatus = FNB_BTN_NONE
            self._nLeftButtonStatus = FNB_BTN_NONE
            self._nTabXButtonStatus = FNB_BTN_NONE
            self._nArrowDownButtonStatus = FNB_BTN_NONE

            where, tabIdx = self.HitTest(event.GetPosition())
            
            if where == FNB_X:
                if event.LeftIsDown():
                
                    self._nXButtonStatus = (self._nLeftClickZone==FNB_X and [FNB_BTN_PRESSED] or [FNB_BTN_NONE])[0]
                
                else:
                
                    self._nXButtonStatus = FNB_BTN_HOVER

            elif where == FNB_DROP_DOWN_ARROW:
                if event.LeftIsDown():

                    self._nArrowDownButtonStatus = (self._nLeftClickZone==FNB_DROP_DOWN_ARROW and [FNB_BTN_PRESSED] or [FNB_BTN_NONE])[0]

                else:

                    self._nArrowDownButtonStatus = FNB_BTN_HOVER

            elif where == FNB_TAB_X:
                if event.LeftIsDown():
                
                    self._nTabXButtonStatus = (self._nLeftClickZone==FNB_TAB_X and [FNB_BTN_PRESSED] or [FNB_BTN_NONE])[0]
                
                else:

                    self._nTabXButtonStatus = FNB_BTN_HOVER
                
            elif where == FNB_RIGHT_ARROW:
                if event.LeftIsDown():
                
                    self._nRightButtonStatus = (self._nLeftClickZone==FNB_RIGHT_ARROW and [FNB_BTN_PRESSED] or [FNB_BTN_NONE])[0]
                
                else:
                
                    self._nRightButtonStatus = FNB_BTN_HOVER
                
            elif where == FNB_LEFT_ARROW:
                if event.LeftIsDown():
                
                    self._nLeftButtonStatus = (self._nLeftClickZone==FNB_LEFT_ARROW and [FNB_BTN_PRESSED] or [FNB_BTN_NONE])[0]
                
                else:
                
                    self._nLeftButtonStatus = FNB_BTN_HOVER
                
            elif where == FNB_TAB:
                # Call virtual method for showing tooltip
                self.ShowTabTooltip(tabIdx)
                
                if not self.GetEnabled(tabIdx):                
                    # Set the cursor to be 'No-entry'
                    wx.SetCursor(wx.StockCursor(wx.CURSOR_NO_ENTRY))
                
                # Support for drag and drop
                if event.Dragging() and not (style & FNB_NODRAG):

                    self._isdragging = True                
                    draginfo = FNBDragInfo(self, tabIdx)
                    drginfo = cPickle.dumps(draginfo)
                    dataobject = wx.CustomDataObject(wx.CustomDataFormat("FlatNotebook"))
                    dataobject.SetData(drginfo)
                    dragSource = FNBDropSource(self)
                    dragSource.SetData(dataobject)
                    dragSource.DoDragDrop(wx.Drag_DefaultMove)
                    
            bRedrawX = self._nXButtonStatus != xButtonStatus
            bRedrawRight = self._nRightButtonStatus != rightButtonStatus
            bRedrawLeft = self._nLeftButtonStatus != leftButtonStatus
            bRedrawTabX = self._nTabXButtonStatus != xTabButtonStatus
            bRedrawDropArrow = self._nArrowDownButtonStatus != dropDownButtonStatus

            render = self._mgr.GetRenderer(style)
        
            if (bRedrawX or bRedrawRight or bRedrawLeft or bRedrawTabX or bRedrawDropArrow):

                dc = wx.ClientDC(self)
                
                if bRedrawX:
                
                    render.DrawX(self, dc)
                
                if bRedrawLeft:
                
                    render.DrawLeftArrow(self, dc)
                
                if bRedrawRight:
                
                    render.DrawRightArrow(self, dc)
                
                if bRedrawTabX:
                
                    render.DrawTabX(self, dc, self._pagesInfoVec[tabIdx].GetXRect(), tabIdx, self._nTabXButtonStatus)

                if bRedrawDropArrow:

                    render.DrawDropDownArrow(self, dc)

        event.Skip()


    def GetLastVisibleTab(self):
        """ Returns the last visible tab. """

        ii = 0
        
        for ii in xrange(self._nFrom, len(self._pagesInfoVec)):
        
            if self._pagesInfoVec[ii].GetPosition() == wx.Point(-1, -1):
                break
        
        return ii-1


    def GetNumTabsCanScrollLeft(self):
        """ Returns the number of tabs than can be scrolled left. """

        # Reserved area for the buttons (<>x)
        rect = self.GetClientRect()
        clientWidth = rect.width
        posx = self._pParent._nPadding
        numTabs = 0
        pom = 0
        
        # In case we have error prevent crash
        if self._nFrom < 0:
            return 0

        dc = wx.ClientDC(self)

        style = self.GetParent().GetWindowStyleFlag()
        render = self._mgr.GetRenderer(style)
        
        for ii in xrange(self._nFrom, -1, -1):

            boldFont = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)        
            boldFont.SetWeight(wx.FONTWEIGHT_BOLD)
            dc.SetFont(boldFont)

            height = dc.GetCharHeight()

            tabHeight = height + FNB_HEIGHT_SPACER # We use 6 pixels as padding
            if style & FNB_VC71:
                tabHeight = (style & FNB_BOTTOM and [tabHeight - 4] or [tabHeight])[0]
            elif style & FNB_FANCY_TABS:
                tabHeight = (style & FNB_BOTTOM and [tabHeight - 3] or [tabHeight])[0]

            width, pom = dc.GetTextExtent(self.GetPageText(ii))
            if style != FNB_VC71:
                shapePoints = int(tabHeight*math.tan(float(self._pagesInfoVec[ii].GetTabAngle())/180.0*math.pi))
            else:
                shapePoints = 0

            tabWidth = 2*self._pParent._nPadding + width
            
            if not (style & FNB_VC71):
                # Default style
                tabWidth += 2*shapePoints

            hasImage = self._ImageList != None and self._pagesInfoVec[ii].GetImageIndex() != -1

            # For VC71 style, we only add the icon size (16 pixels)
            if hasImage:
            
                if not self.IsDefaultTabs():
                    tabWidth += 16 + self._pParent._nPadding
                else:
                    # Default style
                    tabWidth += 16 + self._pParent._nPadding + shapePoints/2
            
            if posx + tabWidth + render.GetButtonsAreaLength(self) >= clientWidth:
                break

            numTabs = numTabs + 1
            posx += tabWidth
        
        return numTabs


    def IsDefaultTabs(self):
        """ Returns whether a tab has a default style. """

        style = self.GetParent().GetWindowStyleFlag()
        res = (style & FNB_VC71) or (style & FNB_FANCY_TABS) or (style & FNB_VC8)
        return not res


    def AdvanceSelection(self, bForward=True):
        """
        Cycles through the tabs.
        The call to this function generates the page changing events.
        """

        nSel = self.GetSelection()

        if nSel < 0:
            return

        nMax = self.GetPageCount() - 1
        
        if bForward:
            newSelection = (nSel == nMax and [0] or [nSel + 1])[0]
        else:
            newSelection = (nSel == 0 and [nMax] or [nSel - 1])[0]

        if not self._pagesInfoVec[newSelection].GetEnabled():
            return

        self.FireEvent(newSelection)


    def OnMouseLeave(self, event):
        """ Handles the wx.EVT_LEAVE_WINDOW event for L{PageContainer}. """

        self._nLeftButtonStatus = FNB_BTN_NONE
        self._nXButtonStatus = FNB_BTN_NONE
        self._nRightButtonStatus = FNB_BTN_NONE
        self._nTabXButtonStatus = FNB_BTN_NONE
        self._nArrowDownButtonStatus = FNB_BTN_NONE

        style = self.GetParent().GetWindowStyleFlag()        
        render = self._mgr.GetRenderer(style)
        
        dc = wx.ClientDC(self)

        render.DrawX(self, dc)
        render.DrawLeftArrow(self, dc)
        render.DrawRightArrow(self, dc)

        selection = self.GetSelection()

        if selection == -1:
            event.Skip()
            return
        
        if not self.IsTabVisible(selection):
            if selection == len(self._pagesInfoVec) - 1:
                if not self.CanFitToScreen(selection):
                    event.Skip()
                    return
            else:
                event.Skip()
                return
                    
        render.DrawTabX(self, dc, self._pagesInfoVec[selection].GetXRect(), selection, self._nTabXButtonStatus)
            
        event.Skip()


    def OnMouseEnterWindow(self, event):
        """ Handles the wx.EVT_ENTER_WINDOW event for L{PageContainer}. """

        self._nLeftButtonStatus = FNB_BTN_NONE
        self._nXButtonStatus = FNB_BTN_NONE
        self._nRightButtonStatus = FNB_BTN_NONE
        self._nLeftClickZone = FNB_BTN_NONE
        self._nArrowDownButtonStatus = FNB_BTN_NONE

        event.Skip()


    def ShowTabTooltip(self, tabIdx):
        """ Shows a tab tooltip. """

        pWindow = self._pParent.GetPage(tabIdx)
        
        if pWindow:        
            pToolTip = pWindow.GetToolTip()
            if pToolTip and pToolTip.GetWindow() == pWindow:
                self.SetToolTipString(pToolTip.GetTip())
        

    def SetPageImage(self, page, imgindex):
        """ Sets the image index associated to a page. """

        if page < len(self._pagesInfoVec):
        
            self._pagesInfoVec[page].SetImageIndex(imgindex)
            self.Refresh()


    def GetPageImage(self, page):
        """ Returns the image index associated to a page. """

        if page < len(self._pagesInfoVec):
        
            return self._pagesInfoVec[page].GetImageIndex()
        
        return -1


    def OnDropTarget(self, x, y, nTabPage, wnd_oldContainer):
        """ Handles the drop action from a DND operation. """

        # Disable drag'n'drop for disabled tab
        if not wnd_oldContainer._pagesInfoVec[nTabPage].GetEnabled():
            return wx.DragCancel

        self._isdragging = True
        oldContainer = wnd_oldContainer
        nIndex = -1

        where, nIndex = self.HitTest(wx.Point(x, y))

        oldNotebook = oldContainer.GetParent()
        newNotebook = self.GetParent()

        if oldNotebook == newNotebook:
        
            if nTabPage >= 0:
            
                if where == FNB_TAB:
                    self.MoveTabPage(nTabPage, nIndex)
                           
        elif self.GetParent().GetWindowStyleFlag() & FNB_ALLOW_FOREIGN_DND:
        
            if wx.Platform in ["__WXMSW__", "__WXGTK__"]:
                if nTabPage >= 0:
                
                    window = oldNotebook.GetPage(nTabPage)

                    if window:
                        where, nIndex = newNotebook._pages.HitTest(wx.Point(x, y))
                        caption = oldContainer.GetPageText(nTabPage)
                        imageindex = oldContainer.GetPageImage(nTabPage)
                        oldNotebook.RemovePage(nTabPage)
                        window.Reparent(newNotebook)

                        if imageindex >= 0:

                            bmp = oldNotebook.GetImageList().GetIcon(imageindex)
                            newImageList = newNotebook.GetImageList()
    
                            if not newImageList:
                                xbmp, ybmp = bmp.GetWidth(), bmp.GetHeight()
                                newImageList = wx.ImageList(xbmp, ybmp)                                
                                imageindex = 0
                            else:
                                imageindex = newImageList.GetImageCount()

                            newImageList.AddIcon(bmp)
                            newNotebook.SetImageList(newImageList)
                                
                        newNotebook.InsertPage(nIndex, window, caption, True, imageindex)

        self._isdragging = False
        
        return wx.DragMove


    def MoveTabPage(self, nMove, nMoveTo):
        """ Moves a tab inside the same L{FlatNotebook}. """

        if nMove == nMoveTo:
            return

        elif nMoveTo < len(self._pParent._windows):
            nMoveTo = nMoveTo + 1

        self._pParent.Freeze()
        
        # Remove the window from the main sizer
        nCurSel = self._pParent._pages.GetSelection()
        self._pParent._mainSizer.Detach(self._pParent._windows[nCurSel])
        self._pParent._windows[nCurSel].Hide()

        pWindow = self._pParent._windows[nMove]
        self._pParent._windows.pop(nMove)
        self._pParent._windows.insert(nMoveTo-1, pWindow)

        pgInfo = self._pagesInfoVec[nMove]

        self._pagesInfoVec.pop(nMove)
        self._pagesInfoVec.insert(nMoveTo - 1, pgInfo)

        # Add the page according to the style
        pSizer = self._pParent._mainSizer
        style = self.GetParent().GetWindowStyleFlag()

        if style & FNB_BOTTOM:
        
            pSizer.Insert(0, pWindow, 1, wx.EXPAND)
        
        else:
        
            # We leave a space of 1 pixel around the window
            pSizer.Add(pWindow, 1, wx.EXPAND)
        
        pWindow.Show()

        pSizer.Layout()
        self._iActivePage = nMoveTo - 1
        self._iPreviousActivePage = -1
        self.DoSetSelection(self._iActivePage)
        self.Refresh()
        self._pParent.Thaw()


    def CanFitToScreen(self, page):
        """ Returns wheter a tab can fit in the left space in the screen or not. """

        # Incase the from is greater than page,
        # we need to reset the self._nFrom, so in order
        # to force the caller to do so, we return false
        if self._nFrom > page:
            return False

        style = self.GetParent().GetWindowStyleFlag()
        render = self._mgr.GetRenderer(style)

        if not self.HasFlag(FNB_VC8):
            rect = self.GetClientRect();
            clientWidth = rect.width;
            tabHeight = render.CalcTabHeight(self)
            tabWidth = render.CalcTabWidth(self, page, tabHeight)

            posx = self._pParent._nPadding

            if self._nFrom >= 0:

                for i in xrange(self._nFrom, len(self._pagesInfoVec)):

                    if self._pagesInfoVec[i].GetPosition() == wx.Point(-1, -1):
                        break
                    
                    posx += self._pagesInfoVec[i].GetSize().x

            if posx + tabWidth + render.GetButtonsAreaLength(self) >= clientWidth:
                return False

            return True

        else:

            # TODO:: this is ugly and should be improved, we should *never* access the
            # raw pointer directly like we do here (render.Get())
            vc8_render = render
            vTabInfo = vc8_render.NumberTabsCanFit(self)

            if page - self._nFrom >= len(vTabInfo):
                return False
            
            return True


    def GetNumOfVisibleTabs(self):
        """ Returns the number of visible tabs. """

        count = 0
        for ii in xrange(self._nFrom, len(self._pagesInfoVec)):
            if self._pagesInfoVec[ii].GetPosition() == wx.Point(-1, -1):
                break
            count = count + 1

        return count


    def GetEnabled(self, page):
        """ Returns whether a tab is enabled or not. """

        if page >= len(self._pagesInfoVec):
            return True # Seems strange, but this is the default
        
        return self._pagesInfoVec[page].GetEnabled()


    def Enable(self, page, enabled=True):
        """ Enables or disables a tab. """

        if page >= len(self._pagesInfoVec):
            return
        
        self._pagesInfoVec[page].Enable(enabled)
        

    def GetSingleLineBorderColour(self):
        """ Returns the colour for the single line border. """

        if self.HasFlag(FNB_FANCY_TABS):
            return self._colorFrom
        
        return wx.WHITE


    def HasFlag(self, flag):
        """ Returns whether a flag is present in the L{FlatNotebook} style. """

        style = self.GetParent().GetWindowStyleFlag()
        res = (style & flag and [True] or [False])[0]
        return res


    def ClearFlag(self, flag):
        """ Deletes a flag from the L{FlatNotebook} style. """

        style = self.GetParent().GetWindowStyleFlag()
        style &= ~flag 
        self.SetWindowStyleFlag(style)


    def TabHasImage(self, tabIdx):
        """ Returns whether a tab has an associated image index or not. """

        if self._ImageList:
            return self._pagesInfoVec[tabIdx].GetImageIndex() != -1
        
        return False


    def OnLeftDClick(self, event):
        """ Handles the wx.EVT_LEFT_DCLICK event for L{PageContainer}. """

        if self.HasFlag(FNB_DCLICK_CLOSES_TABS):
        
            where, tabIdx = self.HitTest(event.GetPosition())
            
            if where == FNB_TAB:
                self.DeletePage(tabIdx)
        
        else:
        
            event.Skip()
        

    def PopupTabsMenu(self):
        """ Pops up the menu activated with the drop down arrow in the navigation area. """

        popupMenu = wx.Menu()

        for i in xrange(len(self._pagesInfoVec)):
            pi = self._pagesInfoVec[i]
            item = wx.MenuItem(popupMenu, i, pi.GetCaption(), pi.GetCaption(), wx.ITEM_NORMAL)
            self.Bind(wx.EVT_MENU, self.OnTabMenuSelection, item)

            # This code is commented, since there is an alignment problem with wx2.6.3 & Menus
            # if self.TabHasImage(ii):
            #   item.SetBitmaps( (*m_ImageList)[pi.GetImageIndex()] );

            popupMenu.AppendItem(item)
            item.Enable(pi.GetEnabled())
            
        self.PopupMenu(popupMenu)


    def OnTabMenuSelection(self, event):
        """ Handles the wx.EVT_MENU event for L{PageContainer}. """

        selection = event.GetId()
        self.FireEvent(selection)


    def FireEvent(self, selection):
        """
        Fires the wxEVT_FLATNOTEBOOK_PAGE_CHANGING and wxEVT_FLATNOTEBOOK_PAGE_CHANGED events
        called from other methods (from menu selection or Smart Tabbing).
        Utility function.
        """

        if selection == self._iActivePage:
            # No events for the same selection
            return
        
        oldSelection = self._iActivePage

        event = FlatNotebookEvent(wxEVT_FLATNOTEBOOK_PAGE_CHANGING, self.GetParent().GetId())
        event.SetSelection(selection)
        event.SetOldSelection(oldSelection)
        event.SetEventObject(self.GetParent())
        
        if not self.GetParent().GetEventHandler().ProcessEvent(event) or event.IsAllowed():
        
            self.SetSelection(selection)

            # Fire a wxEVT_FLATNOTEBOOK_PAGE_CHANGED event
            event.SetEventType(wxEVT_FLATNOTEBOOK_PAGE_CHANGED)
            event.SetOldSelection(oldSelection)
            self.GetParent().GetEventHandler().ProcessEvent(event)
            

    def SetImageList(self, imglist):
        """ Sets the image list for the page control. """

        self._ImageList = imglist


    def GetImageList(self):
        """ Returns the image list for the page control. """

        return self._ImageList


    def GetSelection(self):
        """ Returns the current selected page. """

        return self._iActivePage 


    def GetPageCount(self):
        """ Returns the number of tabs in the L{FlatNotebook} control. """

        return len(self._pagesInfoVec)


    def GetPageText(self, page):
        """ Returns the tab caption of the page. """

        return self._pagesInfoVec[page].GetCaption() 


    def SetPageText(self, page, text):
        """ Sets the tab caption of the page. """

        self._pagesInfoVec[page].SetCaption(text)
        return True 


    def DrawDragHint(self):
        """ Draws small arrow at the place that the tab will be placed. """

        # get the index of tab that will be replaced with the dragged tab
        pt = wx.GetMousePosition()
        client_pt = self.ScreenToClient(pt)
        where, tabIdx = self.HitTest(client_pt)
        self._mgr.GetRenderer(self.GetParent().GetWindowStyleFlag()).DrawDragHint(self, tabIdx)

