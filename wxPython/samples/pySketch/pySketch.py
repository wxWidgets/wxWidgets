""" pySketch

    A simple object-oriented drawing program.

    This is completely free software; please feel free to adapt or use this in
    any way you like.

    Author: Erik Westra (ewestra@wave.co.nz)

    #########################################################################

    NOTE

    pySketch requires wxPython version 2.3.  If you are running an earlier
    version, you need to patch your copy of wxPython to fix a bug which will
    cause the "Edit Text Object" dialog box to crash.

    To patch an earlier version of wxPython, edit the wxPython/windows.py file,
    find the wxPyValidator.__init__ method and change the line which reads:

        self._setSelf(self, wxPyValidator, 0)

    to:

        self._setSelf(self, wxPyValidator, 1)

    This fixes a known bug in wxPython 2.2.5 (and possibly earlier) which has
    now been fixed in wxPython 2.3.

    #########################################################################

    TODO:

      * Add ARGV checking to see if a document was double-clicked on.

    Known Bugs:

      * Scrolling the window causes the drawing panel to be mucked up until you
        refresh it.  I've got no idea why.

      * I suspect that the reference counting for some wxPoint objects is
        getting mucked up; when the user quits, we get errors about being
        unable to call del on a 'None' object.
"""
import cPickle, os.path
from wxPython.wx import *

import traceback, types

#----------------------------------------------------------------------------
#                            System Constants
#----------------------------------------------------------------------------

# Our menu item IDs:

menu_UNDO          = 10001 # Edit menu items.
menu_SELECT_ALL    = 10002
menu_DUPLICATE     = 10003
menu_EDIT_TEXT     = 10004
menu_DELETE        = 10005

menu_SELECT        = 10101 # Tools menu items.
menu_LINE          = 10102
menu_RECT          = 10103
menu_ELLIPSE       = 10104
menu_TEXT          = 10105

menu_MOVE_FORWARD  = 10201 # Object menu items.
menu_MOVE_TO_FRONT = 10202
menu_MOVE_BACKWARD = 10203
menu_MOVE_TO_BACK  = 10204

menu_ABOUT         = 10205 # Help menu items.

# Our tool IDs:

id_SELECT  = 11001
id_LINE    = 11002
id_RECT    = 11003
id_ELLIPSE = 11004
id_TEXT    = 11005

# Our tool option IDs:

id_FILL_OPT   = 12001
id_PEN_OPT    = 12002
id_LINE_OPT   = 12003

id_LINESIZE_0 = 13001
id_LINESIZE_1 = 13002
id_LINESIZE_2 = 13003
id_LINESIZE_3 = 13004
id_LINESIZE_4 = 13005
id_LINESIZE_5 = 13006

# DrawObject type IDs:

obj_LINE    = 1
obj_RECT    = 2
obj_ELLIPSE = 3
obj_TEXT    = 4

# Selection handle IDs:

handle_NONE         = 1
handle_TOP_LEFT     = 2
handle_TOP_RIGHT    = 3
handle_BOTTOM_LEFT  = 4
handle_BOTTOM_RIGHT = 5
handle_START_POINT  = 6
handle_END_POINT    = 7

# Dragging operations:

drag_NONE   = 1
drag_RESIZE = 2
drag_MOVE   = 3
drag_DRAG   = 4

# Visual Feedback types:

feedback_LINE    = 1
feedback_RECT    = 2
feedback_ELLIPSE = 3

# Mouse-event action parameter types:

param_RECT = 1
param_LINE = 2

# Size of the drawing page, in pixels.

PAGE_WIDTH  = 1000
PAGE_HEIGHT = 1000

#----------------------------------------------------------------------------

class DrawingFrame(wxFrame):
    """ A frame showing the contents of a single document. """

    # ==========================================
    # == Initialisation and Window Management ==
    # ==========================================

    def __init__(self, parent, id, title, fileName=None):
        """ Standard constructor.

            'parent', 'id' and 'title' are all passed to the standard wxFrame
            constructor.  'fileName' is the name and path of a saved file to
            load into this frame, if any.
        """
        wxFrame.__init__(self, parent, id, title,
                         style = wxDEFAULT_FRAME_STYLE | wxWANTS_CHARS |
                                 wxNO_FULL_REPAINT_ON_RESIZE)

        # Setup our menu bar.

        menuBar = wxMenuBar()

        self.fileMenu = wxMenu()
        self.fileMenu.Append(wxID_NEW,    "New\tCTRL-N")
        self.fileMenu.Append(wxID_OPEN,   "Open...\tCTRL-O")
        self.fileMenu.Append(wxID_CLOSE,  "Close\tCTRL-W")
        self.fileMenu.AppendSeparator()
        self.fileMenu.Append(wxID_SAVE,   "Save\tCTRL-S")
        self.fileMenu.Append(wxID_SAVEAS, "Save As...")
        self.fileMenu.Append(wxID_REVERT, "Revert...")
        self.fileMenu.AppendSeparator()
        self.fileMenu.Append(wxID_EXIT,   "Quit\tCTRL-Q")

        menuBar.Append(self.fileMenu, "File")

        self.editMenu = wxMenu()
        self.editMenu.Append(menu_UNDO,          "Undo\tCTRL-Z")
        self.editMenu.AppendSeparator()
        self.editMenu.Append(menu_SELECT_ALL,    "Select All\tCTRL-A")
        self.editMenu.AppendSeparator()
        self.editMenu.Append(menu_DUPLICATE,     "Duplicate\tCTRL-D")
        self.editMenu.Append(menu_EDIT_TEXT,     "Edit...\tCTRL-E")
        self.editMenu.Append(menu_DELETE,        "Delete\tDEL")

        menuBar.Append(self.editMenu, "Edit")

        self.toolsMenu = wxMenu()
        self.toolsMenu.Append(menu_SELECT,  "Selection", kind=wxITEM_CHECK)
        self.toolsMenu.Append(menu_LINE,    "Line",      kind=wxITEM_CHECK)
        self.toolsMenu.Append(menu_RECT,    "Rectangle", kind=wxITEM_CHECK)
        self.toolsMenu.Append(menu_ELLIPSE, "Ellipse",   kind=wxITEM_CHECK)
        self.toolsMenu.Append(menu_TEXT,    "Text",      kind=wxITEM_CHECK)

        menuBar.Append(self.toolsMenu, "Tools")

        self.objectMenu = wxMenu()
        self.objectMenu.Append(menu_MOVE_FORWARD,  "Move Forward")
        self.objectMenu.Append(menu_MOVE_TO_FRONT, "Move to Front\tCTRL-F")
        self.objectMenu.Append(menu_MOVE_BACKWARD, "Move Backward")
        self.objectMenu.Append(menu_MOVE_TO_BACK,  "Move to Back\tCTRL-B")

        menuBar.Append(self.objectMenu, "Object")

        self.helpMenu = wxMenu()
        self.helpMenu.Append(menu_ABOUT, "About pySketch...")

        menuBar.Append(self.helpMenu, "Help")

        self.SetMenuBar(menuBar)

        # Create our toolbar.

        self.toolbar = self.CreateToolBar(wxTB_HORIZONTAL |
                                          wxNO_BORDER | wxTB_FLAT)

        self.toolbar.AddSimpleTool(wxID_NEW,
                                   wxBitmap("images/new.bmp",
                                            wxBITMAP_TYPE_BMP),
                                   "New")
        self.toolbar.AddSimpleTool(wxID_OPEN,
                                   wxBitmap("images/open.bmp",
                                            wxBITMAP_TYPE_BMP),
                                   "Open")
        self.toolbar.AddSimpleTool(wxID_SAVE,
                                   wxBitmap("images/save.bmp",
                                            wxBITMAP_TYPE_BMP),
                                   "Save")
        self.toolbar.AddSeparator()
        self.toolbar.AddSimpleTool(menu_UNDO,
                                   wxBitmap("images/undo.bmp",
                                            wxBITMAP_TYPE_BMP),
                                   "Undo")
        self.toolbar.AddSeparator()
        self.toolbar.AddSimpleTool(menu_DUPLICATE,
                                   wxBitmap("images/duplicate.bmp",
                                            wxBITMAP_TYPE_BMP),
                                   "Duplicate")
        self.toolbar.AddSeparator()
        self.toolbar.AddSimpleTool(menu_MOVE_FORWARD,
                                   wxBitmap("images/moveForward.bmp",
                                            wxBITMAP_TYPE_BMP),
                                   "Move Forward")
        self.toolbar.AddSimpleTool(menu_MOVE_BACKWARD,
                                   wxBitmap("images/moveBack.bmp",
                                            wxBITMAP_TYPE_BMP),
                                   "Move Backward")

        self.toolbar.Realize()

        # Associate each menu/toolbar item with the method that handles that
        # item.

        EVT_MENU(self, wxID_NEW,    self.doNew)
        EVT_MENU(self, wxID_OPEN,   self.doOpen)
        EVT_MENU(self, wxID_CLOSE,  self.doClose)
        EVT_MENU(self, wxID_SAVE,   self.doSave)
        EVT_MENU(self, wxID_SAVEAS, self.doSaveAs)
        EVT_MENU(self, wxID_REVERT, self.doRevert)
        EVT_MENU(self, wxID_EXIT,   self.doExit)

        EVT_MENU(self, menu_UNDO,          self.doUndo)
        EVT_MENU(self, menu_SELECT_ALL,    self.doSelectAll)
        EVT_MENU(self, menu_DUPLICATE,     self.doDuplicate)
        EVT_MENU(self, menu_EDIT_TEXT,     self.doEditText)
        EVT_MENU(self, menu_DELETE,        self.doDelete)

        EVT_MENU(self, menu_SELECT,  self.doChooseSelectTool)
        EVT_MENU(self, menu_LINE,    self.doChooseLineTool)
        EVT_MENU(self, menu_RECT,    self.doChooseRectTool)
        EVT_MENU(self, menu_ELLIPSE, self.doChooseEllipseTool)
        EVT_MENU(self, menu_TEXT,    self.doChooseTextTool)

        EVT_MENU(self, menu_MOVE_FORWARD,  self.doMoveForward)
        EVT_MENU(self, menu_MOVE_TO_FRONT, self.doMoveToFront)
        EVT_MENU(self, menu_MOVE_BACKWARD, self.doMoveBackward)
        EVT_MENU(self, menu_MOVE_TO_BACK,  self.doMoveToBack)

        EVT_MENU(self, menu_ABOUT, self.doShowAbout)

        # Install our own method to handle closing the window.  This allows us
        # to ask the user if he/she wants to save before closing the window, as
        # well as keeping track of which windows are currently open.

        EVT_CLOSE(self, self.doClose)

        # Install our own method for handling keystrokes.  We use this to let
        # the user move the selected object(s) around using the arrow keys.

        EVT_CHAR_HOOK(self, self.onKeyEvent)

        # Setup our top-most panel.  This holds the entire contents of the
        # window, excluding the menu bar.

        self.topPanel = wxPanel(self, -1, style=wxSIMPLE_BORDER)

        # Setup our tool palette, with all our drawing tools and option icons.

        self.toolPalette = wxBoxSizer(wxVERTICAL)

        self.selectIcon  = ToolPaletteIcon(self.topPanel, id_SELECT,
                                           "select", "Selection Tool")
        self.lineIcon    = ToolPaletteIcon(self.topPanel, id_LINE,
                                           "line", "Line Tool")
        self.rectIcon    = ToolPaletteIcon(self.topPanel, id_RECT,
                                           "rect", "Rectangle Tool")
        self.ellipseIcon = ToolPaletteIcon(self.topPanel, id_ELLIPSE,
                                           "ellipse", "Ellipse Tool")
        self.textIcon    = ToolPaletteIcon(self.topPanel, id_TEXT,
                                           "text", "Text Tool")

        toolSizer = wxGridSizer(0, 2, 5, 5)
        toolSizer.Add(self.selectIcon)
        toolSizer.Add(0, 0) # Gap to make tool icons line up nicely.
        toolSizer.Add(self.lineIcon)
        toolSizer.Add(self.rectIcon)
        toolSizer.Add(self.ellipseIcon)
        toolSizer.Add(self.textIcon)

        self.optionIndicator = ToolOptionIndicator(self.topPanel)
        self.optionIndicator.SetToolTip(
                wxToolTip("Shows Current Pen/Fill/Line Size Settings"))

        optionSizer = wxBoxSizer(wxHORIZONTAL)

        self.penOptIcon  = ToolPaletteIcon(self.topPanel, id_PEN_OPT,
                                           "penOpt", "Set Pen Colour")
        self.fillOptIcon = ToolPaletteIcon(self.topPanel, id_FILL_OPT,
                                           "fillOpt", "Set Fill Colour")
        self.lineOptIcon = ToolPaletteIcon(self.topPanel, id_LINE_OPT,
                                           "lineOpt", "Set Line Size")

        margin = wxLEFT | wxRIGHT
        optionSizer.Add(self.penOptIcon,  0, margin, 1)
        optionSizer.Add(self.fillOptIcon, 0, margin, 1)
        optionSizer.Add(self.lineOptIcon, 0, margin, 1)

        margin = wxTOP | wxLEFT | wxRIGHT | wxALIGN_CENTRE
        self.toolPalette.Add(toolSizer,            0, margin, 5)
        self.toolPalette.Add(0, 0,                 0, margin, 5) # Spacer.
        self.toolPalette.Add(self.optionIndicator, 0, margin, 5)
        self.toolPalette.Add(optionSizer,          0, margin, 5)

        # Make the tool palette icons respond when the user clicks on them.

        EVT_LEFT_DOWN(self.selectIcon,  self.onToolIconClick)
        EVT_LEFT_DOWN(self.lineIcon,    self.onToolIconClick)
        EVT_LEFT_DOWN(self.rectIcon,    self.onToolIconClick)
        EVT_LEFT_DOWN(self.ellipseIcon, self.onToolIconClick)
        EVT_LEFT_DOWN(self.textIcon,    self.onToolIconClick)
        EVT_LEFT_DOWN(self.penOptIcon,  self.onPenOptionIconClick)
        EVT_LEFT_DOWN(self.fillOptIcon, self.onFillOptionIconClick)
        EVT_LEFT_DOWN(self.lineOptIcon, self.onLineOptionIconClick)

        # Setup the main drawing area.

        self.drawPanel = wxScrolledWindow(self.topPanel, -1,
                                          style=wxSUNKEN_BORDER)
        self.drawPanel.SetBackgroundColour(wxWHITE)

        self.drawPanel.EnableScrolling(True, True)
        self.drawPanel.SetScrollbars(20, 20, PAGE_WIDTH / 20, PAGE_HEIGHT / 20)

        EVT_LEFT_DOWN(self.drawPanel, self.onMouseEvent)
        EVT_LEFT_DCLICK(self.drawPanel, self.onDoubleClickEvent)
        EVT_RIGHT_DOWN(self.drawPanel, self.onRightClick)
        EVT_MOTION(self.drawPanel, self.onMouseEvent)
        EVT_LEFT_UP(self.drawPanel, self.onMouseEvent)
        EVT_PAINT(self.drawPanel, self.onPaintEvent)

        # Position everything in the window.

        topSizer = wxBoxSizer(wxHORIZONTAL)
        topSizer.Add(self.toolPalette, 0)
        topSizer.Add(self.drawPanel, 1, wxEXPAND)

        self.topPanel.SetAutoLayout(True)
        self.topPanel.SetSizer(topSizer)

        self.SetSizeHints(minW=250, minH=200)
        self.SetSize(wxSize(600, 400))

        # Select an initial tool.

        self.curTool = None
        self._setCurrentTool(self.selectIcon)

        # Setup our frame to hold the contents of a sketch document.

        self.dirty     = False
        self.fileName  = fileName
        self.contents  = []     # front-to-back ordered list of DrawingObjects.
        self.selection = []     # List of selected DrawingObjects.
        self.undoInfo  = None   # Saved contents for undo.
        self.dragMode  = drag_NONE # Current mouse-drag mode.

        if self.fileName != None:
            self.loadContents()

        self._adjustMenus()

        # Finally, set our initial pen, fill and line options.

        self.penColour  = wxBLACK
        self.fillColour = wxWHITE
        self.lineSize   = 1

    # ============================
    # == Event Handling Methods ==
    # ============================

    def onToolIconClick(self, event):
        """ Respond to the user clicking on one of our tool icons.
        """
        iconID = wxPyTypeCast(event.GetEventObject(), "wxWindow").GetId()
        if   iconID == id_SELECT:   self.doChooseSelectTool()
        elif iconID == id_LINE:     self.doChooseLineTool()
        elif iconID == id_RECT:     self.doChooseRectTool()
        elif iconID == id_ELLIPSE:  self.doChooseEllipseTool()
        elif iconID == id_TEXT:     self.doChooseTextTool()
        else:                       wxBell()


    def onPenOptionIconClick(self, event):
        """ Respond to the user clicking on the "Pen Options" icon.
        """
        data = wxColourData()
        if len(self.selection) == 1:
            data.SetColour(self.selection[0].getPenColour())
        else:
            data.SetColour(self.penColour)

        dialog = wxColourDialog(self, data)
        if dialog.ShowModal() == wxID_OK:
            c = dialog.GetColourData().GetColour()
            self._setPenColour(wxColour(c.Red(), c.Green(), c.Blue()))
        dialog.Destroy()


    def onFillOptionIconClick(self, event):
        """ Respond to the user clicking on the "Fill Options" icon.
        """
        data = wxColourData()
        if len(self.selection) == 1:
            data.SetColour(self.selection[0].getFillColour())
        else:
            data.SetColour(self.fillColour)

        dialog = wxColourDialog(self, data)
        if dialog.ShowModal() == wxID_OK:
            c = dialog.GetColourData().GetColour()
            self._setFillColour(wxColour(c.Red(), c.Green(), c.Blue()))
        dialog.Destroy()

    def onLineOptionIconClick(self, event):
        """ Respond to the user clicking on the "Line Options" icon.
        """
        if len(self.selection) == 1:
            menu = self._buildLineSizePopup(self.selection[0].getLineSize())
        else:
            menu = self._buildLineSizePopup(self.lineSize)

        pos = self.lineOptIcon.GetPosition()
        pos.y = pos.y + self.lineOptIcon.GetSize().height
        self.PopupMenu(menu, pos)
        menu.Destroy()


    def onKeyEvent(self, event):
        """ Respond to a keypress event.

            We make the arrow keys move the selected object(s) by one pixel in
            the given direction.
        """
        if event.GetKeyCode() == WXK_UP:
            self._moveObject(0, -1)
        elif event.GetKeyCode() == WXK_DOWN:
            self._moveObject(0, 1)
        elif event.GetKeyCode() == WXK_LEFT:
            self._moveObject(-1, 0)
        elif event.GetKeyCode() == WXK_RIGHT:
            self._moveObject(1, 0)
        else:
            event.Skip()


    def onMouseEvent(self, event):
        """ Respond to the user clicking on our main drawing panel.

            How we respond depends on the currently selected tool.
        """
        if not (event.LeftDown() or event.Dragging() or event.LeftUp()):
            return # Ignore mouse movement without click/drag.

        if self.curTool == self.selectIcon:
            feedbackType = feedback_RECT
            action       = self.selectByRectangle
            actionParam  = param_RECT
            selecting    = True
            dashedLine   = True
        elif self.curTool == self.lineIcon:
            feedbackType = feedback_LINE
            action       = self.createLine
            actionParam  = param_LINE
            selecting    = False
            dashedLine   = False
        elif self.curTool == self.rectIcon:
            feedbackType = feedback_RECT
            action       = self.createRect
            actionParam  = param_RECT
            selecting    = False
            dashedLine   = False
        elif self.curTool == self.ellipseIcon:
            feedbackType = feedback_ELLIPSE
            action       = self.createEllipse
            actionParam  = param_RECT
            selecting    = False
            dashedLine   = False
        elif self.curTool == self.textIcon:
            feedbackType = feedback_RECT
            action       = self.createText
            actionParam  = param_RECT
            selecting    = False
            dashedLine   = True
        else:
            wxBell()
            return

        if event.LeftDown():
            mousePt = self._getEventCoordinates(event)
            if selecting:
                obj, handle = self._getObjectAndSelectionHandleAt(mousePt)

            if selecting and (obj != None) and (handle != handle_NONE):

                # The user clicked on an object's selection handle.  Let the
                # user resize the clicked-on object.

                self.dragMode     = drag_RESIZE
                self.resizeObject = obj

                if obj.getType() == obj_LINE:
                    self.resizeFeedback = feedback_LINE
                    pos  = obj.getPosition()
                    startPt = wxPoint(pos.x + obj.getStartPt().x,
                                      pos.y + obj.getStartPt().y)
                    endPt   = wxPoint(pos.x + obj.getEndPt().x,
                                      pos.y + obj.getEndPt().y)
                    if handle == handle_START_POINT:
                        self.resizeAnchor  = endPt
                        self.resizeFloater = startPt
                    else:
                        self.resizeAnchor  = startPt
                        self.resizeFloater = endPt
                else:
                    self.resizeFeedback = feedback_RECT
                    pos  = obj.getPosition()
                    size = obj.getSize()
                    topLeft  = wxPoint(pos.x, pos.y)
                    topRight = wxPoint(pos.x + size.width, pos.y)
                    botLeft  = wxPoint(pos.x, pos.y + size.height)
                    botRight = wxPoint(pos.x + size.width, pos.y + size.height)

                    if handle == handle_TOP_LEFT:
                        self.resizeAnchor  = botRight
                        self.resizeFloater = topLeft
                    elif handle == handle_TOP_RIGHT:
                        self.resizeAnchor  = botLeft
                        self.resizeFloater = topRight
                    elif handle == handle_BOTTOM_LEFT:
                        self.resizeAnchor  = topRight
                        self.resizeFloater = botLeft
                    elif handle == handle_BOTTOM_RIGHT:
                        self.resizeAnchor  = topLeft
                        self.resizeFloater = botRight

                self.curPt = mousePt
                self.resizeOffsetX = self.resizeFloater.x - mousePt.x
                self.resizeOffsetY = self.resizeFloater.y - mousePt.y
                endPt = wxPoint(self.curPt.x + self.resizeOffsetX,
                                self.curPt.y + self.resizeOffsetY)
                self._drawVisualFeedback(self.resizeAnchor, endPt,
                                         self.resizeFeedback, False)

            elif selecting and (self._getObjectAt(mousePt) != None):

                # The user clicked on an object to select it.  If the user
                # drags, he/she will move the object.

                self.select(self._getObjectAt(mousePt))
                self.dragMode = drag_MOVE
                self.moveOrigin = mousePt
                self.curPt      = mousePt
                self._drawObjectOutline(0, 0)

            else:

                # The user is dragging out a selection rect or new object.

                self.dragOrigin = mousePt
                self.curPt      = mousePt
                self.drawPanel.SetCursor(wxCROSS_CURSOR)
                self.drawPanel.CaptureMouse()
                self._drawVisualFeedback(mousePt, mousePt, feedbackType,
                                         dashedLine)
                self.dragMode = drag_DRAG

            event.Skip()
            return

        if event.Dragging():
            if self.dragMode == drag_RESIZE:

                # We're resizing an object.

                mousePt = self._getEventCoordinates(event)
                if (self.curPt.x != mousePt.x) or (self.curPt.y != mousePt.y):
                    # Erase previous visual feedback.
                    endPt = wxPoint(self.curPt.x + self.resizeOffsetX,
                                    self.curPt.y + self.resizeOffsetY)
                    self._drawVisualFeedback(self.resizeAnchor, endPt,
                                             self.resizeFeedback, False)
                    self.curPt = mousePt
                    # Draw new visual feedback.
                    endPt = wxPoint(self.curPt.x + self.resizeOffsetX,
                                    self.curPt.y + self.resizeOffsetY)
                    self._drawVisualFeedback(self.resizeAnchor, endPt,
                                             self.resizeFeedback, False)

            elif self.dragMode == drag_MOVE:

                # We're moving a selected object.

                mousePt = self._getEventCoordinates(event)
                if (self.curPt.x != mousePt.x) or (self.curPt.y != mousePt.y):
                    # Erase previous visual feedback.
                    self._drawObjectOutline(self.curPt.x - self.moveOrigin.x,
                                            self.curPt.y - self.moveOrigin.y)
                    self.curPt = mousePt
                    # Draw new visual feedback.
                    self._drawObjectOutline(self.curPt.x - self.moveOrigin.x,
                                            self.curPt.y - self.moveOrigin.y)

            elif self.dragMode == drag_DRAG:

                # We're dragging out a new object or selection rect.

                mousePt = self._getEventCoordinates(event)
                if (self.curPt.x != mousePt.x) or (self.curPt.y != mousePt.y):
                    # Erase previous visual feedback.
                    self._drawVisualFeedback(self.dragOrigin, self.curPt,
                                             feedbackType, dashedLine)
                    self.curPt = mousePt
                    # Draw new visual feedback.
                    self._drawVisualFeedback(self.dragOrigin, self.curPt,
                                             feedbackType, dashedLine)

            event.Skip()
            return

        if event.LeftUp():
            if self.dragMode == drag_RESIZE:

                # We're resizing an object.

                mousePt = self._getEventCoordinates(event)
                # Erase last visual feedback.
                endPt = wxPoint(self.curPt.x + self.resizeOffsetX,
                                self.curPt.y + self.resizeOffsetY)
                self._drawVisualFeedback(self.resizeAnchor, endPt,
                                         self.resizeFeedback, False)

                resizePt = wxPoint(mousePt.x + self.resizeOffsetX,
                                   mousePt.y + self.resizeOffsetY)

                if (self.resizeFloater.x != resizePt.x) or \
                   (self.resizeFloater.y != resizePt.y):
                   self._resizeObject(self.resizeObject,
                                      self.resizeAnchor,
                                      self.resizeFloater,
                                      resizePt)
                else:
                    self.drawPanel.Refresh() # Clean up after empty resize.

            elif self.dragMode == drag_MOVE:

                # We're moving a selected object.

                mousePt = self._getEventCoordinates(event)
                # Erase last visual feedback.
                self._drawObjectOutline(self.curPt.x - self.moveOrigin.x,
                                        self.curPt.y - self.moveOrigin.y)
                if (self.moveOrigin.x != mousePt.x) or \
                   (self.moveOrigin.y != mousePt.y):
                    self._moveObject(mousePt.x - self.moveOrigin.x,
                                     mousePt.y - self.moveOrigin.y)
                else:
                    self.drawPanel.Refresh() # Clean up after empty drag.

            elif self.dragMode == drag_DRAG:

                # We're dragging out a new object or selection rect.

                mousePt = self._getEventCoordinates(event)
                # Erase last visual feedback.
                self._drawVisualFeedback(self.dragOrigin, self.curPt,
                                         feedbackType, dashedLine)
                self.drawPanel.ReleaseMouse()
                self.drawPanel.SetCursor(wxSTANDARD_CURSOR)
                # Perform the appropriate action for the current tool.
                if actionParam == param_RECT:
                    x1 = min(self.dragOrigin.x, self.curPt.x)
                    y1 = min(self.dragOrigin.y, self.curPt.y)
                    x2 = max(self.dragOrigin.x, self.curPt.x)
                    y2 = max(self.dragOrigin.y, self.curPt.y)

                    startX = x1
                    startY = y1
                    width  = x2 - x1
                    height = y2 - y1

                    if not selecting:
                        if ((x2-x1) < 8) or ((y2-y1) < 8): return # Too small.

                    action(x1, y1, x2-x1, y2-y1)
                elif actionParam == param_LINE:
                    action(self.dragOrigin.x, self.dragOrigin.y,
                           self.curPt.x, self.curPt.y)

            self.dragMode = drag_NONE # We've finished with this mouse event.
            event.Skip()


    def onDoubleClickEvent(self, event):
        """ Respond to a double-click within our drawing panel.
        """
        mousePt = self._getEventCoordinates(event)
        obj = self._getObjectAt(mousePt)
        if obj == None: return

        # Let the user edit the given object.

        if obj.getType() == obj_TEXT:
            editor = EditTextObjectDialog(self, "Edit Text Object")
            editor.objectToDialog(obj)
            if editor.ShowModal() == wxID_CANCEL:
                editor.Destroy()
                return

            self._saveUndoInfo()

            editor.dialogToObject(obj)
            editor.Destroy()

            self.dirty = True
            self.drawPanel.Refresh()
            self._adjustMenus()
        else:
            wxBell()


    def onRightClick(self, event):
        """ Respond to the user right-clicking within our drawing panel.

            We select the clicked-on item, if necessary, and display a pop-up
            menu of available options which can be applied to the selected
            item(s).
        """
        mousePt = self._getEventCoordinates(event)
        obj = self._getObjectAt(mousePt)

        if obj == None: return # Nothing selected.

        # Select the clicked-on object.

        self.select(obj)

        # Build our pop-up menu.

        menu = wxMenu()
        menu.Append(menu_DUPLICATE, "Duplicate")
        menu.Append(menu_EDIT_TEXT, "Edit...")
        menu.Append(menu_DELETE,    "Delete")
        menu.AppendSeparator()
        menu.Append(menu_MOVE_FORWARD,   "Move Forward")
        menu.Append(menu_MOVE_TO_FRONT,  "Move to Front")
        menu.Append(menu_MOVE_BACKWARD,  "Move Backward")
        menu.Append(menu_MOVE_TO_BACK,   "Move to Back")

        menu.Enable(menu_EDIT_TEXT,     obj.getType() == obj_TEXT)
        menu.Enable(menu_MOVE_FORWARD,  obj != self.contents[0])
        menu.Enable(menu_MOVE_TO_FRONT, obj != self.contents[0])
        menu.Enable(menu_MOVE_BACKWARD, obj != self.contents[-1])
        menu.Enable(menu_MOVE_TO_BACK,  obj != self.contents[-1])

        EVT_MENU(self, menu_DUPLICATE,     self.doDuplicate)
        EVT_MENU(self, menu_EDIT_TEXT,     self.doEditText)
        EVT_MENU(self, menu_DELETE,        self.doDelete)
        EVT_MENU(self, menu_MOVE_FORWARD,  self.doMoveForward)
        EVT_MENU(self, menu_MOVE_TO_FRONT, self.doMoveToFront)
        EVT_MENU(self, menu_MOVE_BACKWARD, self.doMoveBackward)
        EVT_MENU(self, menu_MOVE_TO_BACK,  self.doMoveToBack)

        # Show the pop-up menu.

        clickPt = wxPoint(mousePt.x + self.drawPanel.GetPosition().x,
                          mousePt.y + self.drawPanel.GetPosition().y)
        self.drawPanel.PopupMenu(menu, clickPt)
        menu.Destroy()


    def onPaintEvent(self, event):
        """ Respond to a request to redraw the contents of our drawing panel.
        """
        dc = wxPaintDC(self.drawPanel)
        self.drawPanel.PrepareDC(dc)
        dc.BeginDrawing()

        for i in range(len(self.contents)-1, -1, -1):
            obj = self.contents[i]
            if obj in self.selection:
                obj.draw(dc, True)
            else:
                obj.draw(dc, False)

        dc.EndDrawing()

    # ==========================
    # == Menu Command Methods ==
    # ==========================

    def doNew(self, event):
        """ Respond to the "New" menu command.
        """
        global _docList
        newFrame = DrawingFrame(None, -1, "Untitled")
        newFrame.Show(True)
        _docList.append(newFrame)


    def doOpen(self, event):
        """ Respond to the "Open" menu command.
        """
        global _docList

        curDir = os.getcwd()
        fileName = wxFileSelector("Open File", default_extension="psk",
                                  flags = wxOPEN | wxFILE_MUST_EXIST)
        if fileName == "": return
        fileName = os.path.join(os.getcwd(), fileName)
        os.chdir(curDir)

        title = os.path.basename(fileName)

        if (self.fileName == None) and (len(self.contents) == 0):
            # Load contents into current (empty) document.
            self.fileName = fileName
            self.SetTitle(os.path.basename(fileName))
            self.loadContents()
        else:
            # Open a new frame for this document.
            newFrame = DrawingFrame(None, -1, os.path.basename(fileName),
                                    fileName=fileName)
            newFrame.Show(True)
            _docList.append(newFrame)


    def doClose(self, event):
        """ Respond to the "Close" menu command.
        """
        global _docList

        if self.dirty:
            if not self.askIfUserWantsToSave("closing"): return

        _docList.remove(self)
        self.Destroy()


    def doSave(self, event):
        """ Respond to the "Save" menu command.
        """
        if self.fileName != None:
            self.saveContents()


    def doSaveAs(self, event):
        """ Respond to the "Save As" menu command.
        """
        if self.fileName == None:
            default = ""
        else:
            default = self.fileName

        curDir = os.getcwd()
        fileName = wxFileSelector("Save File As", "Saving",
                                  default_filename=default,
                                  default_extension="psk",
                                  wildcard="*.psk",
                                  flags = wxSAVE | wxOVERWRITE_PROMPT)
        if fileName == "": return # User cancelled.
        fileName = os.path.join(os.getcwd(), fileName)
        os.chdir(curDir)

        title = os.path.basename(fileName)
        self.SetTitle(title)

        self.fileName = fileName
        self.saveContents()


    def doRevert(self, event):
        """ Respond to the "Revert" menu command.
        """
        if not self.dirty: return

        if wxMessageBox("Discard changes made to this document?", "Confirm",
                        style = wxOK | wxCANCEL | wxICON_QUESTION,
                        parent=self) == wxCANCEL: return
        self.loadContents()


    def doExit(self, event):
        """ Respond to the "Quit" menu command.
        """
        global _docList, _app
        for doc in _docList:
            if not doc.dirty: continue
            doc.Raise()
            if not doc.askIfUserWantsToSave("quitting"): return
            _docList.remove(doc)
            doc.Destroy()

        _app.ExitMainLoop()


    def doUndo(self, event):
        """ Respond to the "Undo" menu command.
        """
        if self.undoInfo == None: return

        undoData = self.undoInfo
        self._saveUndoInfo() # For undoing the undo...

        self.contents = []

        for type, data in undoData["contents"]:
            obj = DrawingObject(type)
            obj.setData(data)
            self.contents.append(obj)

        self.selection = []
        for i in undoData["selection"]:
            self.selection.append(self.contents[i])

        self.dirty = True
        self.drawPanel.Refresh()
        self._adjustMenus()


    def doSelectAll(self, event):
        """ Respond to the "Select All" menu command.
        """
        self.selectAll()


    def doDuplicate(self, event):
        """ Respond to the "Duplicate" menu command.
        """
        self._saveUndoInfo()

        objs = []
        for obj in self.contents:
            if obj in self.selection:
                newObj = DrawingObject(obj.getType())
                newObj.setData(obj.getData())
                pos = obj.getPosition()
                newObj.setPosition(wxPoint(pos.x + 10, pos.y + 10))
                objs.append(newObj)

        self.contents = objs + self.contents

        self.selectMany(objs)


    def doEditText(self, event):
        """ Respond to the "Edit Text" menu command.
        """
        if len(self.selection) != 1: return

        obj = self.selection[0]
        if obj.getType() != obj_TEXT: return

        editor = EditTextObjectDialog(self, "Edit Text Object")
        editor.objectToDialog(obj)
        if editor.ShowModal() == wxID_CANCEL:
            editor.Destroy()
            return

        self._saveUndoInfo()

        editor.dialogToObject(obj)
        editor.Destroy()

        self.dirty = True
        self.drawPanel.Refresh()
        self._adjustMenus()


    def doDelete(self, event):
        """ Respond to the "Delete" menu command.
        """
        self._saveUndoInfo()

        for obj in self.selection:
            self.contents.remove(obj)
            del obj
        self.deselectAll()


    def doChooseSelectTool(self, event=None):
        """ Respond to the "Select Tool" menu command.
        """
        self._setCurrentTool(self.selectIcon)
        self.drawPanel.SetCursor(wxSTANDARD_CURSOR)
        self._adjustMenus()


    def doChooseLineTool(self, event=None):
        """ Respond to the "Line Tool" menu command.
        """
        self._setCurrentTool(self.lineIcon)
        self.drawPanel.SetCursor(wxCROSS_CURSOR)
        self.deselectAll()
        self._adjustMenus()


    def doChooseRectTool(self, event=None):
        """ Respond to the "Rect Tool" menu command.
        """
        self._setCurrentTool(self.rectIcon)
        self.drawPanel.SetCursor(wxCROSS_CURSOR)
        self.deselectAll()
        self._adjustMenus()


    def doChooseEllipseTool(self, event=None):
        """ Respond to the "Ellipse Tool" menu command.
        """
        self._setCurrentTool(self.ellipseIcon)
        self.drawPanel.SetCursor(wxCROSS_CURSOR)
        self.deselectAll()
        self._adjustMenus()


    def doChooseTextTool(self, event=None):
        """ Respond to the "Text Tool" menu command.
        """
        self._setCurrentTool(self.textIcon)
        self.drawPanel.SetCursor(wxCROSS_CURSOR)
        self.deselectAll()
        self._adjustMenus()


    def doMoveForward(self, event):
        """ Respond to the "Move Forward" menu command.
        """
        if len(self.selection) != 1: return

        self._saveUndoInfo()

        obj = self.selection[0]
        index = self.contents.index(obj)
        if index == 0: return

        del self.contents[index]
        self.contents.insert(index-1, obj)

        self.drawPanel.Refresh()
        self._adjustMenus()


    def doMoveToFront(self, event):
        """ Respond to the "Move to Front" menu command.
        """
        if len(self.selection) != 1: return

        self._saveUndoInfo()

        obj = self.selection[0]
        self.contents.remove(obj)
        self.contents.insert(0, obj)

        self.drawPanel.Refresh()
        self._adjustMenus()


    def doMoveBackward(self, event):
        """ Respond to the "Move Backward" menu command.
        """
        if len(self.selection) != 1: return

        self._saveUndoInfo()

        obj = self.selection[0]
        index = self.contents.index(obj)
        if index == len(self.contents) - 1: return

        del self.contents[index]
        self.contents.insert(index+1, obj)

        self.drawPanel.Refresh()
        self._adjustMenus()


    def doMoveToBack(self, event):
        """ Respond to the "Move to Back" menu command.
        """
        if len(self.selection) != 1: return

        self._saveUndoInfo()

        obj = self.selection[0]
        self.contents.remove(obj)
        self.contents.append(obj)

        self.drawPanel.Refresh()
        self._adjustMenus()


    def doShowAbout(self, event):
        """ Respond to the "About pySketch" menu command.
        """
        dialog = wxDialog(self, -1, "About pySketch") # ,
                          #style=wxDIALOG_MODAL | wxSTAY_ON_TOP)
        dialog.SetBackgroundColour(wxWHITE)

        panel = wxPanel(dialog, -1)
        panel.SetBackgroundColour(wxWHITE)

        panelSizer = wxBoxSizer(wxVERTICAL)

        boldFont = wxFont(panel.GetFont().GetPointSize(),
                          panel.GetFont().GetFamily(),
                          wxNORMAL, wxBOLD)

        logo = wxStaticBitmap(panel, -1, wxBitmap("images/logo.bmp",
                                                  wxBITMAP_TYPE_BMP))

        lab1 = wxStaticText(panel, -1, "pySketch")
        lab1.SetFont(wxFont(36, boldFont.GetFamily(), wxITALIC, wxBOLD))
        lab1.SetSize(lab1.GetBestSize())

        imageSizer = wxBoxSizer(wxHORIZONTAL)
        imageSizer.Add(logo, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5)
        imageSizer.Add(lab1, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5)

        lab2 = wxStaticText(panel, -1, "A simple object-oriented drawing " + \
                                       "program.")
        lab2.SetFont(boldFont)
        lab2.SetSize(lab2.GetBestSize())

        lab3 = wxStaticText(panel, -1, "pySketch is completely free " + \
                                       "software; please")
        lab3.SetFont(boldFont)
        lab3.SetSize(lab3.GetBestSize())

        lab4 = wxStaticText(panel, -1, "feel free to adapt or use this " + \
                                       "in any way you like.")
        lab4.SetFont(boldFont)
        lab4.SetSize(lab4.GetBestSize())

        lab5 = wxStaticText(panel, -1, "Author: Erik Westra " + \
                                       "(ewestra@wave.co.nz)")
        lab5.SetFont(boldFont)
        lab5.SetSize(lab5.GetBestSize())

        btnOK = wxButton(panel, wxID_OK, "OK")

        panelSizer.Add(imageSizer, 0, wxALIGN_CENTRE)
        panelSizer.Add(10, 10) # Spacer.
        panelSizer.Add(lab2, 0, wxALIGN_CENTRE)
        panelSizer.Add(10, 10) # Spacer.
        panelSizer.Add(lab3, 0, wxALIGN_CENTRE)
        panelSizer.Add(lab4, 0, wxALIGN_CENTRE)
        panelSizer.Add(10, 10) # Spacer.
        panelSizer.Add(lab5, 0, wxALIGN_CENTRE)
        panelSizer.Add(10, 10) # Spacer.
        panelSizer.Add(btnOK, 0, wxALL | wxALIGN_CENTRE, 5)

        panel.SetAutoLayout(True)
        panel.SetSizer(panelSizer)
        panelSizer.Fit(panel)

        topSizer = wxBoxSizer(wxHORIZONTAL)
        topSizer.Add(panel, 0, wxALL, 10)

        dialog.SetAutoLayout(True)
        dialog.SetSizer(topSizer)
        topSizer.Fit(dialog)

        dialog.Centre()

        btn = dialog.ShowModal()
        dialog.Destroy()

    # =============================
    # == Object Creation Methods ==
    # =============================

    def createLine(self, x1, y1, x2, y2):
        """ Create a new line object at the given position and size.
        """
        self._saveUndoInfo()

        topLeftX  = min(x1, x2)
        topLeftY  = min(y1, y2)
        botRightX = max(x1, x2)
        botRightY = max(y1, y2)

        obj = DrawingObject(obj_LINE, position=wxPoint(topLeftX, topLeftY),
                            size=wxSize(botRightX-topLeftX,
                                        botRightY-topLeftY),
                            penColour=self.penColour,
                            fillColour=self.fillColour,
                            lineSize=self.lineSize,
                            startPt = wxPoint(x1 - topLeftX, y1 - topLeftY),
                            endPt   = wxPoint(x2 - topLeftX, y2 - topLeftY))
        self.contents.insert(0, obj)
        self.dirty = True
        self.doChooseSelectTool()
        self.select(obj)


    def createRect(self, x, y, width, height):
        """ Create a new rectangle object at the given position and size.
        """
        self._saveUndoInfo()

        obj = DrawingObject(obj_RECT, position=wxPoint(x, y),
                            size=wxSize(width, height),
                            penColour=self.penColour,
                            fillColour=self.fillColour,
                            lineSize=self.lineSize)
        self.contents.insert(0, obj)
        self.dirty = True
        self.doChooseSelectTool()
        self.select(obj)


    def createEllipse(self, x, y, width, height):
        """ Create a new ellipse object at the given position and size.
        """
        self._saveUndoInfo()

        obj = DrawingObject(obj_ELLIPSE, position=wxPoint(x, y),
                            size=wxSize(width, height),
                            penColour=self.penColour,
                            fillColour=self.fillColour,
                            lineSize=self.lineSize)
        self.contents.insert(0, obj)
        self.dirty = True
        self.doChooseSelectTool()
        self.select(obj)


    def createText(self, x, y, width, height):
        """ Create a new text object at the given position and size.
        """
        editor = EditTextObjectDialog(self, "Create Text Object")
        if editor.ShowModal() == wxID_CANCEL:
            editor.Destroy()
            return

        self._saveUndoInfo()

        obj = DrawingObject(obj_TEXT, position=wxPoint(x, y),
                                      size=wxSize(width, height))
        editor.dialogToObject(obj)
        editor.Destroy()

        self.contents.insert(0, obj)
        self.dirty = True
        self.doChooseSelectTool()
        self.select(obj)

    # =======================
    # == Selection Methods ==
    # =======================

    def selectAll(self):
        """ Select every DrawingObject in our document.
        """
        self.selection = []
        for obj in self.contents:
            self.selection.append(obj)
        self.drawPanel.Refresh()
        self._adjustMenus()


    def deselectAll(self):
        """ Deselect every DrawingObject in our document.
        """
        self.selection = []
        self.drawPanel.Refresh()
        self._adjustMenus()


    def select(self, obj):
        """ Select the given DrawingObject within our document.
        """
        self.selection = [obj]
        self.drawPanel.Refresh()
        self._adjustMenus()


    def selectMany(self, objs):
        """ Select the given list of DrawingObjects.
        """
        self.selection = objs
        self.drawPanel.Refresh()
        self._adjustMenus()


    def selectByRectangle(self, x, y, width, height):
        """ Select every DrawingObject in the given rectangular region.
        """
        self.selection = []
        for obj in self.contents:
            if obj.objectWithinRect(x, y, width, height):
                self.selection.append(obj)
        self.drawPanel.Refresh()
        self._adjustMenus()

    # ======================
    # == File I/O Methods ==
    # ======================

    def loadContents(self):
        """ Load the contents of our document into memory.
        """
        f = open(self.fileName, "rb")
        objData = cPickle.load(f)
        f.close()

        for type, data in objData:
            obj = DrawingObject(type)
            obj.setData(data)
            self.contents.append(obj)

        self.dirty = False
        self.selection = []
        self.undoInfo  = None

        self.drawPanel.Refresh()
        self._adjustMenus()


    def saveContents(self):
        """ Save the contents of our document to disk.
        """
        objData = []
        for obj in self.contents:
            objData.append([obj.getType(), obj.getData()])

        f = open(self.fileName, "wb")
        cPickle.dump(objData, f)
        f.close()

        self.dirty = False


    def askIfUserWantsToSave(self, action):
        """ Give the user the opportunity to save the current document.

            'action' is a string describing the action about to be taken.  If
            the user wants to save the document, it is saved immediately.  If
            the user cancels, we return False.
        """
        if not self.dirty: return True # Nothing to do.

        response = wxMessageBox("Save changes before " + action + "?",
                                "Confirm", wxYES_NO | wxCANCEL, self)

        if response == wxYES:
            if self.fileName == None:
                fileName = wxFileSelector("Save File As", "Saving",
                                          default_extension="psk",
                                          wildcard="*.psk",
                                          flags = wxSAVE | wxOVERWRITE_PROMPT)
                if fileName == "": return False # User cancelled.
                self.fileName = fileName

            self.saveContents()
            return True
        elif response == wxNO:
            return True # User doesn't want changes saved.
        elif response == wxCANCEL:
            return False # User cancelled.

    # =====================
    # == Private Methods ==
    # =====================

    def _adjustMenus(self):
        """ Adjust our menus and toolbar to reflect the current state of the
            world.
        """
        canSave   = (self.fileName != None) and self.dirty
        canRevert = (self.fileName != None) and self.dirty
        canUndo   = self.undoInfo != None
        selection = len(self.selection) > 0
        onlyOne   = len(self.selection) == 1
        isText    = onlyOne and (self.selection[0].getType() == obj_TEXT)
        front     = onlyOne and (self.selection[0] == self.contents[0])
        back      = onlyOne and (self.selection[0] == self.contents[-1])

        # Enable/disable our menu items.

        self.fileMenu.Enable(wxID_SAVE,   canSave)
        self.fileMenu.Enable(wxID_REVERT, canRevert)

        self.editMenu.Enable(menu_UNDO,      canUndo)
        self.editMenu.Enable(menu_DUPLICATE, selection)
        self.editMenu.Enable(menu_EDIT_TEXT, isText)
        self.editMenu.Enable(menu_DELETE,    selection)

        self.toolsMenu.Check(menu_SELECT,  self.curTool == self.selectIcon)
        self.toolsMenu.Check(menu_LINE,    self.curTool == self.lineIcon)
        self.toolsMenu.Check(menu_RECT,    self.curTool == self.rectIcon)
        self.toolsMenu.Check(menu_ELLIPSE, self.curTool == self.ellipseIcon)
        self.toolsMenu.Check(menu_TEXT,    self.curTool == self.textIcon)

        self.objectMenu.Enable(menu_MOVE_FORWARD,  onlyOne and not front)
        self.objectMenu.Enable(menu_MOVE_TO_FRONT, onlyOne and not front)
        self.objectMenu.Enable(menu_MOVE_BACKWARD, onlyOne and not back)
        self.objectMenu.Enable(menu_MOVE_TO_BACK,  onlyOne and not back)

        # Enable/disable our toolbar icons.

        self.toolbar.EnableTool(wxID_NEW,           True)
        self.toolbar.EnableTool(wxID_OPEN,          True)
        self.toolbar.EnableTool(wxID_SAVE,          canSave)
        self.toolbar.EnableTool(menu_UNDO,          canUndo)
        self.toolbar.EnableTool(menu_DUPLICATE,     selection)
        self.toolbar.EnableTool(menu_MOVE_FORWARD,  onlyOne and not front)
        self.toolbar.EnableTool(menu_MOVE_BACKWARD, onlyOne and not back)


    def _setCurrentTool(self, newToolIcon):
        """ Set the currently selected tool.
        """
        if self.curTool == newToolIcon: return # Nothing to do.

        if self.curTool != None:
            self.curTool.deselect()

        newToolIcon.select()
        self.curTool = newToolIcon


    def _setPenColour(self, colour):
        """ Set the default or selected object's pen colour.
        """
        if len(self.selection) > 0:
            self._saveUndoInfo()
            for obj in self.selection:
                obj.setPenColour(colour)
            self.drawPanel.Refresh()
        else:
            self.penColour = colour
            self.optionIndicator.setPenColour(colour)


    def _setFillColour(self, colour):
        """ Set the default or selected object's fill colour.
        """
        if len(self.selection) > 0:
            self._saveUndoInfo()
            for obj in self.selection:
                obj.setFillColour(colour)
            self.drawPanel.Refresh()
        else:
            self.fillColour = colour
            self.optionIndicator.setFillColour(colour)


    def _setLineSize(self, size):
        """ Set the default or selected object's line size.
        """
        if len(self.selection) > 0:
            self._saveUndoInfo()
            for obj in self.selection:
                obj.setLineSize(size)
            self.drawPanel.Refresh()
        else:
            self.lineSize = size
            self.optionIndicator.setLineSize(size)


    def _saveUndoInfo(self):
        """ Remember the current state of the document, to allow for undo.

            We make a copy of the document's contents, so that we can return to
            the previous contents if the user does something and then wants to
            undo the operation.
        """
        savedContents = []
        for obj in self.contents:
            savedContents.append([obj.getType(), obj.getData()])

        savedSelection = []
        for i in range(len(self.contents)):
            if self.contents[i] in self.selection:
                savedSelection.append(i)

        self.undoInfo = {"contents"  : savedContents,
                         "selection" : savedSelection}


    def _resizeObject(self, obj, anchorPt, oldPt, newPt):
        """ Resize the given object.

            'anchorPt' is the unchanging corner of the object, while the
            opposite corner has been resized.  'oldPt' are the current
            coordinates for this corner, while 'newPt' are the new coordinates.
            The object should fit within the given dimensions, though if the
            new point is less than the anchor point the object will need to be
            moved as well as resized, to avoid giving it a negative size.
        """
        if obj.getType() == obj_TEXT:
            # Not allowed to resize text objects -- they're sized to fit text.
            wxBell()
            return

        self._saveUndoInfo()

        topLeft  = wxPoint(min(anchorPt.x, newPt.x),
                           min(anchorPt.y, newPt.y))
        botRight = wxPoint(max(anchorPt.x, newPt.x),
                           max(anchorPt.y, newPt.y))

        newWidth  = botRight.x - topLeft.x
        newHeight = botRight.y - topLeft.y

        if obj.getType() == obj_LINE:
            # Adjust the line so that its start and end points match the new
            # overall object size.

            startPt = obj.getStartPt()
            endPt   = obj.getEndPt()

            slopesDown = ((startPt.x < endPt.x) and (startPt.y < endPt.y)) or \
                         ((startPt.x > endPt.x) and (startPt.y > endPt.y))

            # Handle the user flipping the line.

            hFlip = ((anchorPt.x < oldPt.x) and (anchorPt.x > newPt.x)) or \
                    ((anchorPt.x > oldPt.x) and (anchorPt.x < newPt.x))
            vFlip = ((anchorPt.y < oldPt.y) and (anchorPt.y > newPt.y)) or \
                    ((anchorPt.y > oldPt.y) and (anchorPt.y < newPt.y))

            if (hFlip and not vFlip) or (vFlip and not hFlip):
                slopesDown = not slopesDown # Line flipped.

            if slopesDown:
                obj.setStartPt(wxPoint(0, 0))
                obj.setEndPt(wxPoint(newWidth, newHeight))
            else:
                obj.setStartPt(wxPoint(0, newHeight))
                obj.setEndPt(wxPoint(newWidth, 0))

        # Finally, adjust the bounds of the object to match the new dimensions.

        obj.setPosition(topLeft)
        obj.setSize(wxSize(botRight.x - topLeft.x, botRight.y - topLeft.y))

        self.drawPanel.Refresh()


    def _moveObject(self, offsetX, offsetY):
        """ Move the currently selected object(s) by the given offset.
        """
        self._saveUndoInfo()

        for obj in self.selection:
            pos = obj.getPosition()
            pos.x = pos.x + offsetX
            pos.y = pos.y + offsetY
            obj.setPosition(pos)

        self.drawPanel.Refresh()


    def _buildLineSizePopup(self, lineSize):
        """ Build the pop-up menu used to set the line size.

            'lineSize' is the current line size value.  The corresponding item
            is checked in the pop-up menu.
        """
        menu = wxMenu()
        menu.Append(id_LINESIZE_0, "no line",      kind=wxITEM_CHECK)
        menu.Append(id_LINESIZE_1, "1-pixel line", kind=wxITEM_CHECK)
        menu.Append(id_LINESIZE_2, "2-pixel line", kind=wxITEM_CHECK)
        menu.Append(id_LINESIZE_3, "3-pixel line", kind=wxITEM_CHECK)
        menu.Append(id_LINESIZE_4, "4-pixel line", kind=wxITEM_CHECK)
        menu.Append(id_LINESIZE_5, "5-pixel line", kind=wxITEM_CHECK)

        if   lineSize == 0: menu.Check(id_LINESIZE_0, True)
        elif lineSize == 1: menu.Check(id_LINESIZE_1, True)
        elif lineSize == 2: menu.Check(id_LINESIZE_2, True)
        elif lineSize == 3: menu.Check(id_LINESIZE_3, True)
        elif lineSize == 4: menu.Check(id_LINESIZE_4, True)
        elif lineSize == 5: menu.Check(id_LINESIZE_5, True)

        EVT_MENU(self, id_LINESIZE_0, self._lineSizePopupSelected)
        EVT_MENU(self, id_LINESIZE_1, self._lineSizePopupSelected)
        EVT_MENU(self, id_LINESIZE_2, self._lineSizePopupSelected)
        EVT_MENU(self, id_LINESIZE_3, self._lineSizePopupSelected)
        EVT_MENU(self, id_LINESIZE_4, self._lineSizePopupSelected)
        EVT_MENU(self, id_LINESIZE_5, self._lineSizePopupSelected)

        return menu


    def _lineSizePopupSelected(self, event):
        """ Respond to the user selecting an item from the line size popup menu
        """
        id = event.GetId()
        if   id == id_LINESIZE_0: self._setLineSize(0)
        elif id == id_LINESIZE_1: self._setLineSize(1)
        elif id == id_LINESIZE_2: self._setLineSize(2)
        elif id == id_LINESIZE_3: self._setLineSize(3)
        elif id == id_LINESIZE_4: self._setLineSize(4)
        elif id == id_LINESIZE_5: self._setLineSize(5)
        else:
            wxBell()
            return

        self.optionIndicator.setLineSize(self.lineSize)


    def _getEventCoordinates(self, event):
        """ Return the coordinates associated with the given mouse event.

            The coordinates have to be adjusted to allow for the current scroll
            position.
        """
        originX, originY = self.drawPanel.GetViewStart()
        unitX, unitY = self.drawPanel.GetScrollPixelsPerUnit()
        return wxPoint(event.GetX() + (originX * unitX),
                       event.GetY() + (originY * unitY))


    def _getObjectAndSelectionHandleAt(self, pt):
        """ Return the object and selection handle at the given point.

            We draw selection handles (small rectangles) around the currently
            selected object(s).  If the given point is within one of the
            selection handle rectangles, we return the associated object and a
            code indicating which selection handle the point is in.  If the
            point isn't within any selection handle at all, we return the tuple
            (None, handle_NONE).
        """
        for obj in self.selection:
            handle = obj.getSelectionHandleContainingPoint(pt.x, pt.y)
            if handle != handle_NONE:
                return obj, handle

        return None, handle_NONE


    def _getObjectAt(self, pt):
        """ Return the first object found which is at the given point.
        """
        for obj in self.contents:
            if obj.objectContainsPoint(pt.x, pt.y):
                return obj
        return None


    def _drawObjectOutline(self, offsetX, offsetY):
        """ Draw an outline of the currently selected object.

            The selected object's outline is drawn at the object's position
            plus the given offset.

            Note that the outline is drawn by *inverting* the window's
            contents, so calling _drawObjectOutline twice in succession will
            restore the window's contents back to what they were previously.
        """
        if len(self.selection) != 1: return

        position = self.selection[0].getPosition()
        size     = self.selection[0].getSize()

        dc = wxClientDC(self.drawPanel)
        self.drawPanel.PrepareDC(dc)
        dc.BeginDrawing()
        dc.SetPen(wxBLACK_DASHED_PEN)
        dc.SetBrush(wxTRANSPARENT_BRUSH)
        dc.SetLogicalFunction(wxINVERT)

        dc.DrawRectangle(position.x + offsetX, position.y + offsetY,
                         size.width, size.height)

        dc.EndDrawing()


    def _drawVisualFeedback(self, startPt, endPt, type, dashedLine):
        """ Draw visual feedback for a drawing operation.

            The visual feedback consists of a line, ellipse, or rectangle based
            around the two given points.  'type' should be one of the following
            predefined feedback type constants:

                feedback_RECT     ->  draw rectangular feedback.
                feedback_LINE     ->  draw line feedback.
                feedback_ELLIPSE  ->  draw elliptical feedback.

            if 'dashedLine' is True, the feedback is drawn as a dashed rather
            than a solid line.

            Note that the feedback is drawn by *inverting* the window's
            contents, so calling _drawVisualFeedback twice in succession will
            restore the window's contents back to what they were previously.
        """
        dc = wxClientDC(self.drawPanel)
        self.drawPanel.PrepareDC(dc)
        dc.BeginDrawing()
        if dashedLine:
            dc.SetPen(wxBLACK_DASHED_PEN)
        else:
            dc.SetPen(wxBLACK_PEN)
        dc.SetBrush(wxTRANSPARENT_BRUSH)
        dc.SetLogicalFunction(wxINVERT)

        if type == feedback_RECT:
            dc.DrawRectangle(startPt.x, startPt.y,
                             endPt.x - startPt.x,
                             endPt.y - startPt.y)
        elif type == feedback_LINE:
            dc.DrawLine(startPt.x, startPt.y, endPt.x, endPt.y)
        elif type == feedback_ELLIPSE:
            dc.DrawEllipse(startPt.x, startPt.y,
                           endPt.x - startPt.x,
                           endPt.y - startPt.y)

        dc.EndDrawing()

#----------------------------------------------------------------------------

class DrawingObject:
    """ An object within the drawing panel.

        A pySketch document consists of a front-to-back ordered list of
        DrawingObjects.  Each DrawingObject has the following properties:

            'type'          What type of object this is (text, line, etc).
            'position'      The position of the object within the document.
            'size'          The size of the object within the document.
            'penColour'     The colour to use for drawing the object's outline.
            'fillColour'    Colour to use for drawing object's interior.
            'lineSize'      Line width (in pixels) to use for object's outline.
            'startPt'       The point, relative to the object's position, where
                            an obj_LINE object's line should start.
            'endPt'         The point, relative to the object's position, where
                            an obj_LINE object's line should end.
            'text'          The object's text (obj_TEXT objects only).
            'textFont'      The text object's font name.
            'textSize'      The text object's point size.
            'textBoldface'  If True, this text object will be drawn in
                            boldface.
            'textItalic'    If True, this text object will be drawn in italic.
            'textUnderline' If True, this text object will be drawn underlined.
            """

    # ==================
    # == Constructors ==
    # ==================

    def __init__(self, type, position=wxPoint(0, 0), size=wxSize(0, 0),
                 penColour=wxBLACK, fillColour=wxWHITE, lineSize=1,
                 text=None, startPt=wxPoint(0, 0), endPt=wxPoint(0,0)):
        """ Standard constructor.

            'type' is the type of object being created.  This should be one of
            the following constants:

                obj_LINE
                obj_RECT
                obj_ELLIPSE
                obj_TEXT

            The remaining parameters let you set various options for the newly
            created DrawingObject.
        """
        self.type              = type
        self.position          = position
        self.size              = size
        self.penColour         = penColour
        self.fillColour        = fillColour
        self.lineSize          = lineSize
        self.startPt           = startPt
        self.endPt             = endPt
        self.text              = text
        self.textFont          = wxSystemSettings_GetSystemFont(
                                    wxSYS_DEFAULT_GUI_FONT).GetFaceName()
        self.textSize          = 12
        self.textBoldface      = False
        self.textItalic        = False
        self.textUnderline     = False

    # =============================
    # == Object Property Methods ==
    # =============================

    def getData(self):
        """ Return a copy of the object's internal data.

            This is used to save this DrawingObject to disk.
        """
        return [self.type, self.position.x, self.position.y,
                self.size.width, self.size.height,
                self.penColour.Red(),
                self.penColour.Green(),
                self.penColour.Blue(),
                self.fillColour.Red(),
                self.fillColour.Green(),
                self.fillColour.Blue(),
                self.lineSize,
                self.startPt.x, self.startPt.y,
                self.endPt.x, self.endPt.y,
                self.text,
                self.textFont,
                self.textSize,
                self.textBoldface,
                self.textItalic,
                self.textUnderline]


    def setData(self, data):
        """ Set the object's internal data.

            'data' is a copy of the object's saved data, as returned by
            getData() above.  This is used to restore a previously saved
            DrawingObject.
        """
        #data = copy.deepcopy(data) # Needed?

        self.type              = data[0]
        self.position          = wxPoint(data[1], data[2])
        self.size              = wxSize(data[3], data[4])
        self.penColour         = wxColour(red=data[5],
                                          green=data[6],
                                          blue=data[7])
        self.fillColour        = wxColour(red=data[8],
                                          green=data[9],
                                          blue=data[10])
        self.lineSize          = data[11]
        self.startPt           = wxPoint(data[12], data[13])
        self.endPt             = wxPoint(data[14], data[15])
        self.text              = data[16]
        self.textFont          = data[17]
        self.textSize          = data[18]
        self.textBoldface      = data[19]
        self.textItalic        = data[20]
        self.textUnderline     = data[21]


    def getType(self):
        """ Return this DrawingObject's type.
        """
        return self.type


    def setPosition(self, position):
        """ Set the origin (top-left corner) for this DrawingObject.
        """
        self.position = position


    def getPosition(self):
        """ Return this DrawingObject's position.
        """
        return self.position


    def setSize(self, size):
        """ Set the size for this DrawingObject.
        """
        self.size = size


    def getSize(self):
        """ Return this DrawingObject's size.
        """
        return self.size


    def setPenColour(self, colour):
        """ Set the pen colour used for this DrawingObject.
        """
        self.penColour = colour


    def getPenColour(self):
        """ Return this DrawingObject's pen colour.
        """
        return self.penColour


    def setFillColour(self, colour):
        """ Set the fill colour used for this DrawingObject.
        """
        self.fillColour = colour


    def getFillColour(self):
        """ Return this DrawingObject's fill colour.
        """
        return self.fillColour


    def setLineSize(self, lineSize):
        """ Set the linesize used for this DrawingObject.
        """
        self.lineSize = lineSize


    def getLineSize(self):
        """ Return this DrawingObject's line size.
        """
        return self.lineSize


    def setStartPt(self, startPt):
        """ Set the starting point for this line DrawingObject.
        """
        self.startPt = startPt


    def getStartPt(self):
        """ Return the starting point for this line DrawingObject.
        """
        return self.startPt


    def setEndPt(self, endPt):
        """ Set the ending point for this line DrawingObject.
        """
        self.endPt = endPt


    def getEndPt(self):
        """ Return the ending point for this line DrawingObject.
        """
        return self.endPt


    def setText(self, text):
        """ Set the text for this DrawingObject.
        """
        self.text = text


    def getText(self):
        """ Return this DrawingObject's text.
        """
        return self.text


    def setTextFont(self, font):
        """ Set the typeface for this text DrawingObject.
        """
        self.textFont = font


    def getTextFont(self):
        """ Return this text DrawingObject's typeface.
        """
        return self.textFont


    def setTextSize(self, size):
        """ Set the point size for this text DrawingObject.
        """
        self.textSize = size


    def getTextSize(self):
        """ Return this text DrawingObject's text size.
        """
        return self.textSize


    def setTextBoldface(self, boldface):
        """ Set the boldface flag for this text DrawingObject.
        """
        self.textBoldface = boldface


    def getTextBoldface(self):
        """ Return this text DrawingObject's boldface flag.
        """
        return self.textBoldface


    def setTextItalic(self, italic):
        """ Set the italic flag for this text DrawingObject.
        """
        self.textItalic = italic


    def getTextItalic(self):
        """ Return this text DrawingObject's italic flag.
        """
        return self.textItalic


    def setTextUnderline(self, underline):
        """ Set the underling flag for this text DrawingObject.
        """
        self.textUnderline = underline


    def getTextUnderline(self):
        """ Return this text DrawingObject's underline flag.
        """
        return self.textUnderline

    # ============================
    # == Object Drawing Methods ==
    # ============================

    def draw(self, dc, selected):
        """ Draw this DrawingObject into our window.

            'dc' is the device context to use for drawing.  If 'selected' is
            True, the object is currently selected and should be drawn as such.
        """
        if self.type != obj_TEXT:
            if self.lineSize == 0:
                dc.SetPen(wxPen(self.penColour, self.lineSize, wxTRANSPARENT))
            else:
                dc.SetPen(wxPen(self.penColour, self.lineSize, wxSOLID))
            dc.SetBrush(wxBrush(self.fillColour, wxSOLID))
        else:
            dc.SetTextForeground(self.penColour)
            dc.SetTextBackground(self.fillColour)

        self._privateDraw(dc, self.position, selected)

    # =======================
    # == Selection Methods ==
    # =======================

    def objectContainsPoint(self, x, y):
        """ Returns True iff this object contains the given point.

            This is used to determine if the user clicked on the object.
        """
        # Firstly, ignore any points outside of the object's bounds.

        if x < self.position.x: return False
        if x > self.position.x + self.size.x: return False
        if y < self.position.y: return False
        if y > self.position.y + self.size.y: return False

        if self.type in [obj_RECT, obj_TEXT]:
            # Rectangles and text are easy -- they're always selected if the
            # point is within their bounds.
            return True

        # Now things get tricky.  There's no straightforward way of knowing
        # whether the point is within the object's bounds...to get around this,
        # we draw the object into a memory-based bitmap and see if the given
        # point was drawn.  This could no doubt be done more efficiently by
        # some tricky maths, but this approach works and is simple enough.

        bitmap = wxEmptyBitmap(self.size.x + 10, self.size.y + 10)
        dc = wxMemoryDC()
        dc.SelectObject(bitmap)
        dc.BeginDrawing()
        dc.SetBackground(wxWHITE_BRUSH)
        dc.Clear()
        dc.SetPen(wxPen(wxBLACK, self.lineSize + 5, wxSOLID))
        dc.SetBrush(wxBLACK_BRUSH)
        self._privateDraw(dc, wxPoint(5, 5), True)
        dc.EndDrawing()
        pixel = dc.GetPixel(x - self.position.x + 5, y - self.position.y + 5)
        if (pixel.Red() == 0) and (pixel.Green() == 0) and (pixel.Blue() == 0):
            return True
        else:
            return False


    def getSelectionHandleContainingPoint(self, x, y):
        """ Return the selection handle containing the given point, if any.

            We return one of the predefined selection handle ID codes.
        """
        if self.type == obj_LINE:
            # We have selection handles at the start and end points.
            if self._pointInSelRect(x, y, self.position.x + self.startPt.x,
                                          self.position.y + self.startPt.y):
                return handle_START_POINT
            elif self._pointInSelRect(x, y, self.position.x + self.endPt.x,
                                            self.position.y + self.endPt.y):
                return handle_END_POINT
            else:
                return handle_NONE
        else:
            # We have selection handles at all four corners.
            if self._pointInSelRect(x, y, self.position.x, self.position.y):
                return handle_TOP_LEFT
            elif self._pointInSelRect(x, y, self.position.x + self.size.width,
                                            self.position.y):
                return handle_TOP_RIGHT
            elif self._pointInSelRect(x, y, self.position.x,
                                            self.position.y + self.size.height):
                return handle_BOTTOM_LEFT
            elif self._pointInSelRect(x, y, self.position.x + self.size.width,
                                            self.position.y + self.size.height):
                return handle_BOTTOM_RIGHT
            else:
                return handle_NONE


    def objectWithinRect(self, x, y, width, height):
        """ Return True iff this object falls completely within the given rect.
        """
        if x          > self.position.x:                    return False
        if x + width  < self.position.x + self.size.width:  return False
        if y          > self.position.y:                    return False
        if y + height < self.position.y + self.size.height: return False
        return True

    # =====================
    # == Utility Methods ==
    # =====================

    def fitToText(self):
        """ Resize a text DrawingObject so that it fits it's text exactly.
        """
        if self.type != obj_TEXT: return

        if self.textBoldface: weight = wxBOLD
        else:                 weight = wxNORMAL
        if self.textItalic: style = wxITALIC
        else:               style = wxNORMAL
        font = wxFont(self.textSize, wxDEFAULT, style, weight,
                      self.textUnderline, self.textFont)

        dummyWindow = wxFrame(None, -1, "")
        dummyWindow.SetFont(font)
        width, height = dummyWindow.GetTextExtent(self.text)
        dummyWindow.Destroy()

        self.size = wxSize(width, height)

    # =====================
    # == Private Methods ==
    # =====================

    def _privateDraw(self, dc, position, selected):
        """ Private routine to draw this DrawingObject.

            'dc' is the device context to use for drawing, while 'position' is
            the position in which to draw the object.  If 'selected' is True,
            the object is drawn with selection handles.  This private drawing
            routine assumes that the pen and brush have already been set by the
            caller.
        """
        if self.type == obj_LINE:
            dc.DrawLine(position.x + self.startPt.x,
                        position.y + self.startPt.y,
                        position.x + self.endPt.x,
                        position.y + self.endPt.y)
        elif self.type == obj_RECT:
            dc.DrawRectangle(position.x, position.y,
                             self.size.width, self.size.height)
        elif self.type == obj_ELLIPSE:
            dc.DrawEllipse(position.x, position.y,
                           self.size.width, self.size.height)
        elif self.type == obj_TEXT:
            if self.textBoldface: weight = wxBOLD
            else:                 weight = wxNORMAL
            if self.textItalic: style = wxITALIC
            else:               style = wxNORMAL
            font = wxFont(self.textSize, wxDEFAULT, style, weight,
                          self.textUnderline, self.textFont)
            dc.SetFont(font)
            dc.DrawText(self.text, position.x, position.y)

        if selected:
            dc.SetPen(wxTRANSPARENT_PEN)
            dc.SetBrush(wxBLACK_BRUSH)

            if self.type == obj_LINE:
                # Draw selection handles at the start and end points.
                self._drawSelHandle(dc, position.x + self.startPt.x,
                                        position.y + self.startPt.y)
                self._drawSelHandle(dc, position.x + self.endPt.x,
                                        position.y + self.endPt.y)
            else:
                # Draw selection handles at all four corners.
                self._drawSelHandle(dc, position.x, position.y)
                self._drawSelHandle(dc, position.x + self.size.width,
                                        position.y)
                self._drawSelHandle(dc, position.x,
                                        position.y + self.size.height)
                self._drawSelHandle(dc, position.x + self.size.width,
                                        position.y + self.size.height)


    def _drawSelHandle(self, dc, x, y):
        """ Draw a selection handle around this DrawingObject.

            'dc' is the device context to draw the selection handle within,
            while 'x' and 'y' are the coordinates to use for the centre of the
            selection handle.
        """
        dc.DrawRectangle(x - 3, y - 3, 6, 6)


    def _pointInSelRect(self, x, y, rX, rY):
        """ Return True iff (x, y) is within the selection handle at (rX, ry).
        """
        if   x < rX - 3: return False
        elif x > rX + 3: return False
        elif y < rY - 3: return False
        elif y > rY + 3: return False
        else:            return True

#----------------------------------------------------------------------------

class ToolPaletteIcon(wxStaticBitmap):
    """ An icon appearing in the tool palette area of our sketching window.

        Note that this is actually implemented as a wxStaticBitmap rather
        than as a wxIcon.  wxIcon has a very specific meaning, and isn't
        appropriate for this more general use.
    """

    def __init__(self, parent, iconID, iconName, toolTip):
        """ Standard constructor.

            'parent'   is the parent window this icon will be part of.
            'iconID'   is the internal ID used for this icon.
            'iconName' is the name used for this icon.
            'toolTip'  is the tool tip text to show for this icon.

            The icon name is used to get the appropriate bitmap for this icon.
        """
        bmp = wxBitmap("images/" + iconName + "Icon.bmp", wxBITMAP_TYPE_BMP)
        wxStaticBitmap.__init__(self, parent, iconID, bmp, wxDefaultPosition,
                                wxSize(bmp.GetWidth(), bmp.GetHeight()))
        self.SetToolTip(wxToolTip(toolTip))

        self.iconID     = iconID
        self.iconName   = iconName
        self.isSelected = False


    def select(self):
        """ Select the icon.

            The icon's visual representation is updated appropriately.
        """
        if self.isSelected: return # Nothing to do!

        bmp = wxBitmap("images/" + self.iconName + "IconSel.bmp",
                       wxBITMAP_TYPE_BMP)
        self.SetBitmap(bmp)
        self.isSelected = True


    def deselect(self):
        """ Deselect the icon.

            The icon's visual representation is updated appropriately.
        """
        if not self.isSelected: return # Nothing to do!

        bmp = wxBitmap("images/" + self.iconName + "Icon.bmp",
                       wxBITMAP_TYPE_BMP)
        self.SetBitmap(bmp)
        self.isSelected = False

#----------------------------------------------------------------------------

class ToolOptionIndicator(wxWindow):
    """ A visual indicator which shows the current tool options.
    """
    def __init__(self, parent):
        """ Standard constructor.
        """
        wxWindow.__init__(self, parent, -1, wxDefaultPosition, wxSize(52, 32))

        self.penColour  = wxBLACK
        self.fillColour = wxWHITE
        self.lineSize   = 1

        EVT_PAINT(self, self.OnPaint)


    def setPenColour(self, penColour):
        """ Set the indicator's current pen colour.
        """
        self.penColour = penColour
        self.Refresh()


    def setFillColour(self, fillColour):
        """ Set the indicator's current fill colour.
        """
        self.fillColour = fillColour
        self.Refresh()


    def setLineSize(self, lineSize):
        """ Set the indicator's current pen colour.
        """
        self.lineSize = lineSize
        self.Refresh()


    def OnPaint(self, event):
        """ Paint our tool option indicator.
        """
        dc = wxPaintDC(self)
        dc.BeginDrawing()

        if self.lineSize == 0:
            dc.SetPen(wxPen(self.penColour, self.lineSize, wxTRANSPARENT))
        else:
            dc.SetPen(wxPen(self.penColour, self.lineSize, wxSOLID))
        dc.SetBrush(wxBrush(self.fillColour, wxSOLID))

        dc.DrawRectangle(5, 5, self.GetSize().width - 10,
                               self.GetSize().height - 10)

        dc.EndDrawing()

#----------------------------------------------------------------------------

class EditTextObjectDialog(wxDialog):
    """ Dialog box used to edit the properties of a text object.

        The user can edit the object's text, font, size, and text style.
    """

    def __init__(self, parent, title):
        """ Standard constructor.
        """
        wxDialog.__init__(self, parent, -1, title)

        self.textCtrl = wxTextCtrl(self, 1001, "", style=wxTE_PROCESS_ENTER,
                                   validator=TextObjectValidator())
        extent = self.textCtrl.GetFullTextExtent("Hy")
        lineHeight = extent[1] + extent[3]
        self.textCtrl.SetSize(wxSize(-1, lineHeight * 4))

        EVT_TEXT_ENTER(self, 1001, self._doEnter)

        fonts = wxFontEnumerator()
        fonts.EnumerateFacenames()
        self.fontList = fonts.GetFacenames()
        self.fontList.sort()

        fontLabel = wxStaticText(self, -1, "Font:")
        self._setFontOptions(fontLabel, weight=wxBOLD)

        self.fontCombo = wxComboBox(self, -1, "", wxDefaultPosition,
                                    wxDefaultSize, self.fontList,
                                    style = wxCB_READONLY)
        self.fontCombo.SetSelection(0) # Default to first available font.

        self.sizeList = ["8", "9", "10", "12", "14", "16",
                         "18", "20", "24", "32", "48", "72"]

        sizeLabel = wxStaticText(self, -1, "Size:")
        self._setFontOptions(sizeLabel, weight=wxBOLD)

        self.sizeCombo = wxComboBox(self, -1, "", wxDefaultPosition,
                                    wxDefaultSize, self.sizeList,
                                    style=wxCB_READONLY)
        self.sizeCombo.SetSelection(3) # Default to 12 point text.

        gap = wxLEFT | wxTOP | wxRIGHT

        comboSizer = wxBoxSizer(wxHORIZONTAL)
        comboSizer.Add(fontLabel,      0, gap | wxALIGN_CENTRE_VERTICAL, 5)
        comboSizer.Add(self.fontCombo, 0, gap, 5)
        comboSizer.Add(5, 5) # Spacer.
        comboSizer.Add(sizeLabel,      0, gap | wxALIGN_CENTRE_VERTICAL, 5)
        comboSizer.Add(self.sizeCombo, 0, gap, 5)

        self.boldCheckbox      = wxCheckBox(self, -1, "Bold")
        self.italicCheckbox    = wxCheckBox(self, -1, "Italic")
        self.underlineCheckbox = wxCheckBox(self, -1, "Underline")

        self._setFontOptions(self.boldCheckbox,      weight=wxBOLD)
        self._setFontOptions(self.italicCheckbox,    style=wxITALIC)
        self._setFontOptions(self.underlineCheckbox, underline=True)

        styleSizer = wxBoxSizer(wxHORIZONTAL)
        styleSizer.Add(self.boldCheckbox,      0, gap, 5)
        styleSizer.Add(self.italicCheckbox,    0, gap, 5)
        styleSizer.Add(self.underlineCheckbox, 0, gap, 5)

        self.okButton     = wxButton(self, wxID_OK,     "OK")
        self.cancelButton = wxButton(self, wxID_CANCEL, "Cancel")

        btnSizer = wxBoxSizer(wxHORIZONTAL)
        btnSizer.Add(self.okButton,     0, gap, 5)
        btnSizer.Add(self.cancelButton, 0, gap, 5)

        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(self.textCtrl, 1, gap | wxEXPAND,       5)
        sizer.Add(10, 10) # Spacer.
        sizer.Add(comboSizer,    0, gap | wxALIGN_CENTRE, 5)
        sizer.Add(styleSizer,    0, gap | wxALIGN_CENTRE, 5)
        sizer.Add(10, 10) # Spacer.
        sizer.Add(btnSizer,      0, gap | wxALIGN_CENTRE, 5)

        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)

        self.textCtrl.SetFocus()


    def objectToDialog(self, obj):
        """ Copy the properties of the given text object into the dialog box.
        """
        self.textCtrl.SetValue(obj.getText())
        self.textCtrl.SetSelection(0, len(obj.getText()))

        for i in range(len(self.fontList)):
            if self.fontList[i] == obj.getTextFont():
                self.fontCombo.SetSelection(i)
                break

        for i in range(len(self.sizeList)):
            if self.sizeList[i] == str(obj.getTextSize()):
                self.sizeCombo.SetSelection(i)
                break

        self.boldCheckbox.SetValue(obj.getTextBoldface())
        self.italicCheckbox.SetValue(obj.getTextItalic())
        self.underlineCheckbox.SetValue(obj.getTextUnderline())


    def dialogToObject(self, obj):
        """ Copy the properties from the dialog box into the given text object.
        """
        obj.setText(self.textCtrl.GetValue())
        obj.setTextFont(self.fontCombo.GetValue())
        obj.setTextSize(int(self.sizeCombo.GetValue()))
        obj.setTextBoldface(self.boldCheckbox.GetValue())
        obj.setTextItalic(self.italicCheckbox.GetValue())
        obj.setTextUnderline(self.underlineCheckbox.GetValue())
        obj.fitToText()

    # ======================
    # == Private Routines ==
    # ======================

    def _setFontOptions(self, ctrl, family=None, pointSize=-1,
                                    style=wxNORMAL, weight=wxNORMAL,
                                    underline=False):
        """ Change the font settings for the given control.

            The meaning of the 'family', 'pointSize', 'style', 'weight' and
            'underline' parameters are the same as for the wxFont constructor.
            If the family and/or pointSize isn't specified, the current default
            value is used.
        """
        if family == None: family = ctrl.GetFont().GetFamily()
        if pointSize == -1: pointSize = ctrl.GetFont().GetPointSize()

        ctrl.SetFont(wxFont(pointSize, family, style, weight, underline))
        ctrl.SetSize(ctrl.GetBestSize()) # Adjust size to reflect font change.


    def _doEnter(self, event):
        """ Respond to the user hitting the ENTER key.

            We simulate clicking on the "OK" button.
        """
        if self.Validate(): self.Show(False)

#----------------------------------------------------------------------------

class TextObjectValidator(wxPyValidator):
    """ This validator is used to ensure that the user has entered something
        into the text object editor dialog's text field.
    """
    def __init__(self):
        """ Standard constructor.
        """
        wxPyValidator.__init__(self)


    def Clone(self):
        """ Standard cloner.

            Note that every validator must implement the Clone() method.
        """
        return TextObjectValidator()


    def Validate(self, win):
        """ Validate the contents of the given text control.
        """
        textCtrl = wxPyTypeCast(self.GetWindow(), "wxTextCtrl")
        text = textCtrl.GetValue()

        if len(text) == 0:
            wxMessageBox("A text object must contain some text!", "Error")
            return False
        else:
            return True


    def TransferToWindow(self):
        """ Transfer data from validator to window.

            The default implementation returns False, indicating that an error
            occurred.  We simply return True, as we don't do any data transfer.
        """
        return True # Prevent wxDialog from complaining.


    def TransferFromWindow(self):
        """ Transfer data from window to validator.

            The default implementation returns False, indicating that an error
            occurred.  We simply return True, as we don't do any data transfer.
        """
        return True # Prevent wxDialog from complaining.

#----------------------------------------------------------------------------

class ExceptionHandler:
    """ A simple error-handling class to write exceptions to a text file.

        Under MS Windows, the standard DOS console window doesn't scroll and
        closes as soon as the application exits, making it hard to find and
        view Python exceptions.  This utility class allows you to handle Python
        exceptions in a more friendly manner.
    """

    def __init__(self):
        """ Standard constructor.
        """
        self._buff = ""
        if os.path.exists("errors.txt"):
            os.remove("errors.txt") # Delete previous error log, if any.


    def write(self, s):
        """ Write the given error message to a text file.

            Note that if the error message doesn't end in a carriage return, we
            have to buffer up the inputs until a carriage return is received.
        """
        if (s[-1] != "\n") and (s[-1] != "\r"):
            self._buff = self._buff + s
            return

        try:
            s = self._buff + s
            self._buff = ""

            if s[:9] == "Traceback":
                # Tell the user than an exception occurred.
                wxMessageBox("An internal error has occurred.\nPlease " + \
                             "refer to the 'errors.txt' file for details.",
                             "Error", wxOK | wxCENTRE | wxICON_EXCLAMATION)

            f = open("errors.txt", "a")
            f.write(s)
            f.close()
        except:
            pass # Don't recursively crash on errors.

#----------------------------------------------------------------------------

class SketchApp(wxApp):
    """ The main pySketch application object.
    """
    def OnInit(self):
        """ Initialise the application.
        """
        wxInitAllImageHandlers()

        global _docList
        _docList = []

        if len(sys.argv) == 1:
            # No file name was specified on the command line -> start with a
            # blank document.
            frame = DrawingFrame(None, -1, "Untitled")
            frame.Centre()
            frame.Show(True)
            _docList.append(frame)
        else:
            # Load the file(s) specified on the command line.
            for arg in sys.argv[1:]:
                fileName = os.path.join(os.getcwd(), arg)
                if os.path.isfile(fileName):
                    frame = DrawingFrame(None, -1,
                                         os.path.basename(fileName),
                                         fileName=fileName)
                    frame.Show(True)
                    _docList.append(frame)

        return True

#----------------------------------------------------------------------------

def main():
    """ Start up the pySketch application.
    """
    global _app

    # Redirect python exceptions to a log file.

    sys.stderr = ExceptionHandler()

    # Create and start the pySketch application.

    _app = SketchApp(0)
    _app.MainLoop()


if __name__ == "__main__":
    main()

