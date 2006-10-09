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
# Latest Revision: 05 Oct 2006, 20.00 GMT
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
- Currently there are 4 differnt styles - VC8, VC 71, Standard and Fancy. 
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
- Colourful tabs - a random gentle colour is generated for each new tab (very cool,
  VC8 style only)

And much more.


Usage:

The following example shows a simple implementation that uses FlatNotebook inside
a very simple frame::

  import wx
  import wx.lib.flatnotebook as FNB
  
  class MyFrame(wx.Frame):
  
      def __init__(self, parent, id=wx.ID_ANY, title="", pos=wx.DefaultPosition, size=(800, 600),
                   style=wx.DEFAULT_FRAME_STYLE | wx.MAXIMIZE |wx.NO_FULL_REPAINT_ON_RESIZE):
  
          wx.Frame.__init__(self, parent, id, title, pos, size, style)
  
          mainSizer = wx.BoxSizer(wx.VERTICAL)
          self.SetSizer(mainSizer)
  
          bookStyle = FNB.FNB_TABS_BORDER_SIMPLE
  
          self.book = FNB.StyledNotebook(self, wx.ID_ANY, style=bookStyle)
          mainSizer.Add(self.book, 1, wx.EXPAND)
  
          # Add some pages to the notebook
          self.Freeze()
  
          text = wx.TextCtrl(self.book, -1, "Book Page 1", style=wx.TE_MULTILINE)  
          self.book.AddPage(text, "Book Page 1")
  
          text = wx.TextCtrl(self.book, -1, "Book Page 2", style=wx.TE_MULTILINE)
          self.book.AddPage(text,  "Book Page 2")
  
          self.Thaw()	
  
          mainSizer.Layout()
          self.SendSizeEvent()
  
  # our normal wxApp-derived class, as usual
  
  app = wx.PySimpleApp()

  frame = MyFrame(None)
  app.SetTopWindow(frame)
  frame.Show()
  
  app.MainLoop()
  
  
License And Version:

FlatNotebook Is Freeware And Distributed Under The wxPython License. 

Latest Revision: Andrea Gavana @ 05 Oct 2006, 20.00 GMT
Version 0.4.

"""

import wx
import random
import math
import weakref
import cPickle

# Check for the new method in 2.7 (not present in 2.6.3.3)
if wx.VERSION_STRING < "2.7":
    wx.Rect.Contains = lambda self, point: wx.Rect.Inside(self, point)

FNB_HEIGHT_SPACER = 10

# Use Visual Studio 2003 (VC7.1) Style for tabs
FNB_VC71 = 1

# Use fancy style - square tabs filled with gradient coloring
FNB_FANCY_TABS = 2

# Draw thin border around the page
FNB_TABS_BORDER_SIMPLE = 4

# Do not display the 'X' button
FNB_NO_X_BUTTON = 8

# Do not display the Right / Left arrows
FNB_NO_NAV_BUTTONS = 16

# Use the mouse middle button for cloing tabs
FNB_MOUSE_MIDDLE_CLOSES_TABS = 32

# Place tabs at bottom - the default is to place them
# at top
FNB_BOTTOM = 64

# Disable dragging of tabs
FNB_NODRAG = 128

# Use Visual Studio 2005 (VC8) Style for tabs
FNB_VC8 = 256

# Place 'X' on a tab
# Note: This style is not supported on VC8 style
FNB_X_ON_TAB = 512

FNB_BACKGROUND_GRADIENT = 1024

FNB_COLORFUL_TABS = 2048

# Style to close tab using double click - styles 1024, 2048 are reserved
FNB_DCLICK_CLOSES_TABS = 4096

VERTICAL_BORDER_PADDING = 4

# Button size is a 16x16 xpm bitmap
BUTTON_SPACE = 16

VC8_SHAPE_LEN = 16

MASK_COLOR  = wx.Color(0, 128, 128)

# Button status
FNB_BTN_PRESSED = 2
FNB_BTN_HOVER = 1
FNB_BTN_NONE = 0


# Hit Test results
FNB_TAB = 1             # On a tab
FNB_X = 2               # On the X button
FNB_TAB_X = 3           # On the 'X' button (tab's X button)
FNB_LEFT_ARROW = 4      # On the rotate left arrow button
FNB_RIGHT_ARROW = 5     # On the rotate right arrow button
FNB_NOWHERE = 0         # Anywhere else

FNB_DEFAULT_STYLE = FNB_MOUSE_MIDDLE_CLOSES_TABS

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
EVT_FLATNOTEBOOK_PAGE_CHANGING = wx.PyEventBinder(wxEVT_FLATNOTEBOOK_PAGE_CHANGING, 1)
EVT_FLATNOTEBOOK_PAGE_CLOSING = wx.PyEventBinder(wxEVT_FLATNOTEBOOK_PAGE_CLOSING, 1)
EVT_FLATNOTEBOOK_PAGE_CLOSED = wx.PyEventBinder(wxEVT_FLATNOTEBOOK_PAGE_CLOSED, 1)
EVT_FLATNOTEBOOK_PAGE_CONTEXT_MENU = wx.PyEventBinder(wxEVT_FLATNOTEBOOK_PAGE_CONTEXT_MENU, 1)

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
    return wx.Color(r, g, b)


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

        p = wx.Pen(wx.Color(r, g, b))
        dc.SetPen(p)

        if vertical:
            dc.DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i)
        else:
            dc.DrawLine(rect.x+i, rect.y, rect.x+i, rect.y+rect.height)
    
    # Restore the pen and brush
    dc.SetPen(savedPen)
    dc.SetBrush(savedBrush)


def RandomColor(): 
    """ Creates a random colour. """
    
    r = random.randint(0, 255) # Random value betweem 0-255
    g = random.randint(0, 255) # Random value betweem 0-255
    b = random.randint(0, 255) # Random value betweem 0-255

    return wx.Color(r, g, b)


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
        """ Returns the FlatNotebook page (usually a panel). """
        
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

    def __init__(self, caption="", imageindex=-1, tabangle=0, enabled=True):
        """
        Default Class Constructor.

        Parameters:
        - caption: the tab caption;
        - imageindex: the tab image index based on the assigned (set) wx.ImageList (if any);
        - tabangle: the tab angle (only on standard tabs, from 0 to 15 degrees);
        - enabled: sets enabled or disabled the tab.
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


    def GetColor(self):
        """ Returns the tab colour. """

        return self._color 


    def SetColor(self, color):
        """ Sets the tab colour. """

        self._color = color 


# ---------------------------------------------------------------------------- #
# Class FlatNotebookEvent
# ---------------------------------------------------------------------------- #

class FlatNotebookEvent(wx.PyCommandEvent):
    """
    This events will be sent when a EVT_FLATNOTEBOOK_PAGE_CHANGED,
    EVT_FLATNOTEBOOK_PAGE_CHANGING And EVT_FLATNOTEBOOK_PAGE_CLOSING is mapped in
    the parent.
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
# Class FlatNotebookBase
# ---------------------------------------------------------------------------- #

class FlatNotebookBase(wx.Panel):

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

        wx.Panel.__init__(self, parent, id, pos, size, style)
        
        self._pages = StyledTabsContainer(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, style)

        self.Bind(wx.EVT_NAVIGATION_KEY, self.OnNavigationKey)
        
        self._pages._colorBorder = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW)

        self._mainSizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self._mainSizer)

        # The child panels will inherit this bg color, so leave it at the default value
        #self.SetBackgroundColour(wx.SystemSettings.GetColour(wx.SYS_COLOUR_APPWORKSPACE))

        # Add the tab container to the sizer
        self._mainSizer.Insert(0, self._pages, 0, wx.EXPAND)

        # Set default page height
        dc = wx.ClientDC(self)
        font = self.GetFont()
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        dc.SetFont(font)
        height = dc.GetCharHeight()
        ##print height, font.Ok()

        tabHeight = height + FNB_HEIGHT_SPACER         # We use 8 pixels as padding
        self._pages.SetSizeHints(-1, tabHeight)

        self._mainSizer.Layout()

        self._pages._nFrom = self._nFrom
        self._pDropTarget = FNBDropTarget(self)
        self.SetDropTarget(self._pDropTarget)


    def CreatePageContainer(self):
        """ Creates the page container for the tabs. """

        return PageContainerBase(self, wx.ID_ANY)


    def SetActiveTabTextColour(self, textColour):
        """ Sets the text colour for the active tab. """

        self._pages._activeTextColor = textColour


    def OnDropTarget(self, x, y, nTabPage, wnd_oldContainer):
        """ Handles the drop action from a DND operation. """

        return self._pages.OnDropTarget(x, y, nTabPage, wnd_oldContainer)


    def AddPage(self, window, caption, selected=True, imgindex=-1):
        """
        Add a page to the FlatNotebook.

        Parameters:
        - window: Specifies the new page.
        - caption: Specifies the text for the new page.
        - selected: Specifies whether the page should be selected.
        - imgindex: Specifies the optional image index for the new page.
        
        Return value:
        True if successful, False otherwise.
        """

        # sanity check
        if not window:
            return False

        # reparent the window to us
        window.Reparent(self)

        # Add tab
        bSelected = selected or not self._windows
        curSel = self._pages.GetSelection()

        if not self._pages.IsShown():
            self._pages.Show()

        self._pages.AddPage(caption, bSelected, imgindex)
        self._windows.append(window)

        self.Freeze()

        # Check if a new selection was made
        if bSelected:
        
            if curSel >= 0:
            
                # Remove the window from the main sizer
                self._mainSizer.Detach(self._windows[curSel])
                self._windows[curSel].Hide()
            
            if self.GetWindowStyleFlag() & FNB_BOTTOM:
            
                self._mainSizer.Insert(0, window, 1, wx.EXPAND)
            
            else:
            
                # We leave a space of 1 pixel around the window
                self._mainSizer.Add(window, 1, wx.EXPAND)
            
        else:
        
            # Hide the page
            window.Hide()
        
        self._mainSizer.Layout()
        self.Thaw()
        self.Refresh()

        return True        


    def SetImageList(self, imglist):
        """
        Sets the image list for the page control. It does not take ownership
        of the image list, you must delete it yourself.
        """

        self._pages.SetImageList(imglist)


    def GetImageList(self):
        """ Returns the associated image list. """
        
        return self._pages.GetImageList()


    def InsertPage(self, indx, page, text, select=True, imgindex=-1):
        """
        Inserts a new page at the specified position.

        Parameters:
        - indx: Specifies the position of the new page.
        - page: Specifies the new page.
        - text: Specifies the text for the new page.
        - select: Specifies whether the page should be selected.
        - imgindex: Specifies the optional image index for the new page.
        
        Return value:
        True if successful, False otherwise.
        """     

        # sanity check
        if not page:
            return False

        # reparent the window to us
        page.Reparent(self)

        if not self._windows:
        
            self.AddPage(page, text, select, imgindex)
            return True

        # Insert tab
        bSelected = select or not self._windows
        curSel = self._pages.GetSelection()
        
        indx = max(0, min(indx, len(self._windows)))

        if indx <= len(self._windows):
        
            self._windows.insert(indx, page)
        
        else:
        
            self._windows.append(page)
        
        self._pages.InsertPage(indx, text, bSelected, imgindex)
        
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
        
        self._mainSizer.Layout()
        self._pages._iActivePage = page

        self.Thaw()
        
        self._pages.DoSetSelection(page)


    def DeletePage(self, page):
        """
        Deletes the specified page, and the associated window.
        The call to this function generates the page changing events.
        """

        if page >= len(self._windows):
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


    def AdvanceSelection(self, bForward=True):
        """
        Cycles through the tabs.
        The call to this function generates the page changing events.
        """

        self._pages.AdvanceSelection(bForward)


    def GetPageCount(self):
        """ Returns the number of pages in the FlatNotebook control. """
        return self._pages.GetPageCount()


    def OnNavigationKey(self, event):
        """ Handles the wx.EVT_NAVIGATION_KEY event for FlatNotebook. """

        if event.IsWindowChange():
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
        """ Sets the FlatNotebook window style flags. """

        wx.Panel.SetWindowStyleFlag(self, style)

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


    def GetPageText(self, page):
        """ Returns the tab caption. """

        return self._pages.GetPageText(page)


    def SetGradientColors(self, fr, to, border):
        """ Sets the gradient colours for the tab. """

        self._pages._colorFrom = fr
        self._pages._colorTo   = to
        self._pages._colorBorder = border


    def SetGradientColorFrom(self, fr):
        """ Sets the starting colour for the gradient. """

        self._pages._colorFrom = fr


    def SetGradientColorTo(self, to):
        """ Sets the ending colour for the gradient. """

        self._pages._colorTo = to


    def SetGradientColorBorder(self, border):
        """ Sets the tab border colour. """

        self._pages._colorBorder = border


    def GetGradientColorFrom(self):
        """ Gets first gradient colour. """

        return self._pages._colorFrom


    def GetGradientColorTo(self):
        """ Gets second gradient colour. """

        return self._pages._colorTo


    def GetGradientColorBorder(self):
        """ Gets the tab border colour. """

        return self._pages._colorBorder


    def GetActiveTabTextColour(self):
        """ Get the active tab text colour. """

        return self._pages._activeTextColor


    def SetPageImage(self, page, imgindex):
        """
        Sets the image index for the given page. Image is an index into the
        image list which was set with SetImageList.
        """

        self._pages.SetPageImage(page, imgindex)


    def GetPageImageIndex(self, page):
        """
        Returns the image index for the given page. Image is an index into the
        image list which was set with SetImageList.
        """

        return self._pages.GetPageImageIndex(page)


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
# Class PageContainerBase
# Acts as a container for the pages you add to FlatNotebook
# ---------------------------------------------------------------------------- #

class PageContainerBase(wx.Panel):

    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0):
        """ Default class constructor. """
        
        self._ImageList = None
        self._iActivePage = -1
        self._pDropTarget = None
        self._nLeftClickZone = FNB_NOWHERE
        self._tabXBgBmp = wx.EmptyBitmap(16, 16)
        self._xBgBmp = wx.EmptyBitmap(16, 14)
        self._leftBgBmp = wx.EmptyBitmap(16, 14)
        self._rightBgBmp = wx.EmptyBitmap(16, 14)

        self._pRightClickMenu = None
        self._nXButtonStatus = FNB_BTN_NONE
        self._pParent = parent
        self._nRightButtonStatus = FNB_BTN_NONE
        self._nLeftButtonStatus = FNB_BTN_NONE
        self._nTabXButtonStatus = FNB_BTN_NONE

        self._pagesInfoVec = []        

        self._colorTo = wx.SystemSettings.GetColour(wx.SYS_COLOUR_ACTIVECAPTION)
        self._colorFrom = wx.WHITE
        self._activeTabColor = wx.WHITE
        self._activeTextColor = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNTEXT)
        self._nonActiveTextColor = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW)
        self._tabAreaColor = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE)

        self._nFrom = 0
        self._isdragging = False
        
        wx.Panel.__init__(self, parent, id, pos, size, style|wx.TAB_TRAVERSAL)

        self._pDropTarget = FNBDropTarget(self)
        self.SetDropTarget(self._pDropTarget)

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


    def GetButtonAreaWidth(self):
        """ Returns the width of the navigation button area. """

        style = self.GetParent().GetWindowStyleFlag()
        btnareawidth = 2*self._pParent._nPadding

        if style & FNB_NO_X_BUTTON == 0:
            btnareawidth += BUTTON_SPACE

        if style & FNB_NO_NAV_BUTTONS == 0:
            btnareawidth += 2*BUTTON_SPACE

        return btnareawidth


    def OnEraseBackground(self, event):
        """ Handles the wx.EVT_ERASE_BACKGROUND event for PageContainerBase (does nothing)."""

        pass

    
    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for PageContainerBase."""

        dc = wx.BufferedPaintDC(self)

        if "__WXMAC__" in wx.PlatformInfo:
            # Works well on MSW & GTK, however this lines should be skipped on MAC
            if len(self._pagesInfoVec) == 0 or self._nFrom >= len(self._pagesInfoVec):
                self.Hide()
                event.Skip()
                return
        
        # Get the text hight
        style = self.GetParent().GetWindowStyleFlag()

        # For GTK it seems that we must do this steps in order
        # for the tabs will get the proper height on initialization
        # on MSW, preforming these steps yields wierd results
        normalFont = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        boldFont = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        boldFont.SetWeight(wx.FONTWEIGHT_BOLD)
        if "__WXGTK__" in wx.PlatformInfo:
            dc.SetFont(boldFont)

        width, height = dc.GetTextExtent("Tp")

        tabHeight = height + FNB_HEIGHT_SPACER   # We use 8 pixels as padding

        # Calculate the number of rows required for drawing the tabs
        rect = self.GetClientRect()
        clientWidth = rect.width

        # Set the maximum client size
        self.SetSizeHints(self.GetButtonsAreaLength(), tabHeight)
        borderPen = wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW))

        if style & FNB_VC71:
            backBrush = wx.Brush(wx.Colour(247, 243, 233))
        else:
            backBrush = wx.Brush(self._tabAreaColor)

        noselBrush = wx.Brush(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE))
        selBrush = wx.Brush(self._activeTabColor)

        size = self.GetSize()

        # Background
        dc.SetTextBackground((style & FNB_VC71 and [wx.Colour(247, 243, 233)] or [self.GetBackgroundColour()])[0])
        dc.SetTextForeground(self._activeTextColor)
        dc.SetBrush(backBrush)

        # If border style is set, set the pen to be border pen
        if style & FNB_TABS_BORDER_SIMPLE:
            dc.SetPen(borderPen)
        else:
            pc = (self.HasFlag(FNB_VC71) and [wx.Colour(247, 243, 233)] or [self.GetBackgroundColour()])[0]
            dc.SetPen(wx.Pen(pc))
        
        dc.DrawRectangle(0, 0, size.x, size.y)

        # Take 3 bitmaps for the background for the buttons
        
        mem_dc = wx.MemoryDC()

        #---------------------------------------
        # X button
        #---------------------------------------
        rect = wx.Rect(self.GetXPos(), 6, 16, 14)
        mem_dc.SelectObject(self._xBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)

        #---------------------------------------
        # Right button
        #---------------------------------------
        rect = wx.Rect(self.GetRightButtonPos(), 6, 16, 14)
        mem_dc.SelectObject(self._rightBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)

        #---------------------------------------
        # Left button
        #---------------------------------------
        rect = wx.Rect(self.GetLeftButtonPos(), 6, 16, 14)
        mem_dc.SelectObject(self._leftBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)
        
        # We always draw the bottom/upper line of the tabs
        # regradless the style
        dc.SetPen(borderPen)
        self.DrawTabsLine(dc)

        # Restore the pen
        dc.SetPen(borderPen)

        if self.HasFlag(FNB_VC71):
        
            greyLineYVal = self.HasFlag((FNB_BOTTOM and [0] or [size.y - 2])[0])
            whiteLineYVal = self.HasFlag((FNB_BOTTOM and [3] or [size.y - 3])[0])

            pen = wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DFACE))
            dc.SetPen(pen)

            # Draw thik grey line between the windows area and
            # the tab area

            for num in xrange(3):
                dc.DrawLine(0, greyLineYVal + num, size.x, greyLineYVal + num)

            wbPen = wx.Pen((self.HasFlag(FNB_BOTTOM) and [wx.BLACK] or [wx.WHITE])[0])
            dc.SetPen(wbPen)
            dc.DrawLine(1, whiteLineYVal, size.x - 1, whiteLineYVal)

            # Restore the pen
            dc.SetPen(borderPen)
        
        if "__WXMAC__" in wx.PlatformInfo:
            # On MAC, Add these lines so the tab background gets painted
            if len(self._pagesInfoVec) == 0 or self._nFrom >= len(self._pagesInfoVec):
                self.Hide()
                return
            
        # Draw labels
        dc.SetFont(boldFont)
        posx = self._pParent.GetPadding()

        # Update all the tabs from 0 to '_nFrom' to be non visible
        for ii in xrange(self._nFrom):
            self._pagesInfoVec[ii].SetPosition(wx.Point(-1, -1))
            self._pagesInfoVec[ii].GetRegion().Clear()

        if style & FNB_VC71:
            tabHeight = ((style & FNB_BOTTOM) and [tabHeight - 4] or [tabHeight])[0]
        elif style & FNB_FANCY_TABS:
            tabHeight = ((style & FNB_BOTTOM) and [tabHeight - 2] or [tabHeight])[0]

        #----------------------------------------------------------
        # Go over and draw the visible tabs
        #----------------------------------------------------------

        count = self._nFrom
        
        for ii in xrange(self._nFrom, len(self._pagesInfoVec)):
        
            if style != FNB_VC71:
                shapePoints = int(tabHeight*math.tan(float(self._pagesInfoVec[ii].GetTabAngle())/180.0*math.pi))
            else:
                shapePoints = 0

            dc.SetPen(borderPen)
            dc.SetBrush((ii==self.GetSelection() and [selBrush] or [noselBrush])[0])

            # Calculate the text length using the bold font, so when selecting a tab
            # its width will not change
            dc.SetFont(boldFont)
            width, pom = dc.GetTextExtent(self.GetPageText(ii))

            # Now set the font to the correct font
            dc.SetFont((ii==self.GetSelection() and [boldFont] or [normalFont])[0])

            # Set a minimum size to a tab
            if width < 20:
                width = 20

            # Add the padding to the tab width
            # Tab width:
            # +-----------------------------------------------------------+
            # | PADDING | IMG | IMG_PADDING | TEXT | PADDING | x |PADDING |
            # +-----------------------------------------------------------+

            tabWidth = 2*self._pParent._nPadding + width
            imageYCoord = (self.HasFlag(FNB_BOTTOM) and [6] or [8])[0]

            # Style to add a small 'x' button on the top right
            # of the tab
            if style & FNB_X_ON_TAB and ii == self.GetSelection():
            
                # The xpm image that contains the 'x' button is 9 pixles
                tabWidth += self._pParent._nPadding + 9
            
            if not (style & FNB_VC71) and not (style & FNB_FANCY_TABS):
                # Default style
                tabWidth += 2*shapePoints

            hasImage = self._ImageList != None and self._pagesInfoVec[ii].GetImageIndex() != -1

            # For VC71 style, we only add the icon size (16 pixels)
            if hasImage:
            
                if style & FNB_VC71 or style & FNB_FANCY_TABS:
                    tabWidth += 16 + self._pParent._nPadding
                else:
                    # Default style
                    tabWidth += 16 + self._pParent._nPadding + shapePoints/2
            
            # Check if we can draw more
            if posx + tabWidth + self.GetButtonsAreaLength() >= clientWidth:
                break
            
            count = count + 1            

            # By default we clean the tab region
            self._pagesInfoVec[ii].GetRegion().Clear()

            # Clean the 'x' buttn on the tab
            # 'Clean' rectanlge is a rectangle with width or height
            # with values lower than or equal to 0
            self._pagesInfoVec[ii].GetXRect().SetSize(wx.Size(-1, -1))

            # Draw the tab
            if style & FNB_FANCY_TABS:
                self.DrawFancyTab(dc, posx, ii, tabWidth, tabHeight)
            elif style & FNB_VC71:
                self.DrawVC71Tab(dc, posx, ii, tabWidth, tabHeight)
            else:
                self.DrawStandardTab(dc, posx, ii, tabWidth, tabHeight)

            # The width of the images are 16 pixels
            if hasImage:
                textOffset = 2*self._pParent._nPadding + 16 + shapePoints/2 
            else:
                textOffset = self._pParent._nPadding + shapePoints/2 

            # After several testing, it seems that we can draw
            # the text 2 pixles to the right - this is done only
            # for the standard tabs
                
            if not self.HasFlag(FNB_FANCY_TABS):
                textOffset += 2

            if ii != self.GetSelection():
                # Set the text background to be like the vertical lines
                dc.SetTextForeground(self._nonActiveTextColor)
            
            if hasImage:
            
                imageXOffset = textOffset - 16 - self._pParent._nPadding
                self._ImageList.Draw(self._pagesInfoVec[ii].GetImageIndex(), dc,
                                     posx + imageXOffset, imageYCoord,
                                     wx.IMAGELIST_DRAW_TRANSPARENT, True)

            dc.DrawText(self.GetPageText(ii), posx + textOffset, imageYCoord)

            # From version 1.2 - a style to add 'x' button
            # on a tab
            
            if self.HasFlag(FNB_X_ON_TAB) and ii == self.GetSelection():
            
                textWidth, textHeight = dc.GetTextExtent(self.GetPageText(ii))
                tabCloseButtonXCoord = posx + textOffset + textWidth + 1

                # take a bitmap from the position of the 'x' button (the x on tab button)
                # this bitmap will be used later to delete old buttons
                tabCloseButtonYCoord = imageYCoord
                x_rect = wx.Rect(tabCloseButtonXCoord, tabCloseButtonYCoord, 16, 16)
                mem_dc = wx.MemoryDC()
                mem_dc.SelectObject(self._tabXBgBmp)
                mem_dc.Blit(0, 0, x_rect.width, x_rect.height, dc, x_rect.x, x_rect.y)
                mem_dc.SelectObject(wx.NullBitmap)

                # Draw the tab
                self.DrawTabX(dc, x_rect, ii)
            
            # Restore the text forground
            dc.SetTextForeground(self._activeTextColor)

            # Update the tab position & size
            posy = (style & FNB_BOTTOM and [0] or [VERTICAL_BORDER_PADDING])[0]
            
            self._pagesInfoVec[ii].SetPosition(wx.Point(posx, posy))
            self._pagesInfoVec[ii].SetSize(wx.Size(tabWidth, tabHeight))
            posx += tabWidth
            
        # Update all tabs that can not fit into the screen as non-visible
        for ii in xrange(count, len(self._pagesInfoVec)):
        
            self._pagesInfoVec[ii].SetPosition(wx.Point(-1, -1))
            self._pagesInfoVec[ii].GetRegion().Clear()
        
        # Draw the left/right/close buttons
        # Left arrow
        self.DrawLeftArrow(dc)
        self.DrawRightArrow(dc)
        self.DrawX(dc)


    def DrawFancyTab(self, dc, posx, tabIdx, tabWidth, tabHeight):
        """
        Fancy tabs - like with VC71 but with the following differences:
        - The Selected tab is colored with gradient color
        """
        
        borderPen = wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW))
        pen = (tabIdx==self.GetSelection() and [wx.Pen(self._colorBorder)] \
               or [wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DFACE))])[0]

        fnb_bottom = self.HasFlag(FNB_BOTTOM)

        if tabIdx == self.GetSelection():
        
            posy = (fnb_bottom and [2] or [VERTICAL_BORDER_PADDING])[0]
            th = (fnb_bottom and [tabHeight - 2] or [tabHeight - 5])[0]

            rect = wx.Rect(posx, posy, tabWidth, th)
            self.FillGradientColor(dc, rect)
            dc.SetBrush(wx.TRANSPARENT_BRUSH)
            dc.SetPen(pen)
            dc.DrawRectangleRect(rect)

            # erase the bottom/top line of the rectangle
            dc.SetPen(wx.Pen(self._colorFrom))
            if fnb_bottom:
                dc.DrawLine(rect.x, 2, rect.x + rect.width, 2)
            else:
                dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, rect.y + rect.height - 1)
        
        else:
        
            # We dont draw a rectangle for non selected tabs, but only
            # vertical line on the left
            dc.SetPen(borderPen)
            dc.DrawLine(posx + tabWidth, VERTICAL_BORDER_PADDING + 3, posx + tabWidth, tabHeight - 4)
        

    def DrawVC71Tab(self, dc, posx, tabIdx, tabWidth, tabHeight):
        """ Draws tabs with VC71 style. """

        fnb_bottom = self.HasFlag(FNB_BOTTOM)
        
        # Visual studio 7.1 style
        borderPen = wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW))
        dc.SetPen((tabIdx==self.GetSelection() and [wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DFACE))] or [borderPen])[0])
        dc.SetBrush((tabIdx==self.GetSelection() and [wx.Brush(wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DFACE))] or \
                     [wx.Brush(wx.Colour(247, 243, 233))])[0])

        if tabIdx == self.GetSelection():
        
            posy = (fnb_bottom and [0] or [VERTICAL_BORDER_PADDING])[0]
            dc.DrawRectangle(posx, posy, tabWidth, tabHeight - 1)

            # Draw a black line on the left side of the
            # rectangle
            dc.SetPen(wx.BLACK_PEN)

            blackLineY1 = VERTICAL_BORDER_PADDING
            blackLineY2 = (fnb_bottom and [self.GetSize().y - 5] or [self.GetSize().y - 3])[0]
            dc.DrawLine(posx + tabWidth, blackLineY1, posx + tabWidth, blackLineY2)

            # To give the tab more 3D look we do the following
            # Incase the tab is on top,
            # Draw a thick white line on top of the rectangle
            # Otherwise, draw a thin (1 pixel) black line at the bottom

            pen = wx.Pen((fnb_bottom and [wx.BLACK] or [wx.WHITE])[0])
            dc.SetPen(pen)
            whiteLinePosY = (fnb_bottom and [blackLineY2] or [VERTICAL_BORDER_PADDING])[0]
            dc.DrawLine(posx , whiteLinePosY, posx + tabWidth + 1, whiteLinePosY)

            # Draw a white vertical line to the left of the tab
            dc.SetPen(wx.WHITE_PEN)
            if not fnb_bottom:
                blackLineY2 += 1
                
            dc.DrawLine(posx, blackLineY1, posx, blackLineY2)
        
        else:
        
            # We dont draw a rectangle for non selected tabs, but only
            # vertical line on the left

            blackLineY1 = (fnb_bottom and [VERTICAL_BORDER_PADDING + 2] or [VERTICAL_BORDER_PADDING + 1])[0]
            blackLineY2 = self.GetSize().y - 5 
            dc.DrawLine(posx + tabWidth, blackLineY1, posx + tabWidth, blackLineY2)
        

    def DrawStandardTab(self, dc, posx, tabIdx, tabWidth, tabHeight):
        """ Draws tabs with standard style. """

        fnb_bottom = self.HasFlag(FNB_BOTTOM)
        
        # Default style
        borderPen = wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW))

        tabPoints = [wx.Point() for ii in xrange(7)]
        tabPoints[0].x = posx
        tabPoints[0].y = (fnb_bottom and [2] or [tabHeight - 2])[0]

        tabPoints[1].x = int(posx+(tabHeight-2)*math.tan(float(self._pagesInfoVec[tabIdx].GetTabAngle())/180.0*math.pi))
        tabPoints[1].y = (fnb_bottom and [tabHeight - (VERTICAL_BORDER_PADDING+2)] or [(VERTICAL_BORDER_PADDING+2)])[0]

        tabPoints[2].x = tabPoints[1].x+2
        tabPoints[2].y = (fnb_bottom and [tabHeight - VERTICAL_BORDER_PADDING] or [VERTICAL_BORDER_PADDING])[0]

        tabPoints[3].x = int(posx+tabWidth-(tabHeight-2)*math.tan(float(self._pagesInfoVec[tabIdx].GetTabAngle())/180.0*math.pi))-2
        tabPoints[3].y = (fnb_bottom and [tabHeight - VERTICAL_BORDER_PADDING] or [VERTICAL_BORDER_PADDING])[0]

        tabPoints[4].x = tabPoints[3].x+2
        tabPoints[4].y = (fnb_bottom and [tabHeight - (VERTICAL_BORDER_PADDING+2)] or [(VERTICAL_BORDER_PADDING+2)])[0]

        tabPoints[5].x = int(tabPoints[4].x+(tabHeight-2)*math.tan(float(self._pagesInfoVec[tabIdx].GetTabAngle())/180.0*math.pi))
        tabPoints[5].y = (fnb_bottom and [2] or [tabHeight - 2])[0]

        tabPoints[6].x = tabPoints[0].x
        tabPoints[6].y = tabPoints[0].y

        if tabIdx == self.GetSelection():
        
            # Draw the tab as rounded rectangle
            dc.DrawPolygon(tabPoints)
        
        else:
        
            if tabIdx != self.GetSelection() - 1:
            
                # Draw a vertical line to the right of the text
                pt1x = tabPoints[5].x
                pt1y = (fnb_bottom and [4] or [tabHeight - 6])[0]
                pt2x = tabPoints[5].x
                pt2y = (fnb_bottom and [tabHeight - 4] or [4])[0]
                dc.DrawLine(pt1x, pt1y, pt2x, pt2y)
            
        if tabIdx == self.GetSelection():
        
            savePen = dc.GetPen()
            whitePen = wx.Pen(wx.WHITE)
            whitePen.SetWidth(1)
            dc.SetPen(whitePen)

            secPt = wx.Point(tabPoints[5].x + 1, tabPoints[5].y)
            dc.DrawLine(tabPoints[0].x, tabPoints[0].y, secPt.x, secPt.y)

            # Restore the pen
            dc.SetPen(savePen)
        

    def AddPage(self, caption, selected=True, imgindex=-1):
        """
        Add a page to the FlatNotebook.

        Parameters:
        - window: Specifies the new page.
        - caption: Specifies the text for the new page.
        - selected: Specifies whether the page should be selected.
        - imgindex: Specifies the optional image index for the new page.
        
        Return value:
        True if successful, False otherwise.
        """

        if selected:
        
            self._iActivePage = len(self._pagesInfoVec)
        
        # Create page info and add it to the vector
        pageInfo = PageInfo(caption, imgindex)
        self._pagesInfoVec.append(pageInfo)
        self.Refresh()


    def InsertPage(self, indx, text, selected=True, imgindex=-1):
        """
        Inserts a new page at the specified position.

        Parameters:
        - indx: Specifies the position of the new page.
        - page: Specifies the new page.
        - text: Specifies the text for the new page.
        - select: Specifies whether the page should be selected.
        - imgindex: Specifies the optional image index for the new page.
        
        Return value:
        True if successful, False otherwise.
        """     

        if selected:
        
            self._iActivePage = len(self._pagesInfoVec)
        
        self._pagesInfoVec.insert(indx, PageInfo(text, imgindex))
        
        self.Refresh()
        return True


    def OnSize(self, event):
        """ Handles the wx.EVT_SIZE events for PageContainerBase. """

        self.Refresh() # Call on paint
        event.Skip()


    def OnMiddleDown(self, event):
        """ Handles the wx.EVT_MIDDLE_DOWN events for PageContainerBase. """

        # Test if this style is enabled
        style = self.GetParent().GetWindowStyleFlag()
        
        if not style & FNB_MOUSE_MIDDLE_CLOSES_TABS:
            return

        where, tabIdx = self.HitTest(event.GetPosition())
        
        if where == FNB_TAB:
            self.DeletePage(tabIdx)
        
        event.Skip()


    def OnRightDown(self, event):
        """ Handles the wx.EVT_RIGHT_DOWN events for PageContainerBase. """

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
        """ Handles the wx.EVT_LEFT_DOWN events for PageContainerBase. """

        # Reset buttons status
        self._nXButtonStatus     = FNB_BTN_NONE
        self._nLeftButtonStatus  = FNB_BTN_NONE
        self._nRightButtonStatus = FNB_BTN_NONE
        self._nTabXButtonStatus  = FNB_BTN_NONE

        self._nLeftClickZone, tabIdx = self.HitTest(event.GetPosition())

        if self._nLeftClickZone == FNB_LEFT_ARROW:
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

                    oldSelection = self._iActivePage

                    event = FlatNotebookEvent(wxEVT_FLATNOTEBOOK_PAGE_CHANGING, self.GetParent().GetId())
                    event.SetSelection(tabIdx)
                    event.SetOldSelection(oldSelection)
                    event.SetEventObject(self.GetParent())
                    if not self.GetParent().GetEventHandler().ProcessEvent(event) or event.IsAllowed():
                    
                        self.SetSelection(tabIdx)

                        # Fire a wxEVT_TABBEDCTRL_PAGE_CHANGED event
                        event.SetEventType(wxEVT_FLATNOTEBOOK_PAGE_CHANGED)
                        event.SetOldSelection(oldSelection)
                        self.GetParent().GetEventHandler().ProcessEvent(event)        


    def OnLeftUp(self, event):
        """ Handles the wx.EVT_LEFT_UP events for PageContainerBase. """

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
            

    def HitTest(self, pt):
        """
        HitTest method for PageContainerBase.
        Returns the flag (if any) and the hit page (if any).
        """

        fullrect = self.GetClientRect()
        btnLeftPos = self.GetLeftButtonPos()
        btnRightPos = self.GetRightButtonPos()
        btnXPos = self.GetXPos()
        style = self.GetParent().GetWindowStyleFlag()
        
        tabIdx = -1

        if not self._pagesInfoVec:        
            return FNB_NOWHERE, -1
        
        rect = wx.Rect(btnXPos, 6, 16, 16)
        if rect.Contains(pt):
            return (style & FNB_NO_X_BUTTON and [FNB_NOWHERE] or [FNB_X])[0], -1
        
        rect = wx.Rect(btnRightPos, 6, 16, 16)
        if rect.Contains(pt):
            return (style & FNB_NO_NAV_BUTTONS and [FNB_NOWHERE] or [FNB_RIGHT_ARROW])[0], -1
        
        rect = wx.Rect(btnLeftPos, 6, 16, 16)
        if rect.Contains(pt):
            return (style & FNB_NO_NAV_BUTTONS and [FNB_NOWHERE] or [FNB_LEFT_ARROW])[0], -1        

        # Test whether a left click was made on a tab
        for cur in xrange(self._nFrom, len(self._pagesInfoVec)):
        
            pgInfo = self._pagesInfoVec[cur]
        
            if pgInfo.GetPosition() == wx.Point(-1, -1):
                continue
            
            if style & FNB_X_ON_TAB and cur == self.GetSelection():
                # 'x' button exists on a tab
                if self._pagesInfoVec[cur].GetXRect().Contains(pt):                
                    return FNB_TAB_X, cur
                
            tabRect = wx.Rect(pgInfo.GetPosition().x, pgInfo.GetPosition().y, pgInfo.GetSize().x, pgInfo.GetSize().y)
            if tabRect.Contains(pt):
                # We have a match
                return FNB_TAB, cur

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
                return FNB_TAB, len(self._pagesInfoVec)-1

        # Default
        return FNB_NOWHERE, -1


    def SetSelection(self, page):
        """ Sets the selected page. """

        book = self.GetParent()
        book.SetSelection(page)
        self.DoSetSelection(page)


    def DoSetSelection(self, page):
        """ Does the actual selection of a page. """

        # Make sure that the selection is visible
        style = self.GetParent().GetWindowStyleFlag()
        if style & FNB_NO_NAV_BUTTONS:        
            # Incase that we dont have navigation buttons,
            # there is no point of checking if the tab is visible
            # Just do the refresh
            self.Refresh()
            return
        
        if page < len(self._pagesInfoVec):
            #! fix for tabfocus
            da_page = self._pParent.GetPage(page)

            # THIS IS GIVING TROUBLES!!            
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
        """ Delete the specified page from FlatNotebook. """

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

        # The delete page was the last first on the array,
        # but the book still has more pages, so we set the
        # active page to be the first one (0)
        if self._iActivePage < 0 and self._pagesInfoVec:
            self._iActivePage = 0

        # Refresh the tabs
        if self._iActivePage >= 0:
        
            book._bForceSelection = True
            book.SetSelection(self._iActivePage)
            book._bForceSelection = False
        
        if not self._pagesInfoVec:        
            # Erase the page container drawings
            dc = wx.ClientDC(self)
            dc.Clear()
        

    def DeleteAllPages(self):
        """ Deletes all the pages. """

        self._iActivePage = -1
        self._nFrom = 0
        self._pagesInfoVec = []

        # Erase the page container drawings
        dc = wx.ClientDC(self)
        dc.Clear()


    def DrawTabX(self, dc, rect, tabIdx):
        """ Draws the 'X' in the selected tab (VC8 style excluded). """

        if not self.HasFlag(FNB_X_ON_TAB) or not self.CanDrawXOnTab():
            return

        # We draw the 'x' on the active tab only
        if tabIdx != self.GetSelection() or tabIdx < 0:
            return

        # Set the bitmap according to the button status
        if self._nTabXButtonStatus == FNB_BTN_HOVER:        
            xBmp = wx.BitmapFromXPMData(x_button_hilite_xpm)
        elif self._nTabXButtonStatus == FNB_BTN_PRESSED:
            xBmp = wx.BitmapFromXPMData(x_button_pressed_xpm)
        else:
            xBmp = wx.BitmapFromXPMData(x_button_xpm)

        # Set the masking
        xBmp.SetMask(wx.Mask(xBmp, MASK_COLOR))

        # erase old button
        dc.DrawBitmap(self._tabXBgBmp, rect.x, rect.y)

        # Draw the new bitmap
        dc.DrawBitmap(xBmp, rect.x, rect.y, True)

        # Update the vectpr
        self._pagesInfoVec[tabIdx].SetXRect(rect)


    def DrawLeftArrow(self, dc):
        """ Draw the left navigation arrow. """

        style = self.GetParent().GetWindowStyleFlag()
        if style & FNB_NO_NAV_BUTTONS:
            return

        # Make sure that there are pages in the container
        if not self._pagesInfoVec:
            return

        # Set the bitmap according to the button status
        if self._nLeftButtonStatus == FNB_BTN_HOVER:
            arrowBmp = wx.BitmapFromXPMData(left_arrow_hilite_xpm)
        elif self._nLeftButtonStatus == FNB_BTN_PRESSED:
            arrowBmp = wx.BitmapFromXPMData(left_arrow_pressed_xpm)
        else:
            arrowBmp = wx.BitmapFromXPMData(left_arrow_xpm)

        if self._nFrom == 0:
            # Handle disabled arrow
            arrowBmp = wx.BitmapFromXPMData(left_arrow_disabled_xpm)
        
        arrowBmp.SetMask(wx.Mask(arrowBmp, MASK_COLOR))

        # Erase old bitmap
        posx = self.GetLeftButtonPos()
        dc.DrawBitmap(self._leftBgBmp, posx, 6)

        # Draw the new bitmap
        dc.DrawBitmap(arrowBmp, posx, 6, True)


    def DrawRightArrow(self, dc):
        """ Draw the right navigation arrow. """

        style = self.GetParent().GetWindowStyleFlag()
        if style & FNB_NO_NAV_BUTTONS:
            return

        # Make sure that there are pages in the container
        if not self._pagesInfoVec:
            return

        # Set the bitmap according to the button status
        if self._nRightButtonStatus == FNB_BTN_HOVER:        
            arrowBmp = wx.BitmapFromXPMData(right_arrow_hilite_xpm)
        elif self._nRightButtonStatus == FNB_BTN_PRESSED:
            arrowBmp = wx.BitmapFromXPMData(right_arrow_pressed_xpm)
        else:
            arrowBmp = wx.BitmapFromXPMData(right_arrow_xpm)

        # Check if the right most tab is visible, if it is
        # don't rotate right anymore
        if self._pagesInfoVec[-1].GetPosition() != wx.Point(-1, -1):
            arrowBmp = wx.BitmapFromXPMData(right_arrow_disabled_xpm)
        
        arrowBmp.SetMask(wx.Mask(arrowBmp, MASK_COLOR))

        # erase old bitmap
        posx = self.GetRightButtonPos()
        dc.DrawBitmap(self._rightBgBmp, posx, 6)

        # Draw the new bitmap
        dc.DrawBitmap(arrowBmp, posx, 6, True)


    def DrawX(self, dc):
        """ Draw the 'X' navigation button in the navigation area. """

        # Check if this style is enabled
        style = self.GetParent().GetWindowStyleFlag()
        if style & FNB_NO_X_BUTTON:
            return

        # Make sure that there are pages in the container
        if not self._pagesInfoVec:
            return

        # Set the bitmap according to the button status
        if self._nXButtonStatus == FNB_BTN_HOVER:
            xbmp = wx.BitmapFromXPMData(x_button_hilite_xpm)
        elif self._nXButtonStatus == FNB_BTN_PRESSED:
            xbmp = wx.BitmapFromXPMData(x_button_pressed_xpm)
        else:
            xbmp = wx.BitmapFromXPMData(x_button_xpm)

        xbmp.SetMask(wx.Mask(xbmp, MASK_COLOR))
        # erase old bitmap

        posx = self.GetXPos()        
        dc.DrawBitmap(self._xBgBmp, posx, 6)

        # Draw the new bitmap
        dc.DrawBitmap(xbmp, posx, 6, True)


    def OnMouseMove(self, event):
        """ Handles the wx.EVT_MOTION for PageContainerBase. """

        if self._pagesInfoVec and self.IsShown():
        
            xButtonStatus = self._nXButtonStatus
            xTabButtonStatus = self._nTabXButtonStatus
            rightButtonStatus = self._nRightButtonStatus
            leftButtonStatus = self._nLeftButtonStatus
            style = self.GetParent().GetWindowStyleFlag()

            self._nXButtonStatus = FNB_BTN_NONE
            self._nRightButtonStatus = FNB_BTN_NONE
            self._nLeftButtonStatus = FNB_BTN_NONE
            self._nTabXButtonStatus = FNB_BTN_NONE

            where, tabIdx = self.HitTest(event.GetPosition())
            
            if where == FNB_X:
                if event.LeftIsDown():
                
                    self._nXButtonStatus = (self._nLeftClickZone==FNB_X and [FNB_BTN_PRESSED] or [FNB_BTN_NONE])[0]
                
                else:
                
                    self._nXButtonStatus = FNB_BTN_HOVER
                
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
                if event.LeftIsDown() and not (style & FNB_NODRAG):

                    self._isdragging = True                
                    draginfo = FNBDragInfo(self, tabIdx)
                    drginfo = cPickle.dumps(draginfo)
                    dataobject = wx.CustomDataObject(wx.CustomDataFormat("FlatNotebook"))
                    dataobject.SetData(drginfo)
                    dragSource = wx.DropSource(self)
                    dragSource.SetData(dataobject)
                    dragSource.DoDragDrop(wx.Drag_DefaultMove)
                    
            bRedrawX = self._nXButtonStatus != xButtonStatus
            bRedrawRight = self._nRightButtonStatus != rightButtonStatus
            bRedrawLeft = self._nLeftButtonStatus != leftButtonStatus
            bRedrawTabX = self._nTabXButtonStatus != xTabButtonStatus

            if (bRedrawX or bRedrawRight or bRedrawLeft or bRedrawTabX):
            
                dc = wx.ClientDC(self)
                if bRedrawX:
                
                    self.DrawX(dc)
                
                if bRedrawLeft:
                
                    self.DrawLeftArrow(dc)
                
                if bRedrawRight:
                
                    self.DrawRightArrow(dc)
                
                if bRedrawTabX:
                
                    self.DrawTabX(dc, self._pagesInfoVec[tabIdx].GetXRect(), tabIdx)
                
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
        
        dc = wx.ClientDC(self)

        # In case we have error prevent crash
        if self._nFrom < 0:
            return 0

        style = self.GetParent().GetWindowStyleFlag()
        
        for ii in xrange(self._nFrom, -1, -1):

            boldFont = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)        
            boldFont.SetWeight(wx.FONTWEIGHT_BOLD)
            dc.SetFont(boldFont)

            width, height = dc.GetTextExtent("Tp")

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

            tabWidth = self._pParent._nPadding*2 + width
            
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
            
            if posx + tabWidth + self.GetButtonsAreaLength() >= clientWidth:
                break

            numTabs = numTabs + 1
            posx += tabWidth
        
        return numTabs


    def IsDefaultTabs(self):
        """ Returns whether a tab has a default style. """

        style = self.GetParent().GetWindowStyleFlag()
        res = (style & FNB_VC71) or (style & FNB_FANCY_TABS)
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
            self.SetSelection((nSel == nMax and [0] or [nSel + 1])[0])
        else:
            self.SetSelection((nSel == 0 and [nMax] or [nSel - 1])[0])


    def OnMouseLeave(self, event):
        """ Handles the wx.EVT_LEAVE_WINDOW event for PageContainerBase. """

        self._nLeftButtonStatus = FNB_BTN_NONE
        self._nXButtonStatus = FNB_BTN_NONE
        self._nRightButtonStatus = FNB_BTN_NONE
        self._nTabXButtonStatus = FNB_BTN_NONE

        dc = wx.ClientDC(self)
        self.DrawX(dc)
        self.DrawLeftArrow(dc)
        self.DrawRightArrow(dc)

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
                    
        self.DrawTabX(dc, self._pagesInfoVec[selection].GetXRect(), selection)
            
        event.Skip()


    def OnMouseEnterWindow(self, event):
        """ Handles the wx.EVT_ENTER_WINDOW event for PageContainerBase. """

        self._nLeftButtonStatus = FNB_BTN_NONE
        self._nXButtonStatus = FNB_BTN_NONE
        self._nRightButtonStatus = FNB_BTN_NONE
        self._nLeftClickZone = FNB_BTN_NONE

        event.Skip()


    def ShowTabTooltip(self, tabIdx):
        """ Shows a tab tooltip. """

        pWindow = self._pParent.GetPage(tabIdx)
        
        if pWindow:        
            pToolTip = pWindow.GetToolTip()
            if pToolTip and pToolTip.GetWindow() == pWindow:
                self.SetToolTipString(pToolTip.GetTip())
        

    def FillGradientColor(self, dc, rect):
        """ Gradient fill from colour 1 to colour 2 with top to bottom. """

        if rect.height < 1 or rect.width < 1:
            return

        size = rect.height

        # calculate gradient coefficients
        style = self.GetParent().GetWindowStyleFlag()
        col2 = ((style & FNB_BOTTOM) and [self._colorTo] or [self._colorFrom])[0]
        col1 = ((style & FNB_BOTTOM) and [self._colorFrom] or [self._colorTo])[0]

        rf, gf, bf = 0, 0, 0
        rstep = float(col2.Red() - col1.Red())/float(size)
        gstep = float(col2.Green() - col1.Green())/float(size)
        bstep = float(col2.Blue() - col1.Blue())/float(size)

        for y in xrange(rect.y, rect.y + size):
            currCol = wx.Colour(col1.Red() + rf, col1.Green() + gf, col1.Blue() + bf)        
            dc.SetBrush(wx.Brush(currCol))
            dc.SetPen(wx.Pen(currCol))
            dc.DrawLine(rect.x, y, rect.x + rect.width, y)
            rf += rstep
            gf += gstep
            bf += bstep
        

    def SetPageImage(self, page, imgindex):
        """ Sets the image index associated to a page. """

        if page < len(self._pagesInfoVec):
        
            self._pagesInfoVec[page].SetImageIndex(imgindex)
            self.Refresh()


    def GetPageImageIndex(self, page):
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
        
        else:
        
            if wx.Platform in ["__WXMSW__", "__WXGTK__"]:
                if nTabPage >= 0:
                
                    window = oldNotebook.GetPage(nTabPage)

                    if window:
                        where, nIndex = newNotebook._pages.HitTest(wx.Point(x, y))
                        caption = oldContainer.GetPageText(nTabPage)
                        imageindex = oldContainer.GetPageImageIndex(nTabPage)
                        oldNotebook.RemovePage(nTabPage)
                        window.Reparent(newNotebook)

                        newNotebook.InsertPage(nIndex, window, caption, True, imageindex)

        self._isdragging = False
        
        return wx.DragMove


    def MoveTabPage(self, nMove, nMoveTo):
        """ Moves a tab inside the same FlatNotebook. """

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
        self.DoSetSelection(self._iActivePage)
        self.Refresh()
        self._pParent.Thaw()


    def CanFitToScreen(self, page):
        """ Returns whether all the tabs can fit in the available space. """

        # Incase the from is greater than page,
        # we need to reset the self._nFrom, so in order
        # to force the caller to do so, we return False
        if self._nFrom > page:
            return False

        # Calculate the tab width including borders and image if any
        dc = wx.ClientDC(self)

        style = self.GetParent().GetWindowStyleFlag()

        width, height = dc.GetTextExtent("Tp")
        width, pom = dc.GetTextExtent(self.GetPageText(page))

        tabHeight = height + FNB_HEIGHT_SPACER # We use 6 pixels as padding

        if style & FNB_VC71:
            tabHeight = (style & FNB_BOTTOM and [tabHeight - 4] or [tabHeight])[0]
        elif style & FNB_FANCY_TABS:
            tabHeight = (style & FNB_BOTTOM and [tabHeight - 2] or [tabHeight])[0]

        tabWidth = self._pParent._nPadding * 2 + width
        
        if not style & FNB_VC71:
            shapePoints = int(tabHeight*math.tan(float(self._pagesInfoVec[page].GetTabAngle())/180.0*math.pi))
        else:
            shapePoints = 0

        if not style & FNB_VC71:
            # Default style
            tabWidth += 2*shapePoints

        hasImage = self._ImageList != None
        
        if hasImage:
            hasImage &= self._pagesInfoVec[page].GetImageIndex() != -1

        # For VC71 style, we only add the icon size (16 pixels)
        if hasImage and (style & FNB_VC71 or style & FNB_FANCY_TABS):
            tabWidth += 16
        else:
            # Default style
            tabWidth += 16 + shapePoints/2

        # Check if we can draw more
        posx = self._pParent._nPadding

        if self._nFrom >= 0:
            for ii in xrange(self._nFrom, len(self._pagesInfoVec)):
                if self._pagesInfoVec[ii].GetPosition() == wx.Point(-1, -1):
                    break
                posx += self._pagesInfoVec[ii].GetSize().x
            
        rect = self.GetClientRect()
        clientWidth = rect.width

        if posx + tabWidth + self.GetButtonsAreaLength() >= clientWidth:
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
        

    def GetLeftButtonPos(self):
        """ Returns the left button position in the navigation area. """

        style = self.GetParent().GetWindowStyleFlag()
        rect = self.GetClientRect()
        clientWidth = rect.width
        
        if style & FNB_NO_X_BUTTON:
            return clientWidth - 38
        else:
            return clientWidth - 54


    def GetRightButtonPos(self):
        """ Returns the right button position in the navigation area. """

        style = self.GetParent().GetWindowStyleFlag()
        rect = self.GetClientRect()
        clientWidth = rect.width
        
        if style & FNB_NO_X_BUTTON:
            return clientWidth - 22
        else:
            return clientWidth - 38


    def GetXPos(self):
        """ Returns the 'X' button position in the navigation area. """

        style = self.GetParent().GetWindowStyleFlag()
        rect = self.GetClientRect()
        clientWidth = rect.width
        
        if style & FNB_NO_X_BUTTON:
            return clientWidth
        else:
            return clientWidth - 22


    def GetButtonsAreaLength(self):
        """ Returns the navigation area width. """

        style = self.GetParent().GetWindowStyleFlag()
        
        if style & FNB_NO_NAV_BUTTONS and style & FNB_NO_X_BUTTON:
            return 0
        elif style & FNB_NO_NAV_BUTTONS and not style & FNB_NO_X_BUTTON:
            return 53 - 16
        elif not style & FNB_NO_NAV_BUTTONS and style & FNB_NO_X_BUTTON:
            return 53 - 16
        else:
            # All buttons
            return 53


    def GetSingleLineBorderColor(self):

        if self.HasFlag(FNB_FANCY_TABS):
            return self._colorFrom
        
        return wx.WHITE


    def DrawTabsLine(self, dc):
        """ Draws a line over the tabs. """

        clntRect = self.GetClientRect()
        clientRect3 = wx.Rect(0, 0, clntRect.width, clntRect.height)

        if self.HasFlag(FNB_BOTTOM):
        
            clientRect = wx.Rect(0, 2, clntRect.width, clntRect.height - 2)
            clientRect2 = wx.Rect(0, 1, clntRect.width, clntRect.height - 1)
        
        else:
        
            clientRect = wx.Rect(0, 0, clntRect.width, clntRect.height - 2)
            clientRect2 = wx.Rect(0, 0, clntRect.width, clntRect.height - 1)
        
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.SetPen(wx.Pen(self.GetSingleLineBorderColor()))
        dc.DrawRectangleRect(clientRect2)
        dc.DrawRectangleRect(clientRect3)

        dc.SetPen(wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW)))
        dc.DrawRectangleRect(clientRect)

        if not self.HasFlag(FNB_TABS_BORDER_SIMPLE):
        
            dc.SetPen(wx.Pen((self.HasFlag(FNB_VC71) and [wx.Colour(247, 243, 233)] or [self._tabAreaColor])[0]))
            dc.DrawLine(0, 0, 0, clientRect.height+1)
            
            if self.HasFlag(FNB_BOTTOM):
            
                dc.DrawLine(0, clientRect.height+1, clientRect.width, clientRect.height+1)
            
            else:
                dc.DrawLine(0, 0, clientRect.width, 0)
                
            dc.DrawLine(clientRect.width - 1, 0, clientRect.width - 1, clientRect.height+1)
        

    def HasFlag(self, flag):
        """ Returns whether a flag is present in the FlatNotebook style. """

        style = self.GetParent().GetWindowStyleFlag()
        res = (style & flag and [True] or [False])[0]
        return res


    def ClearFlag(self, flag):
        """ Deletes a flag from the FlatNotebook style. """

        style = self.GetParent().GetWindowStyleFlag()
        style &= ~flag 
        self.SetWindowStyleFlag(style)


    def TabHasImage(self, tabIdx):
        """ Returns whether a tab has an associated image index or not. """

        if self._ImageList:
            return self._pagesInfoVec[tabIdx].GetImageIndex() != -1
        
        return False


    def OnLeftDClick(self, event):
        """ Handles the wx.EVT_LEFT_DCLICK event for PageContainerBase. """

        if self.HasFlag(FNB_DCLICK_CLOSES_TABS):
        
            where, tabIdx = self.HitTest(event.GetPosition())
            
            if where == FNB_TAB:
                self.DeletePage(tabIdx)
        
        else:
        
            event.Skip()
        

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
        """ Returns the number of tabs in the FlatNotebook control. """

        return len(self._pagesInfoVec)


    def GetPageText(self, page):
        """ Returns the tab caption of the page. """

        return self._pagesInfoVec[page].GetCaption() 


    def SetPageText(self, page, text):
        """ Sets the tab caption of the page. """

        self._pagesInfoVec[page].SetCaption(text)
        return True 


    def CanDrawXOnTab(self):
        """ Returns whether an 'X' can be drawn on a tab (all styles except VC8. """
        
        return True


# ---------------------------------------------------------------------------- #
# Class FlatNotebook
# Simple super class based on PageContainerBase
# ---------------------------------------------------------------------------- #

class FlatNotebook(FlatNotebookBase):

    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=0, name="FlatNotebook"):
        """
        Default class constructor.

        It is better to use directly the StyledNotebook class (see below) and then
        assigning the style you wish instead of calling FlatNotebook.
        """

        style |= wx.TAB_TRAVERSAL
        
        FlatNotebookBase.__init__(self, parent, id, pos, size, style, name)
        self._pages = self.CreatePageContainer()


    def CreatePageContainer(self):
        """ Creates the page container. """

        return FlatNotebookBase.CreatePageContainer(self)


#--------------------------------------------------------------------
# StyledNotebook - a notebook with look n feel of Visual Studio 2005
#--------------------------------------------------------------------

class StyledNotebook(FlatNotebookBase):

    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=0, name="StyledNotebook"):
        """ Default class constructor.
        
        It is better to use directly the StyledNotebook class and then
        assigning the style you wish instead of calling FlatNotebook.
        """

        style |= wx.TAB_TRAVERSAL
        
        FlatNotebookBase.__init__(self, parent, id, pos, size, style, name)
        
        # Custom initialization of the tab area
        if style & FNB_VC8:
            # Initialise the default style colors
            self.SetNonActiveTabTextColour(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNTEXT))


    def CreatePageContainer(self):
        """ Creates the page container. """

        return StyledTabsContainer(self, wx.ID_ANY)


# ---------------------------------------------------------------------------- #
# Class StyledTabsContainer
# Acts as a container for the pages you add to FlatNotebook
# A more generic and more powerful implementation of PageContainerBase, can
# handle also VC8 tabs style
# ---------------------------------------------------------------------------- #

class StyledTabsContainer(PageContainerBase):

    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=0):
        """ Default class constructor. """        

        self._factor = 1

        self._colorTo = LightColour(wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DFACE), 0) 
        self._colorFrom = LightColour(wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DFACE), 60)
        
        PageContainerBase.__init__(self, parent, id, pos, size, style) 

        self.Bind(wx.EVT_PAINT, self.OnPaint)

        
    def NumberTabsCanFit(self, dc):
        """ Returns the number of tabs that can fit inside the available space. """

        rect = self.GetClientRect()
        clientWidth = rect.width

        # Empty results
        vTabInfo = []

        # We take the maxmimum font size, this is 
        # achieved by setting the font to be bold
        font = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        dc.SetFont(font)

        width, height = dc.GetTextExtent("Tp")

        tabHeight = height + FNB_HEIGHT_SPACER # We use 8 pixels
        # The drawing starts from posx
        posx = self._pParent.GetPadding()
        
        for i in xrange(self._nFrom, len(self._pagesInfoVec)):
        
            width, pom = dc.GetTextExtent(self.GetPageText(i))
            
            # Set a minimum size to a tab
            if width < 20:
                width = 20

            tabWidth = self._pParent.GetPadding() * 2 + width
            
            # Add the image width if it exist
            if self.TabHasImage(i):
                tabWidth += 16 + self._pParent.GetPadding()

            vc8glitch = tabHeight + FNB_HEIGHT_SPACER
            if posx + tabWidth + vc8glitch + self.GetButtonsAreaLength() >= clientWidth:
                break

            # Add a result to the returned vector
            tabRect = wx.Rect(posx, VERTICAL_BORDER_PADDING, tabWidth , tabHeight)
            vTabInfo.append(tabRect)

            # Advance posx
            posx += tabWidth + FNB_HEIGHT_SPACER
        
        return vTabInfo
    

    def GetNumTabsCanScrollLeft(self):
        """ Returns the number of tabs than can be scrolled left. """

        # Reserved area for the buttons (<>x)
        rect = self.GetClientRect()
        clientWidth = rect.width
        posx = self._pParent.GetPadding()
        numTabs = 0
        pom = 0

        dc = wx.ClientDC(self)

        # Incase we have error prevent crash
        if self._nFrom < 0:
            return 0

        style = self.GetParent().GetWindowStyleFlag()
        
        for i in xrange(self._nFrom, -1, -1):
        
            boldFont = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
            boldFont.SetWeight(wx.FONTWEIGHT_BOLD)
            dc.SetFont(boldFont)

            width, height = dc.GetTextExtent("Tp")

            tabHeight = height + FNB_HEIGHT_SPACER # We use 6 pixels as padding
            
            if style & FNB_VC71:
                tabHeight = (self.HasFlag(FNB_BOTTOM) and [tabHeight - 4] or [tabHeight])[0]
            elif style & FNB_FANCY_TABS:
                tabHeight = (self.HasFlag(FNB_BOTTOM) and [tabHeight - 3] or [tabHeight])[0]

            width, pom = dc.GetTextExtent(self.GetPageText(i))
            
            if style != FNB_VC71:
                shapePoints = int(tabHeight*math.tan(float(self._pagesInfoVec[i].GetTabAngle())/180.0*math.pi))
            else:
                shapePoints = 0

            tabWidth = self._pParent.GetPadding() * 2 + width
            if not style & FNB_VC71:
                # Default style
                tabWidth += 2*shapePoints

            # For VC71 style, we only add the icon size (16 pixels)
            if self.TabHasImage(i):
            
                if not self.IsDefaultTabs():
                    tabWidth += 16 + self._pParent.GetPadding()
                else:
                    # Default style
                    tabWidth += 16 + self._pParent.GetPadding() + shapePoints/2
            
            vc8glitch = (style & FNB_VC8 and [tabHeight + FNB_HEIGHT_SPACER] or [0])[0]
            
            if posx + tabWidth + vc8glitch + self.GetButtonsAreaLength() >= clientWidth:
                break

            numTabs = numTabs + 1
            posx += tabWidth

        return numTabs


    def CanDrawXOnTab(self):
        """ Returns whether an 'X' button can be drawn on a tab (not VC8 style). """

        style = self.GetParent().GetWindowStyleFlag()
        isVC8 = (style & FNB_VC8 and [True] or [False])[0]
        return not isVC8


    def IsDefaultTabs(self):
        """ Returns whether a tab has a default style or not. """

        style = self.GetParent().GetWindowStyleFlag()
        res = (style & FNB_VC71) or (style & FNB_FANCY_TABS) or (style & FNB_VC8)
        return not res


    def HitTest(self, pt):
        """ HitTest specific method for VC8 style. """

        fullrect = self.GetClientRect()
        btnLeftPos = self.GetLeftButtonPos()
        btnRightPos = self.GetRightButtonPos()
        btnXPos = self.GetXPos()
        style = self.GetParent().GetWindowStyleFlag()
        tabIdx = -1

        if not self._pagesInfoVec:        
            return FNB_NOWHERE, -1
        
        rect = wx.Rect(btnXPos, 8, 16, 16)
        
        if rect.Contains(pt):
            return (style & FNB_NO_X_BUTTON and [FNB_NOWHERE] or [FNB_X])[0], -1

        rect = wx.Rect(btnRightPos, 8, 16, 16)
        
        if rect.Contains(pt):        
            return (style & FNB_NO_NAV_BUTTONS and [FNB_NOWHERE] or [FNB_RIGHT_ARROW])[0], -1
        
        rect = wx.Rect(btnLeftPos, 8, 16, 16)
        
        if rect.Contains(pt):
            return (style & FNB_NO_NAV_BUTTONS and [FNB_NOWHERE] or [FNB_LEFT_ARROW])[0], -1
        
        # Test whether a left click was made on a tab
        bFoundMatch = False
        
        for cur in xrange(self._nFrom, len(self._pagesInfoVec)):
        
            pgInfo = self._pagesInfoVec[cur]

            if pgInfo.GetPosition() == wx.Point(-1, -1):
                continue
            
            if style & FNB_VC8:
            
                if self._pagesInfoVec[cur].GetRegion().Contains(pt.x, pt.y):
                
                    if bFoundMatch or cur == self.GetSelection():

                        return FNB_TAB, cur
                    
                    tabIdx = cur
                    bFoundMatch = True
                
            else:
            
                if style & FNB_X_ON_TAB and cur == self.GetSelection():
                
                    # 'x' button exists on a tab
                    if self._pagesInfoVec[cur].GetXRect().Contains(pt):                    
                        return FNB_TAB_X, cur
                    
                tabRect = wx.Rect(pgInfo.GetPosition().x, pgInfo.GetPosition().y, pgInfo.GetSize().x, pgInfo.GetSize().y)
                
                if tabRect.Contains(pt):                
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


    def OnPaint(self, event):
        """
        Handles the wx.EVT_PAINT event for StyledTabsContainer.
        Switches to PageContainerBase.OnPaint() method if the style is not VC8. 
        """

        if not self.HasFlag(FNB_VC8):
        
            PageContainerBase.OnPaint(self, event)
            return

        # Visual studio 8 style
        dc = wx.BufferedPaintDC(self)

        if "__WXMAC__" in wx.PlatformInfo:
            # Works well on MSW & GTK, however this lines should be skipped on MAC
            if not self._pagesInfoVec or self._nFrom >= len(self._pagesInfoVec):
                self.Hide()
                event.Skip()
                return
        
        # Set the font for measuring the tab height
        normalFont = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        boldFont = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        boldFont.SetWeight(wx.FONTWEIGHT_BOLD)

        if "__WXGTK__" in wx.PlatformInfo:
            dc.SetFont(boldFont)

        width, height = dc.GetTextExtent("Tp")

        tabHeight = height + FNB_HEIGHT_SPACER # We use 8 pixels as padding

        # Calculate the number of rows required for drawing the tabs
        rect = self.GetClientRect()

        # Set the maximum client size
        self.SetSizeHints(self.GetButtonsAreaLength(), tabHeight)
        borderPen = wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW))

        # Create brushes
        backBrush = wx.Brush(self._tabAreaColor)
        noselBrush = wx.Brush(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE))
        selBrush = wx.Brush(self._activeTabColor)
        size = self.GetSize()

        # Background
        dc.SetTextBackground(self.GetBackgroundColour())
        dc.SetTextForeground(self._activeTextColor)
        
        # If border style is set, set the pen to be border pen
        if self.HasFlag(FNB_TABS_BORDER_SIMPLE):
            dc.SetPen(borderPen)
        else:
            dc.SetPen(wx.TRANSPARENT_PEN)

        lightFactor = (self.HasFlag(FNB_BACKGROUND_GRADIENT) and [70] or [0])[0]
        # For VC8 style, we color the tab area in gradient coloring
        PaintStraightGradientBox(dc, self.GetClientRect(), self._tabAreaColor, LightColour(self._tabAreaColor, lightFactor))

        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.DrawRectangle(0, 0, size.x, size.y)

        # Take 3 bitmaps for the background for the buttons
        
        mem_dc = wx.MemoryDC()

        #---------------------------------------
        # X button
        #---------------------------------------
        rect = wx.Rect(self.GetXPos(), 6, 16, 14)
        mem_dc.SelectObject(self._xBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)

        #---------------------------------------
        # Right button
        #---------------------------------------
        rect = wx.Rect(self.GetRightButtonPos(), 6, 16, 14)
        mem_dc.SelectObject(self._rightBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)

        #---------------------------------------
        # Left button
        #---------------------------------------
        rect = wx.Rect(self.GetLeftButtonPos(), 6, 16, 14)
        mem_dc.SelectObject(self._leftBgBmp)
        mem_dc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        mem_dc.SelectObject(wx.NullBitmap)
        
        # We always draw the bottom/upper line of the tabs
        # regradless the style
        dc.SetPen(borderPen)
        self.DrawTabsLine(dc)

        # Restore the pen
        dc.SetPen(borderPen)

        # Draw labels
        dc.SetFont(boldFont)
        activeTabPosx = 0

        # Update all the tabs from 0 to 'self._nFrom' to be non visible
        for i in xrange(self._nFrom):
        
            self._pagesInfoVec[i].SetPosition(wx.Point(-1, -1))
            self._pagesInfoVec[i].GetRegion().Clear()

        # Draw the visible tabs, in VC8 style, we draw them from right to left
        vTabsInfo = self.NumberTabsCanFit(dc)
        
        for cur in xrange(len(vTabsInfo) - 1, -1, -1):
        
            # 'i' points to the index of the currently drawn tab
            # in self._pagesInfoVec vector
            i = self._nFrom + cur
            dc.SetPen(borderPen)
            dc.SetBrush((i==self.GetSelection() and [selBrush] or [noselBrush])[0])

            # Calculate the text length using the bold font, so when selecting a tab
            # its width will not change
            dc.SetFont(boldFont)
            width, pom = dc.GetTextExtent(self.GetPageText(i))

            # Now set the font to the correct font
            dc.SetFont((i==self.GetSelection() and [boldFont] or [normalFont])[0])

            # Set a minimum size to a tab
            if width < 20:
                width = 20

            # Add the padding to the tab width
            # Tab width:
            # +-----------------------------------------------------------+
            # | PADDING | IMG | IMG_PADDING | TEXT | PADDING | x |PADDING |
            # +-----------------------------------------------------------+

            tabWidth = self._pParent.GetPadding() * 2 + width
            imageYCoord = (self.HasFlag(FNB_BOTTOM) and [6] or [8])[0]

            if self.TabHasImage(i):
                tabWidth += 16 + self._pParent.GetPadding()

            posx = vTabsInfo[cur].x

            # By default we clean the tab region
            # incase we use the VC8 style which requires
            # the region, it will be filled by the function
            # drawVc8Tab
            self._pagesInfoVec[i].GetRegion().Clear()
            
            # Clean the 'x' buttn on the tab 
            # 'Clean' rectanlge is a rectangle with width or height
            # with values lower than or equal to 0
            self._pagesInfoVec[i].GetXRect().SetSize(wx.Size(-1, -1))

            # Draw the tab
            # Incase we are drawing the active tab
            # we need to redraw so it will appear on top
            # of all other tabs
            if i == self.GetSelection():
            
                activeTabPosx = posx
            
            else:
            
                self.DrawVC8Tab(dc, posx, i, tabWidth, tabHeight)

            # Text drawing offset from the left border of the 
            # rectangle
            # The width of the images are 16 pixels
            vc8ShapeLen = tabHeight
            
            if self.TabHasImage(i):
                textOffset = self._pParent.GetPadding() * 2 + 16 + vc8ShapeLen 
            else:
                textOffset = self._pParent.GetPadding() + vc8ShapeLen

            # Set the non-active text color
            if i != self.GetSelection():
                dc.SetTextForeground(self._nonActiveTextColor)

            if self.TabHasImage(i):
                imageXOffset = textOffset - 16 - self._pParent.GetPadding()
                self._ImageList.Draw(self._pagesInfoVec[i].GetImageIndex(), dc,
                                     posx + imageXOffset, imageYCoord,
                                     wx.IMAGELIST_DRAW_TRANSPARENT, True)

            dc.DrawText(self.GetPageText(i), posx + textOffset, imageYCoord)
            
            textWidth, textHeight = dc.GetTextExtent(self.GetPageText(i))

            # Restore the text forground
            dc.SetTextForeground(self._activeTextColor)

            # Update the tab position & size
            self._pagesInfoVec[i].SetPosition(wx.Point(posx, VERTICAL_BORDER_PADDING))
            self._pagesInfoVec[i].SetSize(wx.Size(tabWidth, tabHeight))
        
        # Incase we are in VC8 style, redraw the active tab (incase it is visible)
        if self.GetSelection() >= self._nFrom and self.GetSelection() < self._nFrom + len(vTabsInfo):
        
            hasImage = self.TabHasImage(self.GetSelection())

            dc.SetFont(boldFont)
            width, pom = dc.GetTextExtent(self.GetPageText(self.GetSelection()))

            tabWidth = self._pParent.GetPadding() * 2 + width

            if hasImage:
                tabWidth += 16 + self._pParent.GetPadding()

            # Set the active tab font, pen brush and font-color
            dc.SetPen(borderPen)
            dc.SetBrush(selBrush)
            dc.SetFont(boldFont)
            dc.SetTextForeground(self._activeTextColor)
            self.DrawVC8Tab(dc, activeTabPosx, self.GetSelection(), tabWidth, tabHeight)

            # Text drawing offset from the left border of the 
            # rectangle
            # The width of the images are 16 pixels
            vc8ShapeLen = tabHeight - VERTICAL_BORDER_PADDING - 2
            if hasImage:
                textOffset = self._pParent.GetPadding() * 2 + 16 + vc8ShapeLen 
            else:
                textOffset = self._pParent.GetPadding() + vc8ShapeLen

            # Draw the image for the tab if any
            imageYCoord = (self.HasFlag(FNB_BOTTOM) and [6] or [8])[0]

            if hasImage:
                imageXOffset = textOffset - 16 - self._pParent.GetPadding()
                self._ImageList.Draw(self._pagesInfoVec[self.GetSelection()].GetImageIndex(), dc,
                                     activeTabPosx + imageXOffset, imageYCoord,
                                     wx.IMAGELIST_DRAW_TRANSPARENT, True)            

            dc.DrawText(self.GetPageText(self.GetSelection()), activeTabPosx + textOffset, imageYCoord)

        # Update all tabs that can not fit into the screen as non-visible
        for xx in xrange(self._nFrom + len(vTabsInfo), len(self._pagesInfoVec)):
        
            self._pagesInfoVec[xx].SetPosition(wx.Point(-1, -1))
            self._pagesInfoVec[xx].GetRegion().Clear()
        
        # Draw the left/right/close buttons 
        # Left arrow
        self.DrawLeftArrow(dc)
        self.DrawRightArrow(dc)
        self.DrawX(dc)


    def DrawVC8Tab(self, dc, posx, tabIdx, tabWidth, tabHeight):
        """ Draws the VC8 style tabs. """

        borderPen = wx.Pen(self._colorBorder)
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

        tabPoints[0].x = (self.HasFlag(FNB_BOTTOM) and [posx] or [posx+self._factor])[0]
        tabPoints[0].y = (self.HasFlag(FNB_BOTTOM) and [2] or [tabHeight - 3])[0]

        tabPoints[1].x = tabPoints[0].x + tabHeight - VERTICAL_BORDER_PADDING - 3 - self._factor
        tabPoints[1].y = (self.HasFlag(FNB_BOTTOM) and [tabHeight - (VERTICAL_BORDER_PADDING+2)] or [(VERTICAL_BORDER_PADDING+2)])[0]

        tabPoints[2].x = tabPoints[1].x + 4
        tabPoints[2].y = (self.HasFlag(FNB_BOTTOM) and [tabHeight - VERTICAL_BORDER_PADDING] or [VERTICAL_BORDER_PADDING])[0]

        tabPoints[3].x = tabPoints[2].x + tabWidth - 2
        tabPoints[3].y = (self.HasFlag(FNB_BOTTOM) and [tabHeight - VERTICAL_BORDER_PADDING] or [VERTICAL_BORDER_PADDING])[0]

        tabPoints[4].x = tabPoints[3].x + 1
        tabPoints[4].y = (self.HasFlag(FNB_BOTTOM) and [tabPoints[3].y - 1] or [tabPoints[3].y + 1])[0]

        tabPoints[5].x = tabPoints[4].x + 1
        tabPoints[5].y = (self.HasFlag(FNB_BOTTOM) and [(tabPoints[4].y - 1)] or [tabPoints[4].y + 1])[0]

        tabPoints[6].x = tabPoints[2].x + tabWidth
        tabPoints[6].y = tabPoints[0].y

        tabPoints[7].x = tabPoints[0].x
        tabPoints[7].y = tabPoints[0].y

        self._pagesInfoVec[tabIdx].SetRegion(tabPoints)

        # Draw the polygon
        br = dc.GetBrush()
        dc.SetBrush(wx.Brush((tabIdx == self.GetSelection() and [self._activeTabColor] or [self._colorTo])[0]))
        dc.SetPen(wx.Pen((tabIdx == self.GetSelection() and [wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW)] or [self._colorBorder])[0]))
        dc.DrawPolygon(tabPoints)

        # Restore the brush
        dc.SetBrush(br)

        rect = self.GetClientRect()

        if tabIdx != self.GetSelection() and not self.HasFlag(FNB_BOTTOM):
        
            # Top default tabs
            dc.SetPen(wx.Pen(self._colorBorder))
            lineY = rect.height
            curPen = dc.GetPen()
            curPen.SetWidth(1)
            dc.SetPen(curPen)
            dc.DrawLine(posx, lineY, posx+rect.width, lineY)

        # In case we are drawing the selected tab, we draw the border of it as well
        # but without the bottom (upper line incase of wxBOTTOM)
        if tabIdx == self.GetSelection():
        
            borderPen = wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW))
            brush = wx.TRANSPARENT_BRUSH
            dc.SetPen(borderPen)
            dc.SetBrush(brush)
            dc.DrawPolygon(tabPoints)

            # Delete the bottom line (or the upper one, incase we use wxBOTTOM) 
            dc.SetPen(wx.WHITE_PEN)
            dc.DrawLine(tabPoints[0].x, tabPoints[0].y, tabPoints[6].x, tabPoints[6].y)
        
        self.FillVC8GradientColor(dc, tabPoints, tabIdx == self.GetSelection(), tabIdx)

        # Draw a thin line to the right of the non-selected tab
        if tabIdx != self.GetSelection():
        
            dc.SetPen(wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DFACE)))
            dc.DrawLine(tabPoints[4].x-1, tabPoints[4].y, tabPoints[5].x-1, tabPoints[5].y)
            dc.DrawLine(tabPoints[5].x-1, tabPoints[5].y, tabPoints[6].x-1, tabPoints[6].y)
        

    def FillVC8GradientColor(self, dc, tabPoints, bSelectedTab, tabIdx):
        """ Fills a tab with a gradient colour. """

        # calculate gradient coefficients
        col2 = (self.HasFlag(FNB_BOTTOM) and [self._colorTo] or [self._colorFrom])[0]
        col1 = (self.HasFlag(FNB_BOTTOM) and [self._colorFrom] or [self._colorTo])[0]

        # If colorful tabs style is set, override the tab color
        if self.HasFlag(FNB_COLORFUL_TABS):
        
            if not self._pagesInfoVec[tabIdx].GetColor():
            
                # First time, generate color, and keep it in the vector
                tabColor = self.GenTabColour()
                self._pagesInfoVec[tabIdx].SetColor(tabColor)
            
            if self.HasFlag(FNB_BOTTOM):
            
                col2 = LightColour(self._pagesInfoVec[tabIdx].GetColor(), 50 )
                col1 = LightColour(self._pagesInfoVec[tabIdx].GetColor(), 80 )
            
            else:
            
                col1 = LightColour(self._pagesInfoVec[tabIdx].GetColor(), 50 )
                col2 = LightColour(self._pagesInfoVec[tabIdx].GetColor(), 80 )
            
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
            self.DrawTabsLine(dc)

        while 1:
        
            if self.HasFlag(FNB_BOTTOM):
            
                if y > tabPoints[0].y + size:
                    break
            
            else:
            
                if y < tabPoints[0].y - size:
                    break
            
            currCol = wx.Colour(col1.Red() + rf, col1.Green() + gf, col1.Blue() + bf)

            dc.SetPen((bSelectedTab and [wx.Pen(self._activeTabColor)] or [wx.Pen(currCol)])[0])
            startX = self.GetStartX(tabPoints, y) 
            endX = self.GetEndX(tabPoints, y)
            dc.DrawLine(startX, y, endX, y)

            # Draw the border using the 'edge' point
            dc.SetPen(wx.Pen((bSelectedTab and [wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW)] or [self._colorBorder])[0]))
            
            dc.DrawPoint(startX, y)
            dc.DrawPoint(endX, y)
            
            # Progress the color 
            rf += rstep
            gf += gstep
            bf += bstep

            if self.HasFlag(FNB_BOTTOM):
                y = y + 1
            else:
                y = y - 1


    def GetStartX(self, tabPoints, y):
        """ Returns the x start position of a tab. """

        x1, x2, y1, y2 = 0.0, 0.0, 0.0, 0.0

        # We check the 3 points to the left
        style = self.GetParent().GetWindowStyleFlag()
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


    def GetEndX(self, tabPoints, y):
        """ Returns the x end position of a tab. """

        x1, x2, y1, y2 = 0.0, 0.0, 0.0, 0.0

        # We check the 3 points to the left
        style = self.GetParent().GetWindowStyleFlag()
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
        b = y1 - ((y2 - y1)/(x2 - x1)) * x1

        if a == 0:
            return int(x1)

        x = (y - b)/a

        return int(x)


    def GenTabColour(self):
        """ Generates a random soft pleasant colour for a tab. """

        return RandomColor()


    def GetSingleLineBorderColor(self):

        if self.HasFlag(FNB_VC8):
            return self._activeTabColor
        else:
            return PageContainerBase.GetSingleLineBorderColor(self)


    def SetFactor(self, factor):
        """ Sets the brighten colour factor. """
        
        self._factor = factor
        self.Refresh()


    def GetFactor(self):
        """ Returns the brighten colour factor. """

        return self._factor 



