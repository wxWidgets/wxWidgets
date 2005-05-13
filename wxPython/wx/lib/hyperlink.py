# --------------------------------------------------------------------------- #
# HYPERLINKSCTRL wxPython IMPLEMENTATION
# Ported From Angelo Mandato C++ Code By:
#
# Andrea Gavana, @ 27 Mar 2005
# Latest Revision: 27 Apr 2005, 22.30 CET
#
#
# Original Web Site (For The C++ Code):
#
# http://www.spaceblue.com/codedetail.php?CodeID=7
#
#
# For all kind of problems, requests of enhancements and bug reports, please
# write to me at:
#
# andrea.gavana@agip.it
# andrea_gavan@tin.it
#
# Or, obviously, to the wxPython mailing list!!!
#
#
# End Of Comments
# --------------------------------------------------------------------------- #

"""
`HyperLinkCtrl` is a control for wxPython that acts like a hyper link
in a typical browser. Latest features include the ability to capture
your own Left, Middle, and Right click events to perform your own
custom event handling and ability to open link in a new or current
browser window.

Special thanks to Robin Dunn for the event binder for the 3 mouse buttons.


Latest Revision: Andrea Gavana @ 11 May 2005, 21.00 CET

"""

import wx
from wx.lib.stattext import GenStaticText as StaticText

# Import the useful webbrowser module for platform-independent results
import webbrowser

# Set no delay time to open the web page
webbrowser.PROCESS_CREATION_DELAY = 0

# To show a popup that copies the hyperlinks on the clipboard
wxHYPERLINKS_POPUP_COPY = 1000


#-----------------------------------#
#        HyperLinksEvents
#-----------------------------------#

# wxEVT_HYPERLINK_LEFT: Respond To A Left Mouse Button Event
# wxEVT_HYPERLINK_MIDDLE: Respond To A Middle Mouse Button Event
# wxEVT_HYPERLINK_RIGHT: Respond To A Right Mouse Button Event

wxEVT_HYPERLINK_LEFT = wx.NewEventType()
wxEVT_HYPERLINK_MIDDLE = wx.NewEventType()
wxEVT_HYPERLINK_RIGHT = wx.NewEventType()

EVT_HYPERLINK_LEFT = wx.PyEventBinder(wxEVT_HYPERLINK_LEFT, 1)
EVT_HYPERLINK_MIDDLE = wx.PyEventBinder(wxEVT_HYPERLINK_MIDDLE, 1)
EVT_HYPERLINK_RIGHT = wx.PyEventBinder(wxEVT_HYPERLINK_RIGHT, 1)


# ------------------------------------------------------------
# This class implements the event listener for the hyperlinks
# ------------------------------------------------------------

class HyperLinkEvent(wx.PyCommandEvent):
    """
    Event object sent in response to clicking on a `HyperLinkCtrl`.
    """

    def __init__(self, eventType, id):
        """ Default Class Constructor. """        
        wx.PyCommandEvent.__init__(self, eventType, id)
        self._eventType = eventType


    def SetPosition(self, pos):
        """ Sets Event Position """        
        self._pos = pos


    def GetPosition(self):
        """ Returns Event Position """        
        return self._pos


# -------------------------------------------------
# This is the main HyperLinkCtrl implementation
# it user the StatiText from wx.lib.stattext
# because of its "quasi-dynamic" behavior
# -------------------------------------------------

class HyperLinkCtrl(StaticText):
    """
    `HyperLinkCtrl` is a control for wxPython that acts like a hyper
    link in a typical browser. Latest features include the ability to
    capture your own Left, Middle, and Right click events to perform
    your own custom event handling and ability to open link in a new
    or current browser window.

    Events
    ------
        ====================  =======================================
        EVT_HYPERLINK_LEFT    Sent when the left mouse button is
                              clicked, but only if `AutoBrowse` is set
                              to ``False``.
        EVT_HYPERLINK_MIDDLE  Sent when the middle mouse button is
                              clicked.
        EVT_HYPERLINK_RIGHT   Sent when the right mouse button is
                              clicked, but only if `DoPopup` is set
                              to ``False``.
        ====================  =======================================
    """
    
    def __init__(self, parent, id=-1, label="", pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0, name="staticText", URL=""):
        """
        Default class constructor.
                       
        Pass URL == "" to use the label as the url link to navigate to
        """
        
        StaticText.__init__(self, parent, id, label, pos, size,
                            style, name)

        if URL.strip() == "":
            self._URL = label
        else:
            self._URL = URL

        # Set Tooltip
        self.SetToolTip(wx.ToolTip(self._URL))

        # Set default properties
        # default: True
        self.ReportErrors()

        # default: True, True, True
        self.SetUnderlines()

        # default: blue, violet, blue        
        self.SetColours()

        # default: False
        self.SetVisited()

        # default: False 
        self.EnableRollover()

        # default: False        
        self.SetBold()

        # default: wx.CURSOR_HAND        
        self.SetLinkCursor() 

        # default True
        self.AutoBrowse()

        # default True        
        self.DoPopup() 

        # default False
        self.OpenInSameWindow()

        # Set control properties and refresh
        self.UpdateLink(True)

        self.Bind(wx.EVT_MOUSE_EVENTS, self.OnMouseEvent)
        self.Bind(wx.EVT_MOTION, self.OnMouseEvent)        
    
    
    def GotoURL(self, URL, ReportErrors=True, NotSameWinIfPossible=False):
        """
        Goto The Specified URL.

        :param ReportErrors: Use True to display error dialog if an
            error occurrs navigating to the URL.

        :param NotSameWinIfPossible: Use True to attempt to open the
            URL in new browser window.

        """
    
        logOff = wx.LogNull()

        try:            
            webbrowser.open(URL, new=NotSameWinIfPossible)
            self.SetVisited(True)
            self.UpdateLink(True)

            return True
        
        except:            
            self.DisplayError("Unable To Launch Browser.", ReportErrors)
            return False


    def OnMouseEvent(self, event):
        """ Captures mouse events for cursor, link colors and underlines. """

        if event.Moving():
            # Mouse Is Moving On The StaticText
            # Set The Hand Cursor On The Link
            self.SetCursor(self._CursorHand)

            if self._EnableRollover:
                self.SetForegroundColour(self._LinkRolloverColor)
                fontTemp = self.GetFont()
                fontTemp.SetUnderlined(self._RolloverUnderline)
                if self._Bold:
                    fontTemp.SetWeight(wx.BOLD)

                self.SetFont(fontTemp)
                self.Refresh()

        else:            
            # Restore The Original Cursor            
            self.SetCursor(wx.NullCursor)
            if self._EnableRollover:
                self.UpdateLink(True)
            
            if event.LeftUp():
                # Left Button Was Pressed
                if self._AutoBrowse:                    
                    self.GotoURL(self._URL, self._ReportErrors,
                                 self._NotSameWinIfPossible)

                else:
                    eventOut = HyperLinkEvent(wxEVT_HYPERLINK_LEFT, self.GetId())
                    eventOut.SetEventObject(self)
                    eventOut.SetPosition(event.GetPosition())
                    self.GetEventHandler().ProcessEvent(eventOut)

                self.SetVisited(True)

            elif event.RightUp():
                # Right Button Was Pressed
                if self._DoPopup:
                    # Popups A Menu With The "Copy HyperLynks" Feature
                    menuPopUp = wx.Menu("", wx.MENU_TEAROFF)
                    menuPopUp.Append(wxHYPERLINKS_POPUP_COPY, "Copy HyperLink")
                    self.Bind(wx.EVT_MENU, self.OnPopUpCopy, id=wxHYPERLINKS_POPUP_COPY)
                    self.PopupMenu(menuPopUp, wx.Point(event.m_x, event.m_y))
                    menuPopUp.Destroy()
                    self.Unbind(wx.EVT_MENU, id=wxHYPERLINKS_POPUP_COPY)
                    
                else:                    
                    eventOut = HyperLinkEvent(wxEVT_HYPERLINK_RIGHT, self.GetId())
                    eventOut.SetEventObject(self)
                    eventOut.SetPosition(event.GetPosition())
                    self.GetEventHandler().ProcessEvent(eventOut)

            elif event.MiddleUp():
                # Middle Button Was Pressed
                eventOut = HyperLinkEvent(wxEVT_HYPERLINK_MIDDLE, self.GetId())
                eventOut.SetEventObject(self)
                eventOut.SetPosition(event.GetPosition())
                self.GetEventHandler().ProcessEvent(eventOut)

        event.Skip()


    def OnPopUpCopy(self, event):
        """ Copy data from the HyperLink to the clipboard. """

        wx.TheClipboard.UsePrimarySelection(False)
        if not wx.TheClipboard.Open():
            return
        data = wx.TextDataObject(self._URL)
        wx.TheClipboard.SetData(data)
        wx.TheClipboard.Close()


    def UpdateLink(self, OnRefresh=True):
        """
        Updates the link.

        Changing text properties if:
            - User Specific Setting
            - Link Visited
            - New Link
        
        """

        fontTemp = self.GetFont()

        if self._Visited:
            self.SetForegroundColour(self._VisitedColour)
            fontTemp.SetUnderlined(self._VisitedUnderline)
        
        else:

            self.SetForegroundColour(self._LinkColour)
            fontTemp.SetUnderlined(self._LinkUnderline)


        if self._Bold:
            fontTemp.SetWeight(wx.BOLD)

        self.SetFont(fontTemp)
        self.Refresh(OnRefresh)


    def DisplayError(self, ErrorMessage, ReportErrors=True):
        """
        Displays an error message (according to ReportErrors variable)
        in a MessageBox.
        """
        if ReportErrors:
            wx.MessageBox(ErrorMessage, "HyperLinks Error", wx.OK | wx.CENTRE | wx.ICON_ERROR)


    def SetColours(self,
                   link=wx.Colour(0, 0, 255),
                   visited=wx.Colour(79, 47, 79),
                   rollover=wx.Colour(0, 0, 255)):
        """ Sets the colours for the link, the visited link and the mouse rollover.

        Defaults Are:
            - New Link: RED
            - Visited Link: VIOLET
            - Rollover: BLUE

        """        
        self._LinkColour = link
        self._VisitedColour = visited
        self._LinkRolloverColor = rollover

    
    def GetColours(self):
        """
        Gets the colours for the link, the visited link and the mouse
        rollover.
        """        
        return self._LinkColour, self._VisitedColour, self._LinkRolloverColor

    
    def SetUnderlines(self, link=True, visited=True, rollover=True):
        """ Underlines Properties. """        
        self._LinkUnderline = link
        self._RolloverUnderline = rollover
        self._VisitedUnderline = visited


    def GetUnderlines(self):
        """
        Returns if link is underlined, if the mouse rollover is
        underlined and if the visited link is underlined.
        """
        return self._LinkUnderline, self._RolloverUnderline, self._VisitedUnderline
    

    def SetLinkCursor(self, cur=wx.CURSOR_HAND):
        """ Sets link cursor properties. """        
        self._CursorHand = wx.StockCursor(cur)


    def GetLinkCursor(self):
        """ Gets the link cursor. """        
        return self._CursorHand


    def SetVisited(self, Visited=False):
        """ Sets a link as visited. """
        
        self._Visited = Visited

        
    def GetVisited(self):
        """ Returns whether a link has been visited or not. """        
        return self._Visited


    def SetBold(self, Bold=False):
        """ Sets the HyperLink in bold text. """        
        self._Bold = Bold

        
    def GetBold(self):
        """ Returns whether the HyperLink has text in bold or not. """
        return self._Bold


    def SetURL(self, URL):
        """ Sets the HyperLink text to the specified URL. """        
        self._URL = URL

        
    def GetURL(self):
        """ Retrieve the URL associated to the HyperLink. """        
        return self._URL


    def OpenInSameWindow(self, NotSameWinIfPossible=False):
        """ Open multiple URL in the same window (if possible). """        
        self._NotSameWinIfPossible = NotSameWinIfPossible


    def EnableRollover(self, EnableRollover=False):
        """ Enable/disable rollover. """        
        self._EnableRollover = EnableRollover

    
    def ReportErrors(self, ReportErrors=True):
        """ Set whether to report browser errors or not. """        
        self._ReportErrors = ReportErrors


    def AutoBrowse(self, AutoBrowse=True):
        """
        Automatically browse to URL when clicked. set to False to
        receive EVT_HYPERLINK_LEFT event.
        """        
        self._AutoBrowse = AutoBrowse


    def DoPopup(self, DoPopup=True):
        """ Sets whether to show popup menu on right click or not. """        
        self._DoPopup = DoPopup

    
