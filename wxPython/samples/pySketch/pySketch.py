""" pySketch

    A simple object-oriented drawing program.

    This is completely free software; please feel free to adapt or use this in
    any way you like.

    Original Author: Erik Westra (ewestra@wave.co.nz)

    Other contributors: Bill Baxter (wbaxter@gmail.com)

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

      * Saving files via pickling is not a robust cross-platform solution.
"""
import sys
import cPickle, os.path
import copy
import wx
from wx.lib.buttons import GenBitmapButton,GenBitmapToggleButton


import traceback, types

#----------------------------------------------------------------------------
#                            System Constants
#----------------------------------------------------------------------------

# Our menu item IDs:

menu_DUPLICATE     = wx.NewId() # Edit menu items.
menu_EDIT_PROPS    = wx.NewId()

menu_SELECT        = wx.NewId() # Tools menu items.
menu_LINE          = wx.NewId()
menu_POLYGON       = wx.NewId()
menu_RECT          = wx.NewId()
menu_ELLIPSE       = wx.NewId()
menu_TEXT          = wx.NewId()

menu_DC            = wx.NewId() # View menu items.
menu_GCDC          = wx.NewId()

menu_MOVE_FORWARD  = wx.NewId() # Object menu items.
menu_MOVE_TO_FRONT = wx.NewId()
menu_MOVE_BACKWARD = wx.NewId()
menu_MOVE_TO_BACK  = wx.NewId()

menu_ABOUT         = wx.NewId() # Help menu items.

# Our tool IDs:

id_SELECT   = wx.NewId()
id_LINE     = wx.NewId()
id_POLYGON  = wx.NewId()
id_SCRIBBLE = wx.NewId()
id_RECT     = wx.NewId()
id_ELLIPSE  = wx.NewId()
id_TEXT     = wx.NewId()

# Our tool option IDs:

id_FILL_OPT   = wx.NewId()
id_PEN_OPT    = wx.NewId()
id_LINE_OPT   = wx.NewId()

id_LINESIZE_0 = wx.NewId()
id_LINESIZE_1 = wx.NewId()
id_LINESIZE_2 = wx.NewId()
id_LINESIZE_3 = wx.NewId()
id_LINESIZE_4 = wx.NewId()
id_LINESIZE_5 = wx.NewId()

# Size of the drawing page, in pixels.

PAGE_WIDTH  = 1000
PAGE_HEIGHT = 1000

#----------------------------------------------------------------------------

class DrawingFrame(wx.Frame):
    """ A frame showing the contents of a single document. """

    # ==========================================
    # == Initialisation and Window Management ==
    # ==========================================

    def __init__(self, parent, id, title, fileName=None):
        """ Standard constructor.

            'parent', 'id' and 'title' are all passed to the standard wx.Frame
            constructor.  'fileName' is the name and path of a saved file to
            load into this frame, if any.
        """
        wx.Frame.__init__(self, parent, id, title,
                         style = wx.DEFAULT_FRAME_STYLE | wx.WANTS_CHARS |
                                 wx.NO_FULL_REPAINT_ON_RESIZE)

        # Setup our menu bar.
        menuBar = wx.MenuBar()

        self.fileMenu = wx.Menu()
        self.fileMenu.Append(wx.ID_NEW,    "New\tCtrl-N", "Create a new document")
        self.fileMenu.Append(wx.ID_OPEN,   "Open...\tCtrl-O", "Open an existing document")
        self.fileMenu.Append(wx.ID_CLOSE,  "Close\tCtrl-W")
        self.fileMenu.AppendSeparator()
        self.fileMenu.Append(wx.ID_SAVE,   "Save\tCtrl-S")
        self.fileMenu.Append(wx.ID_SAVEAS, "Save As...")
        self.fileMenu.Append(wx.ID_REVERT, "Revert...")
        self.fileMenu.AppendSeparator()
        self.fileMenu.Append(wx.ID_EXIT,   "Quit\tCtrl-Q")

        menuBar.Append(self.fileMenu, "File")

        self.editMenu = wx.Menu()
        self.editMenu.Append(wx.ID_UNDO,      "Undo\tCtrl-Z")
        self.editMenu.Append(wx.ID_REDO,      "Redo\tCtrl-Y")
        self.editMenu.AppendSeparator()
        self.editMenu.Append(wx.ID_SELECTALL, "Select All\tCtrl-A")
        self.editMenu.AppendSeparator()
        self.editMenu.Append(menu_DUPLICATE,  "Duplicate\tCtrl-D")
        self.editMenu.Append(menu_EDIT_PROPS,"Edit...\tCtrl-E", "Edit object properties")
        self.editMenu.Append(wx.ID_CLEAR,     "Delete\tDel")

        menuBar.Append(self.editMenu, "Edit")

        self.viewMenu = wx.Menu()
        self.viewMenu.Append(menu_DC,  "Normal quality", 
                             "Normal rendering using wx.DC",
                             kind=wx.ITEM_RADIO)
        self.viewMenu.Append(menu_GCDC,"High quality", 
                             "Anti-aliased rendering using wx.GCDC", 
                             kind=wx.ITEM_RADIO)

        menuBar.Append(self.viewMenu, "View")

        self.toolsMenu = wx.Menu()
        self.toolsMenu.Append(id_SELECT,  "Selection", kind=wx.ITEM_RADIO)
        self.toolsMenu.Append(id_LINE,    "Line",      kind=wx.ITEM_RADIO)
        self.toolsMenu.Append(id_POLYGON, "Polygon",   kind=wx.ITEM_RADIO)
        self.toolsMenu.Append(id_SCRIBBLE,"Scribble",  kind=wx.ITEM_RADIO)
        self.toolsMenu.Append(id_RECT,    "Rectangle", kind=wx.ITEM_RADIO)
        self.toolsMenu.Append(id_ELLIPSE, "Ellipse",   kind=wx.ITEM_RADIO)
        self.toolsMenu.Append(id_TEXT,    "Text",      kind=wx.ITEM_RADIO)

        menuBar.Append(self.toolsMenu, "Tools")

        self.objectMenu = wx.Menu()
        self.objectMenu.Append(menu_MOVE_FORWARD,  "Move Forward")
        self.objectMenu.Append(menu_MOVE_TO_FRONT, "Move to Front\tCtrl-F")
        self.objectMenu.Append(menu_MOVE_BACKWARD, "Move Backward")
        self.objectMenu.Append(menu_MOVE_TO_BACK,  "Move to Back\tCtrl-B")

        menuBar.Append(self.objectMenu, "Object")

        self.helpMenu = wx.Menu()
        self.helpMenu.Append(menu_ABOUT, "About pySketch...")

        menuBar.Append(self.helpMenu, "Help")

        self.SetMenuBar(menuBar)

        # Create our statusbar

        self.CreateStatusBar()

        # Create our toolbar.

        tsize = (15,15)
        self.toolbar = self.CreateToolBar(wx.TB_HORIZONTAL | wx.NO_BORDER | wx.TB_FLAT)

        artBmp = wx.ArtProvider.GetBitmap
        self.toolbar.AddSimpleTool(
            wx.ID_NEW, artBmp(wx.ART_NEW, wx.ART_TOOLBAR, tsize), "New")
        self.toolbar.AddSimpleTool(
            wx.ID_OPEN, artBmp(wx.ART_FILE_OPEN, wx.ART_TOOLBAR, tsize), "Open")
        self.toolbar.AddSimpleTool(
            wx.ID_SAVE, artBmp(wx.ART_FILE_SAVE, wx.ART_TOOLBAR, tsize), "Save")
        self.toolbar.AddSimpleTool(
            wx.ID_SAVEAS, artBmp(wx.ART_FILE_SAVE_AS, wx.ART_TOOLBAR, tsize),
            "Save As...")
        #-------
        self.toolbar.AddSeparator()
        self.toolbar.AddSimpleTool(
            wx.ID_UNDO, artBmp(wx.ART_UNDO, wx.ART_TOOLBAR, tsize), "Undo")
        self.toolbar.AddSimpleTool(
            wx.ID_REDO, artBmp(wx.ART_REDO, wx.ART_TOOLBAR, tsize), "Redo")
        self.toolbar.AddSeparator()
        self.toolbar.AddSimpleTool(
            menu_DUPLICATE, wx.Bitmap("images/duplicate.bmp", wx.BITMAP_TYPE_BMP),
            "Duplicate")
        #-------
        self.toolbar.AddSeparator()
        self.toolbar.AddSimpleTool(
            menu_MOVE_FORWARD, wx.Bitmap("images/moveForward.bmp", wx.BITMAP_TYPE_BMP),
            "Move Forward")
        self.toolbar.AddSimpleTool(
            menu_MOVE_BACKWARD, wx.Bitmap("images/moveBack.bmp", wx.BITMAP_TYPE_BMP),
            "Move Backward")

        self.toolbar.Realize()

        # Associate menu/toolbar items with their handlers.
        menuHandlers = [
        (wx.ID_NEW,    self.doNew),
        (wx.ID_OPEN,   self.doOpen),
        (wx.ID_CLOSE,  self.doClose),
        (wx.ID_SAVE,   self.doSave),
        (wx.ID_SAVEAS, self.doSaveAs),
        (wx.ID_REVERT, self.doRevert),
        (wx.ID_EXIT,   self.doExit),

        (wx.ID_UNDO,         self.doUndo),
        (wx.ID_REDO,         self.doRedo),
        (wx.ID_SELECTALL,    self.doSelectAll),
        (menu_DUPLICATE,     self.doDuplicate),
        (menu_EDIT_PROPS,    self.doEditObject),
        (wx.ID_CLEAR,        self.doDelete),

        (id_SELECT,  self.onChooseTool, self.updChooseTool),
        (id_LINE,    self.onChooseTool, self.updChooseTool),
        (id_POLYGON, self.onChooseTool, self.updChooseTool),
        (id_SCRIBBLE,self.onChooseTool, self.updChooseTool),
        (id_RECT,    self.onChooseTool, self.updChooseTool),
        (id_ELLIPSE, self.onChooseTool, self.updChooseTool),
        (id_TEXT,    self.onChooseTool, self.updChooseTool),

        (menu_DC,      self.doChooseQuality),
        (menu_GCDC,    self.doChooseQuality),

        (menu_MOVE_FORWARD,  self.doMoveForward),
        (menu_MOVE_TO_FRONT, self.doMoveToFront),
        (menu_MOVE_BACKWARD, self.doMoveBackward),
        (menu_MOVE_TO_BACK,  self.doMoveToBack),

        (menu_ABOUT, self.doShowAbout)]
        for combo in menuHandlers:
            id, handler = combo[:2]
            self.Bind(wx.EVT_MENU, handler, id = id)
            if len(combo)>2:
                self.Bind(wx.EVT_UPDATE_UI, combo[2], id = id)
                
        # Install our own method to handle closing the window.  This allows us
        # to ask the user if he/she wants to save before closing the window, as
        # well as keeping track of which windows are currently open.

        self.Bind(wx.EVT_CLOSE, self.doClose)

        # Install our own method for handling keystrokes.  We use this to let
        # the user move the selected object(s) around using the arrow keys.

        self.Bind(wx.EVT_CHAR_HOOK, self.onKeyEvent)

        # Setup our top-most panel.  This holds the entire contents of the
        # window, excluding the menu bar.

        self.topPanel = wx.Panel(self, -1, style=wx.SIMPLE_BORDER)

        # Setup our tool palette, with all our drawing tools and option icons.

        self.toolPalette = wx.BoxSizer(wx.VERTICAL)

        self.selectIcon  = ToolPaletteToggle(self.topPanel, id_SELECT,
                                           "select", "Selection Tool", mode=wx.ITEM_RADIO)
        self.lineIcon    = ToolPaletteToggle(self.topPanel, id_LINE,
                                           "line", "Line Tool", mode=wx.ITEM_RADIO)
        self.polygonIcon = ToolPaletteToggle(self.topPanel, id_POLYGON,
                                             "polygon", "Polygon Tool", mode=wx.ITEM_RADIO)
        self.scribbleIcon = ToolPaletteToggle(self.topPanel, id_SCRIBBLE,
                                             "scribble", "Scribble Tool", mode=wx.ITEM_RADIO)
        self.rectIcon    = ToolPaletteToggle(self.topPanel, id_RECT,
                                           "rect", "Rectangle Tool", mode=wx.ITEM_RADIO)
        self.ellipseIcon = ToolPaletteToggle(self.topPanel, id_ELLIPSE,
                                           "ellipse", "Ellipse Tool", mode=wx.ITEM_RADIO)
        self.textIcon    = ToolPaletteToggle(self.topPanel, id_TEXT,
                                             "text", "Text Tool", mode=wx.ITEM_RADIO)

        # Create the tools
        self.tools = {
            'select'  : (self.selectIcon,   SelectDrawingTool()),
            'line'    : (self.lineIcon,     LineDrawingTool()),
            'polygon' : (self.polygonIcon,  PolygonDrawingTool()),
            'scribble': (self.scribbleIcon, ScribbleDrawingTool()),
            'rect'    : (self.rectIcon,     RectDrawingTool()),
            'ellipse' : (self.ellipseIcon,  EllipseDrawingTool()),
            'text'    : (self.textIcon,     TextDrawingTool())
        }


        toolSizer = wx.GridSizer(0, 2, 5, 5)
        toolSizer.Add(self.selectIcon)
        toolSizer.Add(self.lineIcon)
        toolSizer.Add(self.rectIcon)
        toolSizer.Add(self.ellipseIcon)
        toolSizer.Add(self.polygonIcon)
        toolSizer.Add(self.scribbleIcon)
        toolSizer.Add(self.textIcon)

        self.optionIndicator = ToolOptionIndicator(self.topPanel)
        self.optionIndicator.SetToolTip(
                wx.ToolTip("Shows Current Pen/Fill/Line Size Settings"))

        optionSizer = wx.BoxSizer(wx.HORIZONTAL)

        self.penOptIcon  = ToolPaletteButton(self.topPanel, id_PEN_OPT,
                                             "penOpt", "Set Pen Colour",)
        self.fillOptIcon = ToolPaletteButton(self.topPanel, id_FILL_OPT,
                                             "fillOpt", "Set Fill Colour")
        self.lineOptIcon = ToolPaletteButton(self.topPanel, id_LINE_OPT,
                                             "lineOpt", "Set Line Size")

        margin = wx.LEFT | wx.RIGHT
        optionSizer.Add(self.penOptIcon,  0, margin, 1)
        optionSizer.Add(self.fillOptIcon, 0, margin, 1)
        optionSizer.Add(self.lineOptIcon, 0, margin, 1)

        margin = wx.TOP | wx.LEFT | wx.RIGHT | wx.ALIGN_CENTRE
        self.toolPalette.Add(toolSizer,            0, margin, 5)
        self.toolPalette.Add((0, 0),               0, margin, 5) # Spacer.
        self.toolPalette.Add(self.optionIndicator, 0, margin, 5)
        self.toolPalette.Add(optionSizer,          0, margin, 5)

        # Make the tool palette icons respond when the user clicks on them.

        for tool in self.tools.itervalues():
            tool[0].Bind(wx.EVT_BUTTON,    self.onChooseTool)

        self.selectIcon.Bind(wx.EVT_BUTTON, self.onChooseTool)
        self.lineIcon.Bind(wx.EVT_BUTTON, self.onChooseTool)


        self.penOptIcon.Bind(wx.EVT_BUTTON, self.onPenOptionIconClick)
        self.fillOptIcon.Bind(wx.EVT_BUTTON, self.onFillOptionIconClick)
        self.lineOptIcon.Bind(wx.EVT_BUTTON, self.onLineOptionIconClick)

        # Setup the main drawing area.

        self.drawPanel = wx.ScrolledWindow(self.topPanel, -1,
                                          style=wx.SUNKEN_BORDER|wx.NO_FULL_REPAINT_ON_RESIZE)
        self.drawPanel.SetBackgroundColour(wx.WHITE)

        self.drawPanel.EnableScrolling(True, True)
        self.drawPanel.SetScrollbars(20, 20, PAGE_WIDTH / 20, PAGE_HEIGHT / 20)

        self.drawPanel.Bind(wx.EVT_MOUSE_EVENTS, self.onMouseEvent)

        self.drawPanel.Bind(wx.EVT_IDLE, self.onIdle)
        self.drawPanel.Bind(wx.EVT_SIZE, self.onSize)
        self.drawPanel.Bind(wx.EVT_PAINT, self.onPaint)
        self.drawPanel.Bind(wx.EVT_ERASE_BACKGROUND, self.onEraseBackground)
        self.drawPanel.Bind(wx.EVT_SCROLLWIN, self.onPanelScroll)

        self.Bind(wx.EVT_TIMER, self.onIdle)


        # Position everything in the window.

        topSizer = wx.BoxSizer(wx.HORIZONTAL)
        topSizer.Add(self.toolPalette, 0)
        topSizer.Add(self.drawPanel, 1, wx.EXPAND)

        self.topPanel.SetAutoLayout(True)
        self.topPanel.SetSizer(topSizer)

        self.SetSizeHints(250, 200)
        self.SetSize(wx.Size(600, 400))

        # Select an initial tool.

        self.curToolName = None
        self.curToolIcon = None
        self.curTool = None
        self.setCurrentTool("select")

        # Set initial dc mode to fast
        self.wrapDC = lambda dc: dc

        # Setup our frame to hold the contents of a sketch document.

        self.dirty     = False
        self.fileName  = fileName
        self.contents  = []     # front-to-back ordered list of DrawingObjects.
        self.selection = []     # List of selected DrawingObjects.
        self.undoStack = []     # Stack of saved contents for undo.
        self.redoStack = []     # Stack of saved contents for redo.

        if self.fileName != None:
            self.loadContents()

        self._initBuffer()

        self._adjustMenus()

        # Finally, set our initial pen, fill and line options.

        self._setPenColour(wx.BLACK)
        self._setFillColour(wx.Colour(215,253,254))
        self._setLineSize(2)
        
        self.backgroundFillBrush = None # create on demand

        # Start the background redraw timer
        # This is optional, but it gives the double-buffered contents a 
        # chance to redraw even when idle events are disabled (like during 
        # resize and scrolling)
        self.redrawTimer = wx.Timer(self)
        self.redrawTimer.Start(700)


    # ============================
    # == Event Handling Methods ==
    # ============================


    def onPenOptionIconClick(self, event):
        """ Respond to the user clicking on the "Pen Options" icon.
        """
        data = wx.ColourData()
        if len(self.selection) == 1:
            data.SetColour(self.selection[0].getPenColour())
        else:
            data.SetColour(self.penColour)

        dialog = wx.ColourDialog(self, data)
        dialog.SetTitle('Choose line colour')
        if dialog.ShowModal() == wx.ID_OK:
            c = dialog.GetColourData().GetColour()
            self._setPenColour(wx.Colour(c.Red(), c.Green(), c.Blue()))
        dialog.Destroy()


    def onFillOptionIconClick(self, event):
        """ Respond to the user clicking on the "Fill Options" icon.
        """
        data = wx.ColourData()
        if len(self.selection) == 1:
            data.SetColour(self.selection[0].getFillColour())
        else:
            data.SetColour(self.fillColour)

        dialog = wx.ColourDialog(self, data)
        dialog.SetTitle('Choose fill colour')
        if dialog.ShowModal() == wx.ID_OK:
            c = dialog.GetColourData().GetColour()
            self._setFillColour(wx.Colour(c.Red(), c.Green(), c.Blue()))
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
        step = 1
        if event.ShiftDown():
            step = 20

        if event.GetKeyCode() == wx.WXK_UP:
            self._moveObject(0, -step)
        elif event.GetKeyCode() == wx.WXK_DOWN:
            self._moveObject(0, step)
        elif event.GetKeyCode() == wx.WXK_LEFT:
            self._moveObject(-step, 0)
        elif event.GetKeyCode() == wx.WXK_RIGHT:
            self._moveObject(step, 0)
        else:
            event.Skip()


    def onMouseEvent(self, event):
        """ Respond to mouse events in the main drawing panel

            How we respond depends on the currently selected tool.
        """
        if self.curTool is None: return

        # Translate event into canvas coordinates and pass to current tool
        origx,origy = event.X, event.Y
        pt = self._getEventCoordinates(event)
        event.m_x = pt.x
        event.m_y = pt.y
        handled = self.curTool.onMouseEvent(self,event)
        event.m_x = origx
        event.m_y = origy

        if handled: return

        # otherwise handle it ourselves
        if event.RightDown():
            self.doPopupContextMenu(event)
        

    def doPopupContextMenu(self, event):
        """ Respond to the user right-clicking within our drawing panel.

            We select the clicked-on item, if necessary, and display a pop-up
            menu of available options which can be applied to the selected
            item(s).
        """
        mousePt = self._getEventCoordinates(event)
        obj = self.getObjectAt(mousePt)

        if obj == None: return # Nothing selected.

        # Select the clicked-on object.

        self.select(obj)

        # Build our pop-up menu.

        menu = wx.Menu()
        menu.Append(menu_DUPLICATE, "Duplicate")
        menu.Append(menu_EDIT_PROPS,"Edit...")
        menu.Append(wx.ID_CLEAR,    "Delete")
        menu.AppendSeparator()
        menu.Append(menu_MOVE_FORWARD,   "Move Forward")
        menu.Append(menu_MOVE_TO_FRONT,  "Move to Front")
        menu.Append(menu_MOVE_BACKWARD,  "Move Backward")
        menu.Append(menu_MOVE_TO_BACK,   "Move to Back")

        menu.Enable(menu_EDIT_PROPS,    obj.hasPropertyEditor())
        menu.Enable(menu_MOVE_FORWARD,  obj != self.contents[0])
        menu.Enable(menu_MOVE_TO_FRONT, obj != self.contents[0])
        menu.Enable(menu_MOVE_BACKWARD, obj != self.contents[-1])
        menu.Enable(menu_MOVE_TO_BACK,  obj != self.contents[-1])

        self.Bind(wx.EVT_MENU, self.doDuplicate,   id=menu_DUPLICATE)
        self.Bind(wx.EVT_MENU, self.doEditObject,  id=menu_EDIT_PROPS)
        self.Bind(wx.EVT_MENU, self.doDelete,      id=wx.ID_CLEAR)
        self.Bind(wx.EVT_MENU, self.doMoveForward, id=menu_MOVE_FORWARD)
        self.Bind(wx.EVT_MENU, self.doMoveToFront, id=menu_MOVE_TO_FRONT)
        self.Bind(wx.EVT_MENU, self.doMoveBackward,id=menu_MOVE_BACKWARD)
        self.Bind(wx.EVT_MENU, self.doMoveToBack,  id=menu_MOVE_TO_BACK)  
                            
        # Show the pop-up menu.

        clickPt = wx.Point(mousePt.x + self.drawPanel.GetPosition().x,
                          mousePt.y + self.drawPanel.GetPosition().y)
        self.drawPanel.PopupMenu(menu, mousePt)
        menu.Destroy()


    def onSize(self, event):
        """
        Called when the window is resized.  We set a flag so the idle
        handler will resize the buffer.
        """
        self.requestRedraw()


    def onIdle(self, event):
        """
        If the size was changed then resize the bitmap used for double
        buffering to match the window size.  We do it in Idle time so
        there is only one refresh after resizing is done, not lots while
        it is happening.
        """
        if self._reInitBuffer and self.IsShown():
            self._initBuffer()
            self.drawPanel.Refresh(False)

    def requestRedraw(self):
        """Requests a redraw of the drawing panel contents.

        The actual redrawing doesn't happen until the next idle time.
        """
        self._reInitBuffer = True

    def onPaint(self, event):
        """
        Called when the window is exposed.
        """
        # Create a buffered paint DC.  It will create the real
        # wx.PaintDC and then blit the bitmap to it when dc is
        # deleted.
        dc = wx.BufferedPaintDC(self.drawPanel, self.buffer)


        # On Windows, if that's all we do things look a little rough
        # So in order to make scrolling more polished-looking
        # we iterate over the exposed regions and fill in unknown
        # areas with a fall-back pattern.

        if wx.Platform != '__WXMSW__':
            return

        # First get the update rects and subtract off the part that 
        # self.buffer has correct already
        region = self.drawPanel.GetUpdateRegion()
        panelRect = self.drawPanel.GetClientRect()
        offset = list(self.drawPanel.CalcUnscrolledPosition(0,0))
        offset[0] -= self.saved_offset[0]
        offset[1] -= self.saved_offset[1]
        region.Subtract(-offset[0],- offset[1],panelRect.Width, panelRect.Height)

        # Now iterate over the remaining region rects and fill in with a pattern
        rgn_iter = wx.RegionIterator(region)
        if rgn_iter.HaveRects():
            self.setBackgroundMissingFillStyle(dc)
            offset = self.drawPanel.CalcUnscrolledPosition(0,0)
        while rgn_iter:
            r = rgn_iter.GetRect()
            if r.Size != self.drawPanel.ClientSize:
                dc.DrawRectangleRect(r)
            rgn_iter.Next()
            

    def setBackgroundMissingFillStyle(self, dc):
        if self.backgroundFillBrush is None:
            # Win95 can only handle a 8x8 stipple bitmaps max
            #stippleBitmap = wx.BitmapFromBits("\xf0"*4 + "\x0f"*4,8,8)
            # ...but who uses Win95?
            stippleBitmap = wx.BitmapFromBits("\x06",2,2)
            stippleBitmap.SetMask(wx.Mask(stippleBitmap))
            bgbrush = wx.Brush(wx.WHITE, wx.STIPPLE_MASK_OPAQUE)
            bgbrush.SetStipple(stippleBitmap)
            self.backgroundFillBrush = bgbrush

        dc.SetPen(wx.TRANSPARENT_PEN)
        dc.SetBrush(self.backgroundFillBrush)
        dc.SetTextForeground(wx.LIGHT_GREY)
        dc.SetTextBackground(wx.WHITE)
            

    def onEraseBackground(self, event):
        """
        Overridden to do nothing to prevent flicker
        """
        pass


    def onPanelScroll(self, event):
        """
        Called when the user changes scrolls the drawPanel
        """
        # make a note to ourselves to redraw when we get a chance
        self.requestRedraw()
        event.Skip()
        pass

    def drawContents(self, dc):
        """
        Does the actual drawing of all drawing contents with the specified dc
        """
        # PrepareDC sets the device origin according to current scrolling
        self.drawPanel.PrepareDC(dc)

        gdc = self.wrapDC(dc)

        # First pass draws objects
        ordered_selection = []
        for obj in self.contents[::-1]:
            if obj in self.selection:
                obj.draw(gdc, True)
                ordered_selection.append(obj)
            else:
                obj.draw(gdc, False)

        # First pass draws objects
        if self.curTool is not None:
            self.curTool.draw(gdc)

        # Second pass draws selection handles so they're always on top
        for obj in ordered_selection:
            obj.drawHandles(gdc)



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
        fileName = wx.FileSelector("Open File", default_extension="psk",
                                  flags = wx.OPEN | wx.FILE_MUST_EXIST)
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
        fileName = wx.FileSelector("Save File As", "Saving",
                                  default_filename=default,
                                  default_extension="psk",
                                  wildcard="*.psk",
                                  flags = wx.SAVE | wx.OVERWRITE_PROMPT)
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

        if wx.MessageBox("Discard changes made to this document?", "Confirm",
                        style = wx.OK | wx.CANCEL | wx.ICON_QUESTION,
                        parent=self) == wx.CANCEL: return
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
        if not self.undoStack: return 

        state = self._buildStoredState()
        self.redoStack.append(state)
        state = self.undoStack.pop()
        self._restoreStoredState(state)

    def doRedo(self, event):
        """ Respond to the "Redo" menu.
        """
        if not self.redoStack: return

        state = self._buildStoredState()
        self.undoStack.append(state)
        state = self.redoStack.pop()
        self._restoreStoredState(state)

    def doSelectAll(self, event):
        """ Respond to the "Select All" menu command.
        """
        self.selectAll()


    def doDuplicate(self, event):
        """ Respond to the "Duplicate" menu command.
        """
        self.saveUndoInfo()

        objs = []
        for obj in self.contents:
            if obj in self.selection:
                newObj = copy.deepcopy(obj)
                pos = obj.getPosition()
                newObj.setPosition(wx.Point(pos.x + 10, pos.y + 10))
                objs.append(newObj)

        self.contents = objs + self.contents

        self.selectMany(objs)


    def doEditObject(self, event):
        """ Respond to the "Edit..." menu command.
        """
        if len(self.selection) != 1: return

        obj = self.selection[0]
        if not obj.hasPropertyEditor(): 
            assert False, "doEditObject called on non-editable"

        ret = obj.doPropertyEdit(self)
        if ret:
            self.dirty = True
            self.requestRedraw()
            self._adjustMenus()


    def doDelete(self, event):
        """ Respond to the "Delete" menu command.
        """
        self.saveUndoInfo()

        for obj in self.selection:
            self.contents.remove(obj)
            del obj
        self.deselectAll()


    def onChooseTool(self, event):
        """ Respond to tool selection menu and tool palette selections
        """
        obj = event.GetEventObject()
        id2name = { id_SELECT: "select",
                    id_LINE: "line",
                    id_POLYGON: "polygon",
                    id_SCRIBBLE: "scribble",
                    id_RECT: "rect",
                    id_ELLIPSE: "ellipse",
                    id_TEXT: "text" }
        toolID = event.GetId()
        name = id2name.get( toolID )
        
        if name:
            self.setCurrentTool(name)

    def updChooseTool(self, event):
        """UI update event that keeps tool menu in sync with the PaletteIcons"""
        obj = event.GetEventObject()
        id2name = { id_SELECT: "select",
                    id_LINE: "line",
                    id_POLYGON: "polygon",
                    id_SCRIBBLE: "scribble",
                    id_RECT: "rect",
                    id_ELLIPSE: "ellipse",
                    id_TEXT: "text" }
        toolID = event.GetId()
        event.Check( toolID == self.curToolIcon.GetId() )


    def doChooseQuality(self, event):
        """Respond to the render quality menu commands
        """
        if event.GetId() == menu_DC:
            self.wrapDC = lambda dc: dc
        else:
            self.wrapDC = lambda dc: wx.GCDC(dc)
        self._adjustMenus()
        self.requestRedraw()

    def doMoveForward(self, event):
        """ Respond to the "Move Forward" menu command.
        """
        if len(self.selection) != 1: return

        self.saveUndoInfo()

        obj = self.selection[0]
        index = self.contents.index(obj)
        if index == 0: return

        del self.contents[index]
        self.contents.insert(index-1, obj)

        self.requestRedraw()
        self._adjustMenus()


    def doMoveToFront(self, event):
        """ Respond to the "Move to Front" menu command.
        """
        if len(self.selection) != 1: return

        self.saveUndoInfo()

        obj = self.selection[0]
        self.contents.remove(obj)
        self.contents.insert(0, obj)

        self.requestRedraw()
        self._adjustMenus()


    def doMoveBackward(self, event):
        """ Respond to the "Move Backward" menu command.
        """
        if len(self.selection) != 1: return

        self.saveUndoInfo()

        obj = self.selection[0]
        index = self.contents.index(obj)
        if index == len(self.contents) - 1: return

        del self.contents[index]
        self.contents.insert(index+1, obj)

        self.requestRedraw()
        self._adjustMenus()


    def doMoveToBack(self, event):
        """ Respond to the "Move to Back" menu command.
        """
        if len(self.selection) != 1: return

        self.saveUndoInfo()

        obj = self.selection[0]
        self.contents.remove(obj)
        self.contents.append(obj)

        self.requestRedraw()
        self._adjustMenus()


    def doShowAbout(self, event):
        """ Respond to the "About pySketch" menu command.
        """
        dialog = wx.Dialog(self, -1, "About pySketch") # ,
                          #style=wx.DIALOG_MODAL | wx.STAY_ON_TOP)
        dialog.SetBackgroundColour(wx.WHITE)

        panel = wx.Panel(dialog, -1)
        panel.SetBackgroundColour(wx.WHITE)

        panelSizer = wx.BoxSizer(wx.VERTICAL)

        boldFont = wx.Font(panel.GetFont().GetPointSize(),
                          panel.GetFont().GetFamily(),
                          wx.NORMAL, wx.BOLD)

        logo = wx.StaticBitmap(panel, -1, wx.Bitmap("images/logo.bmp",
                                                  wx.BITMAP_TYPE_BMP))

        lab1 = wx.StaticText(panel, -1, "pySketch")
        lab1.SetFont(wx.Font(36, boldFont.GetFamily(), wx.ITALIC, wx.BOLD))
        lab1.SetSize(lab1.GetBestSize())

        imageSizer = wx.BoxSizer(wx.HORIZONTAL)
        imageSizer.Add(logo, 0, wx.ALL | wx.ALIGN_CENTRE_VERTICAL, 5)
        imageSizer.Add(lab1, 0, wx.ALL | wx.ALIGN_CENTRE_VERTICAL, 5)

        lab2 = wx.StaticText(panel, -1, "A simple object-oriented drawing " + \
                                       "program.")
        lab2.SetFont(boldFont)
        lab2.SetSize(lab2.GetBestSize())

        lab3 = wx.StaticText(panel, -1, "pySketch is completely free " + \
                                       "software; please")
        lab3.SetFont(boldFont)
        lab3.SetSize(lab3.GetBestSize())

        lab4 = wx.StaticText(panel, -1, "feel free to adapt or use this " + \
                                       "in any way you like.")
        lab4.SetFont(boldFont)
        lab4.SetSize(lab4.GetBestSize())

        lab5 = wx.StaticText(panel, -1,
                             "Author: Erik Westra " + \
                             "(ewestra@wave.co.nz)\n" + \
                             "Contributors: Bill Baxter " +\
                             "(wbaxter@gmail.com)  ")

        lab5.SetFont(boldFont)
        lab5.SetSize(lab5.GetBestSize())

        btnOK = wx.Button(panel, wx.ID_OK, "OK")

        panelSizer.Add(imageSizer, 0, wx.ALIGN_CENTRE)
        panelSizer.Add((10, 10)) # Spacer.
        panelSizer.Add(lab2, 0, wx.ALIGN_CENTRE)
        panelSizer.Add((10, 10)) # Spacer.
        panelSizer.Add(lab3, 0, wx.ALIGN_CENTRE)
        panelSizer.Add(lab4, 0, wx.ALIGN_CENTRE)
        panelSizer.Add((10, 10)) # Spacer.
        panelSizer.Add(lab5, 0, wx.ALIGN_CENTRE)
        panelSizer.Add((10, 10)) # Spacer.
        panelSizer.Add(btnOK, 0, wx.ALL | wx.ALIGN_CENTRE, 5)

        panel.SetAutoLayout(True)
        panel.SetSizer(panelSizer)
        panelSizer.Fit(panel)

        topSizer = wx.BoxSizer(wx.HORIZONTAL)
        topSizer.Add(panel, 0, wx.ALL, 10)

        dialog.SetAutoLayout(True)
        dialog.SetSizer(topSizer)
        topSizer.Fit(dialog)

        dialog.Centre()

        btn = dialog.ShowModal()
        dialog.Destroy()

    def getTextEditor(self):
        if not hasattr(self,'textEditor') or not self.textEditor:
            self.textEditor = EditTextObjectDialog(self, "Edit Text Object")
        return self.textEditor

    # =============================
    # == Object Creation Methods ==
    # =============================

    def addObject(self, obj, select=True):
        """Add a new drawing object to the canvas.
        
        If select is True then also select the object
        """
        self.saveUndoInfo()
        self.contents.insert(0, obj)
        self.dirty = True
        if select:
            self.select(obj)
        #self.setCurrentTool('select')

    def saveUndoInfo(self):
        """ Remember the current state of the document, to allow for undo.

            We make a copy of the document's contents, so that we can return to
            the previous contents if the user does something and then wants to
            undo the operation.
            
            This should be called only for a new modification to the document
            since it erases the redo history.
        """
        state = self._buildStoredState()

        self.undoStack.append(state)
        self.redoStack = []
        self.dirty = True
        self._adjustMenus()

    # =======================
    # == Selection Methods ==
    # =======================

    def setCurrentTool(self, toolName):
        """ Set the currently selected tool.
        """
        
        toolIcon, tool = self.tools[toolName]
        if self.curToolIcon is not None:
            self.curToolIcon.SetValue(False)

        toolIcon.SetValue(True)
        self.curToolName = toolName
        self.curToolIcon = toolIcon
        self.curTool = tool
        self.drawPanel.SetCursor(tool.getDefaultCursor())


    def selectAll(self):
        """ Select every DrawingObject in our document.
        """
        self.selection = []
        for obj in self.contents:
            self.selection.append(obj)
        self.requestRedraw()
        self._adjustMenus()


    def deselectAll(self):
        """ Deselect every DrawingObject in our document.
        """
        self.selection = []
        self.requestRedraw()
        self._adjustMenus()


    def select(self, obj, add=False):
        """ Select the given DrawingObject within our document.

        If 'add' is True obj is added onto the current selection
        """
        if not add:
            self.selection = []
        if obj not in self.selection:
            self.selection += [obj]
            self.requestRedraw()
            self._adjustMenus()

    def selectMany(self, objs):
        """ Select the given list of DrawingObjects.
        """
        self.selection = objs
        self.requestRedraw()
        self._adjustMenus()


    def selectByRectangle(self, x, y, width, height):
        """ Select every DrawingObject in the given rectangular region.
        """
        self.selection = []
        for obj in self.contents:
            if obj.objectWithinRect(x, y, width, height):
                self.selection.append(obj)
        self.requestRedraw()
        self._adjustMenus()

    def getObjectAndSelectionHandleAt(self, pt):
        """ Return the object and selection handle at the given point.

            We draw selection handles (small rectangles) around the currently
            selected object(s).  If the given point is within one of the
            selection handle rectangles, we return the associated object and a
            code indicating which selection handle the point is in.  If the
            point isn't within any selection handle at all, we return the tuple
            (None, None).
        """
        for obj in self.selection:
            handle = obj.getSelectionHandleContainingPoint(pt.x, pt.y)
            if handle is not None:
                return obj, handle

        return None, None


    def getObjectAt(self, pt):
        """ Return the first object found which is at the given point.
        """
        for obj in self.contents:
            if obj.objectContainsPoint(pt.x, pt.y):
                return obj
        return None


    # ======================
    # == File I/O Methods ==
    # ======================

    def loadContents(self):
        """ Load the contents of our document into memory.
        """

        try:
            f = open(self.fileName, "rb")
            objData = cPickle.load(f)
            f.close()

            for klass, data in objData:
                obj = klass()
                obj.setData(data)
                self.contents.append(obj)

            self.dirty = False
            self.selection = []
            self.undoStack  = []
            self.redoStack  = []

            self.requestRedraw()
            self._adjustMenus()
        except:
            response = wx.MessageBox("Unable to load " + self.fileName + ".",
                                     "Error", wx.OK|wx.ICON_ERROR, self)



    def saveContents(self):
        """ Save the contents of our document to disk.
        """
        # SWIG-wrapped native wx contents cannot be pickled, so 
        # we have to convert our data to something pickle-friendly.
        
        try:
            objData = []
            for obj in self.contents:
                objData.append([obj.__class__, obj.getData()])

            f = open(self.fileName, "wb")
            cPickle.dump(objData, f)
            f.close()

            self.dirty = False
            self._adjustMenus()
        except:
            response = wx.MessageBox("Unable to load " + self.fileName + ".",
                                     "Error", wx.OK|wx.ICON_ERROR, self)


    def askIfUserWantsToSave(self, action):
        """ Give the user the opportunity to save the current document.

            'action' is a string describing the action about to be taken.  If
            the user wants to save the document, it is saved immediately.  If
            the user cancels, we return False.
        """
        if not self.dirty: return True # Nothing to do.

        response = wx.MessageBox("Save changes before " + action + "?",
                                "Confirm", wx.YES_NO | wx.CANCEL, self)

        if response == wx.YES:
            if self.fileName == None:
                fileName = wx.FileSelector("Save File As", "Saving",
                                          default_extension="psk",
                                          wildcard="*.psk",
                                          flags = wx.SAVE | wx.OVERWRITE_PROMPT)
                if fileName == "": return False # User cancelled.
                self.fileName = fileName

            self.saveContents()
            return True
        elif response == wx.NO:
            return True # User doesn't want changes saved.
        elif response == wx.CANCEL:
            return False # User cancelled.

    # =====================
    # == Private Methods ==
    # =====================

    def _initBuffer(self):
        """Initialize the bitmap used for buffering the display."""
        size = self.drawPanel.GetSize()
        self.buffer = wx.EmptyBitmap(max(1,size.width),max(1,size.height))
        dc = wx.BufferedDC(None, self.buffer)
        dc.SetBackground(wx.Brush(self.drawPanel.GetBackgroundColour()))
        dc.Clear()
        self.drawContents(dc)
        del dc  # commits all drawing to the buffer

        self.saved_offset = self.drawPanel.CalcUnscrolledPosition(0,0)

        self._reInitBuffer = False



    def _adjustMenus(self):
        """ Adjust our menus and toolbar to reflect the current state of the
            world.

            Doing this manually rather than using an EVT_UPDATE_UI is a bit
            more efficient (since it's only done when it's really needed), 
            but it means we have to remember to call _adjustMenus any time
            menus may need adjusting.
        """
        canSave   = (self.fileName != None) and self.dirty
        canRevert = (self.fileName != None) and self.dirty
        canUndo   = self.undoStack!=[]
        canRedo   = self.redoStack!=[]
        selection = len(self.selection) > 0
        onlyOne   = len(self.selection) == 1
        hasEditor = onlyOne and self.selection[0].hasPropertyEditor()
        front     = onlyOne and (self.selection[0] == self.contents[0])
        back      = onlyOne and (self.selection[0] == self.contents[-1])

        # Enable/disable our menu items.

        self.fileMenu.Enable(wx.ID_SAVE,   canSave)
        self.fileMenu.Enable(wx.ID_REVERT, canRevert)

        self.editMenu.Enable(wx.ID_UNDO,      canUndo)
        self.editMenu.Enable(wx.ID_REDO,      canRedo)
        self.editMenu.Enable(menu_DUPLICATE, selection)
        self.editMenu.Enable(menu_EDIT_PROPS,hasEditor)
        self.editMenu.Enable(wx.ID_CLEAR,    selection)

        self.objectMenu.Enable(menu_MOVE_FORWARD,  onlyOne and not front)
        self.objectMenu.Enable(menu_MOVE_TO_FRONT, onlyOne and not front)
        self.objectMenu.Enable(menu_MOVE_BACKWARD, onlyOne and not back)
        self.objectMenu.Enable(menu_MOVE_TO_BACK,  onlyOne and not back)

        # Enable/disable our toolbar icons.

        self.toolbar.EnableTool(wx.ID_NEW,          True)
        self.toolbar.EnableTool(wx.ID_OPEN,         True)
        self.toolbar.EnableTool(wx.ID_SAVE,         canSave)
        self.toolbar.EnableTool(wx.ID_UNDO,         canUndo)
        self.toolbar.EnableTool(wx.ID_REDO,         canRedo)
        self.toolbar.EnableTool(menu_DUPLICATE,     selection)
        self.toolbar.EnableTool(menu_MOVE_FORWARD,  onlyOne and not front)
        self.toolbar.EnableTool(menu_MOVE_BACKWARD, onlyOne and not back)


    def _setPenColour(self, colour):
        """ Set the default or selected object's pen colour.
        """
        if len(self.selection) > 0:
            self.saveUndoInfo()
            for obj in self.selection:
                obj.setPenColour(colour)
            self.requestRedraw()

        self.penColour = colour
        self.optionIndicator.setPenColour(colour)


    def _setFillColour(self, colour):
        """ Set the default or selected object's fill colour.
        """
        if len(self.selection) > 0:
            self.saveUndoInfo()
            for obj in self.selection:
                obj.setFillColour(colour)
            self.requestRedraw()

        self.fillColour = colour
        self.optionIndicator.setFillColour(colour)


    def _setLineSize(self, size):
        """ Set the default or selected object's line size.
        """
        if len(self.selection) > 0:
            self.saveUndoInfo()
            for obj in self.selection:
                obj.setLineSize(size)
            self.requestRedraw()

        self.lineSize = size
        self.optionIndicator.setLineSize(size)


    def _buildStoredState(self):
        """ Remember the current state of the document, to allow for undo.

            We make a copy of the document's contents, so that we can return to
            the previous contents if the user does something and then wants to
            undo the operation.  

            Returns an object representing the current document state.
        """
        savedContents = []
        for obj in self.contents:
            savedContents.append([obj.__class__, obj.getData()])

        savedSelection = []
        for i in range(len(self.contents)):
            if self.contents[i] in self.selection:
                savedSelection.append(i)

        info = {"contents"  : savedContents,
                "selection" : savedSelection}

        return info
        
    def _restoreStoredState(self, savedState):
        """Restore the state of the document to a previous point for undo/redo.

        Takes a stored state object and recreates the document from it.
        Used by undo/redo implementation.
        """
        self.contents = []

        for draw_class, data in savedState["contents"]:
            obj = draw_class()
            obj.setData(data)
            self.contents.append(obj)

        self.selection = []
        for i in savedState["selection"]:
            self.selection.append(self.contents[i])

        self.dirty = True
        self._adjustMenus()
        self.requestRedraw()

    def _resizeObject(self, obj, anchorPt, oldPt, newPt):
        """ Resize the given object.

            'anchorPt' is the unchanging corner of the object, while the
            opposite corner has been resized.  'oldPt' are the current
            coordinates for this corner, while 'newPt' are the new coordinates.
            The object should fit within the given dimensions, though if the
            new point is less than the anchor point the object will need to be
            moved as well as resized, to avoid giving it a negative size.
        """
        if isinstance(obj, TextDrawingObject):
            # Not allowed to resize text objects -- they're sized to fit text.
            wx.Bell()
            return

        self.saveUndoInfo()

        topLeft  = wx.Point(min(anchorPt.x, newPt.x),
                           min(anchorPt.y, newPt.y))
        botRight = wx.Point(max(anchorPt.x, newPt.x),
                           max(anchorPt.y, newPt.y))

        newWidth  = botRight.x - topLeft.x
        newHeight = botRight.y - topLeft.y

        if isinstance(obj, LineDrawingObject):
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
                obj.setStartPt(wx.Point(0, 0))
                obj.setEndPt(wx.Point(newWidth, newHeight))
            else:
                obj.setStartPt(wx.Point(0, newHeight))
                obj.setEndPt(wx.Point(newWidth, 0))

        # Finally, adjust the bounds of the object to match the new dimensions.

        obj.setPosition(topLeft)
        obj.setSize(wx.Size(botRight.x - topLeft.x, botRight.y - topLeft.y))

        self.requestRedraw()


    def _moveObject(self, offsetX, offsetY):
        """ Move the currently selected object(s) by the given offset.
        """
        self.saveUndoInfo()

        for obj in self.selection:
            pos = obj.getPosition()
            pos.x = pos.x + offsetX
            pos.y = pos.y + offsetY
            obj.setPosition(pos)

        self.requestRedraw()


    def _buildLineSizePopup(self, lineSize):
        """ Build the pop-up menu used to set the line size.

            'lineSize' is the current line size value.  The corresponding item
            is checked in the pop-up menu.
        """
        menu = wx.Menu()
        menu.Append(id_LINESIZE_0, "no line",      kind=wx.ITEM_CHECK)
        menu.Append(id_LINESIZE_1, "1-pixel line", kind=wx.ITEM_CHECK)
        menu.Append(id_LINESIZE_2, "2-pixel line", kind=wx.ITEM_CHECK)
        menu.Append(id_LINESIZE_3, "3-pixel line", kind=wx.ITEM_CHECK)
        menu.Append(id_LINESIZE_4, "4-pixel line", kind=wx.ITEM_CHECK)
        menu.Append(id_LINESIZE_5, "5-pixel line", kind=wx.ITEM_CHECK)

        if   lineSize == 0: menu.Check(id_LINESIZE_0, True)
        elif lineSize == 1: menu.Check(id_LINESIZE_1, True)
        elif lineSize == 2: menu.Check(id_LINESIZE_2, True)
        elif lineSize == 3: menu.Check(id_LINESIZE_3, True)
        elif lineSize == 4: menu.Check(id_LINESIZE_4, True)
        elif lineSize == 5: menu.Check(id_LINESIZE_5, True)

        self.Bind(wx.EVT_MENU, self._lineSizePopupSelected, id=id_LINESIZE_0, id2=id_LINESIZE_5)

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
            wx.Bell()
            return

        self.optionIndicator.setLineSize(self.lineSize)


    def _getEventCoordinates(self, event):
        """ Return the coordinates associated with the given mouse event.

            The coordinates have to be adjusted to allow for the current scroll
            position.
        """
        originX, originY = self.drawPanel.GetViewStart()
        unitX, unitY = self.drawPanel.GetScrollPixelsPerUnit()
        return wx.Point(event.GetX() + (originX * unitX),
                       event.GetY() + (originY * unitY))


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

        dc = wx.ClientDC(self.drawPanel)
        self.drawPanel.PrepareDC(dc)
        dc.BeginDrawing()
        dc.SetPen(wx.BLACK_DASHED_PEN)
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.SetLogicalFunction(wx.INVERT)

        dc.DrawRectangle(position.x + offsetX, position.y + offsetY,
                         size.width, size.height)

        dc.EndDrawing()


#============================================================================
class DrawingTool(object):
    """Base class for drawing tools"""

    def __init__(self):
        pass

    def getDefaultCursor(self):
        """Return the cursor to use by default which this drawing tool is selected"""
        return wx.STANDARD_CURSOR

    def draw(self,dc):
        pass


    def onMouseEvent(self,parent, event):
        """Mouse events passed in from the parent.

        Returns True if the event is handled by the tool,
        False if the canvas can try to use it.
        """
        event.Skip()
        return False

#----------------------------------------------------------------------------
class SelectDrawingTool(DrawingTool):
    """Represents the tool for selecting things"""

    def __init__(self):
        self.curHandle = None
        self.curObject = None
        self.objModified = False
        self.startPt = None
        self.curPt = None

    def getDefaultCursor(self):
        """Return the cursor to use by default which this drawing tool is selected"""
        return wx.STANDARD_CURSOR

    def draw(self, dc):
        if self._doingRectSelection():
            dc.SetPen(wx.BLACK_DASHED_PEN)
            dc.SetBrush(wx.TRANSPARENT_BRUSH)
            x = [self.startPt.x, self.curPt.x]; x.sort()
            y = [self.startPt.y, self.curPt.y]; y.sort()
            dc.DrawRectangle(x[0],y[0], x[1]-x[0],y[1]-y[0])


    def onMouseEvent(self,parent, event):
        handlers = { wx.EVT_LEFT_DOWN.evtType[0]:   self.onMouseLeftDown,
                     wx.EVT_MOTION.evtType[0]:      self.onMouseMotion,
                     wx.EVT_LEFT_UP.evtType[0]:     self.onMouseLeftUp,
                     wx.EVT_LEFT_DCLICK.evtType[0]: self.onMouseLeftDClick }
        handler = handlers.get(event.GetEventType())
        if handler is not None:
            return handler(parent,event)
        else:
            event.Skip()
            return False

    def onMouseLeftDown(self,parent,event):
        mousePt = wx.Point(event.X,event.Y)
        obj, handle = parent.getObjectAndSelectionHandleAt(mousePt)
        self.startPt = mousePt
        self.curPt = mousePt
        if obj is not None and handle is not None:
            self.curObject = obj
            self.curHandle = handle
        else:
            self.curObject = None
            self.curHandle = None
        
        obj = parent.getObjectAt(mousePt)
        if self.curObject is None and obj is not None:
            self.curObject = obj
            self.dragDelta = obj.position-mousePt
            self.curHandle = None
            parent.select(obj, event.ShiftDown())
            
        return True

    def onMouseMotion(self,parent,event):
        if not event.LeftIsDown(): return

        self.curPt = wx.Point(event.X,event.Y)

        obj,handle = self.curObject,self.curHandle
        if self._doingDragHandle():
            self._prepareToModify(parent)
            obj.moveHandle(handle,event.X,event.Y)
            parent.requestRedraw()

        elif self._doingDragObject():
            self._prepareToModify(parent)
            obj.position = self.curPt + self.dragDelta
            parent.requestRedraw()

        elif self._doingRectSelection():
            parent.requestRedraw()

        return True

    def onMouseLeftUp(self,parent,event):

        obj,handle = self.curObject,self.curHandle
        if self._doingDragHandle():
            obj.moveHandle(handle,event.X,event.Y)
            obj.finalizeHandle(handle,event.X,event.Y)

        elif self._doingDragObject():
            curPt = wx.Point(event.X,event.Y)
            obj.position = curPt + self.dragDelta

        elif self._doingRectSelection():
            x = [event.X, self.startPt.x]
            y = [event.Y, self.startPt.y]
            x.sort()
            y.sort()
            parent.selectByRectangle(x[0],y[0],x[1]-x[0],y[1]-y[0])
            

        self.curObject = None
        self.curHandle = None
        self.curPt = None
        self.startPt = None
        self.objModified = False
        parent.requestRedraw()

        return True

    def onMouseLeftDClick(self,parent,event):
        event.Skip()
        mousePt = wx.Point(event.X,event.Y)
        obj = parent.getObjectAt(mousePt)
        if obj and obj.hasPropertyEditor():
            if obj.doPropertyEdit(parent):
                parent.requestRedraw()
                return True

        return False

    
    def _prepareToModify(self,parent):
        if not self.objModified:
            parent.saveUndoInfo()
            self.objModified = True
        
    def _doingRectSelection(self):
        return self.curObject is None \
               and self.startPt is not None \
               and self.curPt is not None

    def _doingDragObject(self):
        return self.curObject is not None and self.curHandle is None

    def _doingDragHandle(self):
        return self.curObject is not None and self.curHandle is not None



#----------------------------------------------------------------------------
class LineDrawingTool(DrawingTool):
    """Represents the tool for drawing lines"""

    def __init__(self):
        self.newObject = None
        self.startPt = None


    def getDefaultCursor(self):
        """Return the cursor to use by default which this drawing tool is selected"""
        return wx.StockCursor(wx.CURSOR_PENCIL)

    def draw(self, dc):
        if self.newObject is None: return
        self.newObject.draw(dc,True)

    def onMouseEvent(self,parent, event):
        handlers = { wx.EVT_LEFT_DOWN.evtType[0]: self.onMouseLeftDown,
                     wx.EVT_MOTION.evtType[0]:    self.onMouseMotion,
                     wx.EVT_LEFT_UP.evtType[0]:   self.onMouseLeftUp }
        handler = handlers.get(event.GetEventType())
        if handler is not None:
            return handler(parent,event)
        else:
            event.Skip()
            return False

    def onMouseLeftDown(self,parent, event):
        self.startPt = wx.Point(event.GetX(), event.GetY())
        self.newObject = None
        event.Skip()
        return True

    def onMouseMotion(self,parent, event):
        if not event.Dragging(): return

        if self.newObject is None:
            obj = LineDrawingObject(startPt=wx.Point(0,0),
                                    penColour=parent.penColour,
                                    fillColour=parent.fillColour,
                                    lineSize=parent.lineSize,
                                    position=wx.Point(event.X,event.Y))
            self.newObject = obj

        self._updateObjFromEvent(self.newObject, event)

        parent.requestRedraw()
        event.Skip()
        return True

    def onMouseLeftUp(self,parent, event):

        if self.newObject is None:
            return

        self._updateObjFromEvent(self.newObject,event)

        parent.addObject(self.newObject)

        self.newObject = None
        self.startPt = None

        event.Skip()
        return True


    def _updateObjFromEvent(self,obj,event):
        obj.setEndPt(wx.Point(event.X,event.Y))


#----------------------------------------------------------------------------
class RectDrawingTool(DrawingTool):
    """Represents the tool for drawing rectangles"""

    def __init__(self):
        self.newObject = None

    def getDefaultCursor(self):
        """Return the cursor to use by default which this drawing tool is selected"""
        return wx.CROSS_CURSOR

    def draw(self, dc):
        if self.newObject is None: return
        self.newObject.draw(dc,True)


    def onMouseEvent(self,parent, event):
        handlers = { wx.EVT_LEFT_DOWN.evtType[0]: self.onMouseLeftDown,
                     wx.EVT_MOTION.evtType[0]:    self.onMouseMotion,
                     wx.EVT_LEFT_UP.evtType[0]:   self.onMouseLeftUp }
        handler = handlers.get(event.GetEventType())
        if handler is not None:
            return handler(parent,event)
        else:
            event.Skip()
            return False

    def onMouseLeftDown(self,parent, event):
        self.startPt = wx.Point(event.GetX(), event.GetY())
        self.newObject = None
        event.Skip()
        return True

    def onMouseMotion(self,parent, event):
        if not event.Dragging(): return

        if self.newObject is None:
            obj = RectDrawingObject(penColour=parent.penColour,
                                    fillColour=parent.fillColour,
                                    lineSize=parent.lineSize)
            self.newObject = obj

        self._updateObjFromEvent(self.newObject, event)

        parent.requestRedraw()
        event.Skip()
        return True

    def onMouseLeftUp(self,parent, event):

        if self.newObject is None:
            return

        self._updateObjFromEvent(self.newObject,event)

        parent.addObject(self.newObject)

        self.newObject = None

        event.Skip()
        return True


    def _updateObjFromEvent(self,obj,event):
        x = [event.X, self.startPt.x]
        y = [event.Y, self.startPt.y]
        x.sort()
        y.sort()
        width = x[1]-x[0]
        height = y[1]-y[0]

        obj.setPosition(wx.Point(x[0],y[0]))
        obj.setSize(wx.Size(width,height))

        


#----------------------------------------------------------------------------
class EllipseDrawingTool(DrawingTool):
    """Represents the tool for drawing ellipses"""

    def getDefaultCursor(self):
        """Return the cursor to use by default which this drawing tool is selected"""
        return wx.CROSS_CURSOR


    def __init__(self):
        self.newObject = None

    def getDefaultCursor(self):
        """Return the cursor to use by default which this drawing tool is selected"""
        return wx.CROSS_CURSOR

    def draw(self, dc):
        if self.newObject is None: return
        self.newObject.draw(dc,True)


    def onMouseEvent(self,parent, event):
        handlers = { wx.EVT_LEFT_DOWN.evtType[0]: self.onMouseLeftDown,
                     wx.EVT_MOTION.evtType[0]:    self.onMouseMotion,
                     wx.EVT_LEFT_UP.evtType[0]:   self.onMouseLeftUp }
        handler = handlers.get(event.GetEventType())
        if handler is not None:
            return handler(parent,event)
        else:
            event.Skip()
            return False
            
    def onMouseLeftDown(self,parent, event):
        self.startPt = wx.Point(event.GetX(), event.GetY())
        self.newObject = None
        event.Skip()
        return True

    def onMouseMotion(self,parent, event):
        if not event.Dragging(): return

        if self.newObject is None:
            obj = EllipseDrawingObject(penColour=parent.penColour,
                                       fillColour=parent.fillColour,
                                       lineSize=parent.lineSize)
            self.newObject = obj

        self._updateObjFromEvent(self.newObject, event)

        parent.requestRedraw()
        event.Skip()
        return True 

    def onMouseLeftUp(self,parent, event):

        if self.newObject is None:
            return

        self._updateObjFromEvent(self.newObject,event)

        parent.addObject(self.newObject)

        self.newObject = None

        event.Skip()
        return True


    def _updateObjFromEvent(self,obj,event):
        x = [event.X, self.startPt.x]
        y = [event.Y, self.startPt.y]
        x.sort()
        y.sort()
        width = x[1]-x[0]
        height = y[1]-y[0]

        obj.setPosition(wx.Point(x[0],y[0]))
        obj.setSize(wx.Size(width,height))


#----------------------------------------------------------------------------
class PolygonDrawingTool(DrawingTool):
    """Represents the tool for drawing polygons"""

    def __init__(self):
        self.newObject = None

    def getDefaultCursor(self):
        """Return the cursor to use by default which this drawing tool is selected"""
        return wx.CROSS_CURSOR


    def draw(self, dc):
        if self.newObject is None: return
        self.newObject.draw(dc,True)


    def onMouseEvent(self,parent, event):
        handlers = { wx.EVT_LEFT_DOWN.evtType[0]:  self.onMouseLeftDown,
                     wx.EVT_MOTION.evtType[0]:     self.onMouseMotion,
                     wx.EVT_LEFT_UP.evtType[0]:    self.onMouseLeftUp, 
                     wx.EVT_LEFT_DCLICK.evtType[0]:self.onMouseLeftDClick }
        handler = handlers.get(event.GetEventType())
        if handler is not None:
            return handler(parent,event)
        else:
            event.Skip()
            return False
            
    def onMouseLeftDown(self,parent, event):
        event.Skip()
        self.startPt = (event.GetX(), event.GetY())
        if self.newObject is None:
            obj = PolygonDrawingObject(points=[(0,0)],penColour=parent.penColour,
                                       fillColour=parent.fillColour,
                                       lineSize=parent.lineSize,
                                       position=wx.Point(event.X, event.Y))
            obj.addPoint(event.X,event.Y)
            self.newObject = obj
        else:
            CLOSE_THRESH=3
            pt0 = self.newObject.getPoint(0)
            if abs(pt0[0]-event.X)<CLOSE_THRESH and abs(pt0[1]-event.Y)<CLOSE_THRESH:
                self.newObject.popPoint()
                parent.addObject(self.newObject)
                self.newObject = None
            else:
                self.newObject.addPoint(event.X,event.Y)

        return True

    def onMouseMotion(self,parent, event):

        event.Skip()
        if self.newObject:
            self.newObject.movePoint(-1, event.X, event.Y)
            parent.requestRedraw()
            return True 

        return False

    def onMouseLeftDClick(self,parent,event):
        event.Skip()
        if self.newObject:
            CLOSE_THRESH=3
            pt0 = self.newObject.getPoint(0)
            if abs(pt0[0]-event.X)<CLOSE_THRESH and abs(pt0[1]-event.Y)<CLOSE_THRESH:
                self.newObject.popPoint()
            self.newObject.popPoint()
            parent.addObject(self.newObject)
            self.newObject = None

        return True

    def onMouseLeftUp(self,parent, event):
        event.Skip()
        return True

        


#----------------------------------------------------------------------------
class ScribbleDrawingTool(DrawingTool):
    """Represents the tool for drawing scribble drawing objects"""

    def __init__(self):
        self.newObject = None

    def getDefaultCursor(self):
        """Return the cursor to use by default which this drawing tool is selected"""
        return wx.StockCursor(wx.CURSOR_PENCIL)

    def draw(self, dc):
        if self.newObject is None: return
        self.newObject.draw(dc,True)


    def onMouseEvent(self,parent, event):
        handlers = { wx.EVT_LEFT_DOWN.evtType[0]:  self.onMouseLeftDown,
                     wx.EVT_MOTION.evtType[0]:     self.onMouseMotion,
                     wx.EVT_LEFT_UP.evtType[0]:    self.onMouseLeftUp 
                    }
        handler = handlers.get(event.GetEventType())
        if handler is not None:
            return handler(parent,event)
        else:
            event.Skip()
            return False
            
    def onMouseLeftDown(self,parent, event):
        event.Skip()
        obj = ScribbleDrawingObject(points=[(0,0)],penColour=parent.penColour,
                                    fillColour=parent.fillColour,
                                    lineSize=parent.lineSize,
                                    position=wx.Point(event.X, event.Y))
        self.newObject = obj
        return True

    def onMouseMotion(self,parent, event):
        event.Skip()
        if self.newObject:
            self.newObject.addPoint(event.X,event.Y)
            parent.requestRedraw()
            return True 

        return False

    def onMouseLeftUp(self,parent, event):
        event.Skip()
        if self.newObject:
            parent.addObject(self.newObject)
            self.newObject = None
        return True

        



#----------------------------------------------------------------------------
class TextDrawingTool(DrawingTool):
    """Represents the tool for drawing text"""

    def getDefaultCursor(self):
        """Return the cursor to use by default which this drawing tool is selected"""
        return wx.StockCursor(wx.CURSOR_IBEAM)

    def onMouseEvent(self,parent, event):
        handlers = { #wx.EVT_LEFT_DOWN.evtType[0]: self.onMouseLeftDown,
                     #wx.EVT_MOTION.evtType[0]:    self.onMouseMotion,
                     wx.EVT_LEFT_UP.evtType[0]:   self.onMouseLeftUp 
                     }
        handler = handlers.get(event.GetEventType())
        if handler is not None:
            return handler(parent,event)
        else:
            event.Skip()
            return False

    def onMouseLeftUp(self,parent, event):

        editor = parent.getTextEditor()
        editor.SetTitle("Create Text Object")
        if editor.ShowModal() == wx.ID_CANCEL:
            editor.Hide()
            return True

        obj = TextDrawingObject(position=wx.Point(event.X, event.Y))
        editor.dialogToObject(obj)
        editor.Hide()

        parent.addObject(obj)

        event.Skip()
        return True



#============================================================================
class DrawingObject(object):
    """ Base class for objects within the drawing panel.

        A pySketch document consists of a front-to-back ordered list of
        DrawingObjects.  Each DrawingObject has the following properties:

            'position'      The position of the object within the document.
            'size'          The size of the object within the document.
            'penColour'     The colour to use for drawing the object's outline.
            'fillColour'    Colour to use for drawing object's interior.
            'lineSize'      Line width (in pixels) to use for object's outline.
            """

    # ==================
    # == Constructors ==
    # ==================

    def __init__(self, position=wx.Point(0, 0), size=wx.Size(0, 0),
                 penColour=wx.BLACK, fillColour=wx.WHITE, lineSize=1,
                 ):
        """ Standard constructor.

            The remaining parameters let you set various options for the newly
            created DrawingObject.
        """
        # One must take great care with constructed default arguments
        # like wx.Point(0,0) above.  *EVERY* caller that uses the
        # default will get the same instance.  Thus, below we make a
        # deep copy of those arguments with object defaults.

        self.position          = wx.Point(position.x,position.y)
        self.size              = wx.Size(size.x,size.y)
        self.penColour         = penColour
        self.fillColour        = fillColour
        self.lineSize          = lineSize

    # =============================
    # == Object Property Methods ==
    # =============================

    def getData(self):
        """ Return a copy of the object's internal data.

            This is used to save this DrawingObject to disk.
        """
        return [self.position.x, self.position.y,
                self.size.width, self.size.height,
                self.penColour.Red(),
                self.penColour.Green(),
                self.penColour.Blue(),
                self.fillColour.Red(),
                self.fillColour.Green(),
                self.fillColour.Blue(),
                self.lineSize]


    def setData(self, data):
        """ Set the object's internal data.

            'data' is a copy of the object's saved data, as returned by
            getData() above.  This is used to restore a previously saved
            DrawingObject.

            Returns an iterator to any remaining data not consumed by 
            this base class method.
        """
        #data = copy.deepcopy(data) # Needed?

        d = iter(data)
        try:
            self.position          = wx.Point(d.next(), d.next())
            self.size              = wx.Size(d.next(), d.next())
            self.penColour         = wx.Colour(red=d.next(),
                                              green=d.next(),
                                              blue=d.next())
            self.fillColour        = wx.Colour(red=d.next(),
                                              green=d.next(),
                                              blue=d.next())
            self.lineSize          = d.next()
        except StopIteration:
            raise ValueError('Not enough data in setData call')

        return d


    def hasPropertyEditor(self):
        return False

    def doPropertyEdit(self, parent):
        assert False, "Must be overridden if hasPropertyEditor returns True"

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


    # ============================
    # == Object Drawing Methods ==
    # ============================

    def draw(self, dc, selected):
        """ Draw this DrawingObject into our window.

            'dc' is the device context to use for drawing.  

            If 'selected' is True, the object is currently selected.
            Drawing objects can use this to change the way selected objects
            are drawn, however the actual drawing of selection handles
            should be done in the 'drawHandles' method
        """
        if self.lineSize == 0:
            dc.SetPen(wx.Pen(self.penColour, self.lineSize, wx.TRANSPARENT))
        else:
            dc.SetPen(wx.Pen(self.penColour, self.lineSize, wx.SOLID))
        dc.SetBrush(wx.Brush(self.fillColour, wx.SOLID))

        self._privateDraw(dc, self.position, selected)


    def drawHandles(self, dc):
        """Draw selection handles for this DrawingObject"""

        # Default is to draw selection handles at all four corners.
        dc.SetPen(wx.BLACK_PEN)
        dc.SetBrush(wx.BLACK_BRUSH)
        
        x,y = self.position
        self._drawSelHandle(dc, x, y)
        self._drawSelHandle(dc, x + self.size.width, y)
        self._drawSelHandle(dc, x, y + self.size.height)
        self._drawSelHandle(dc, x + self.size.width, y + self.size.height)


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

        # Now things get tricky.  There's no straightforward way of
        # knowing whether the point is within an arbitrary object's
        # bounds...to get around this, we draw the object into a
        # memory-based bitmap and see if the given point was drawn.
        # This could no doubt be done more efficiently by some tricky
        # maths, but this approach works and is simple enough.

        # Subclasses can implement smarter faster versions of this.

        bitmap = wx.EmptyBitmap(self.size.x + 10, self.size.y + 10)
        dc = wx.MemoryDC()
        dc.SelectObject(bitmap)
        dc.BeginDrawing()
        dc.SetBackground(wx.WHITE_BRUSH)
        dc.Clear()
        dc.SetPen(wx.Pen(wx.BLACK, self.lineSize + 5, wx.SOLID))
        dc.SetBrush(wx.BLACK_BRUSH)
        self._privateDraw(dc, wx.Point(5, 5), True)
        dc.EndDrawing()
        pixel = dc.GetPixel(x - self.position.x + 5, y - self.position.y + 5)
        if (pixel.Red() == 0) and (pixel.Green() == 0) and (pixel.Blue() == 0):
            return True
        else:
            return False

    handle_TOP    = 0
    handle_BOTTOM = 1
    handle_LEFT   = 0
    handle_RIGHT  = 1

    def getSelectionHandleContainingPoint(self, x, y):
        """ Return the selection handle containing the given point, if any.

            We return one of the predefined selection handle ID codes.
        """
        # Default implementation assumes selection handles at all four bbox corners.
        # Return a list so we can modify the contents later in moveHandle()
        if self._pointInSelRect(x, y, self.position.x, self.position.y):
            return [self.handle_TOP, self.handle_LEFT]
        elif self._pointInSelRect(x, y, self.position.x + self.size.width,
                                        self.position.y):
            return [self.handle_TOP, self.handle_RIGHT]
        elif self._pointInSelRect(x, y, self.position.x,
                                        self.position.y + self.size.height):
            return [self.handle_BOTTOM, self.handle_LEFT]
        elif self._pointInSelRect(x, y, self.position.x + self.size.width,
                                        self.position.y + self.size.height):
            return [self.handle_BOTTOM, self.handle_RIGHT]
        else:
            return None

    def moveHandle(self, handle, x, y):
        """ Move the specified selection handle to given canvas location.
        """
        assert handle is not None

        # Default implementation assumes selection handles at all four bbox corners.
        pt = wx.Point(x,y)
        x,y = self.position
        w,h = self.size
        if handle[0] == self.handle_TOP:
            if handle[1] == self.handle_LEFT:
                dpos = pt - self.position
                self.position = pt
                self.size.width -= dpos.x
                self.size.height -= dpos.y
            else:
                dx = pt.x - ( x + w )
                dy = pt.y - ( y )
                self.position.y = pt.y
                self.size.width += dx
                self.size.height -= dy
        else: # BOTTOM
            if handle[1] == self.handle_LEFT:
                dx = pt.x - ( x )
                dy = pt.y - ( y + h )
                self.position.x = pt.x
                self.size.width -= dx
                self.size.height += dy
            else: 
                dpos = pt - self.position
                dpos.x -= w
                dpos.y -= h
                self.size.width += dpos.x
                self.size.height += dpos.y


        # Finally, normalize so no negative widths or heights.
        # And update the handle variable accordingly.
        if self.size.height<0:
            self.position.y += self.size.height
            self.size.height = -self.size.height
            handle[0] = 1-handle[0]

        if self.size.width<0:
            self.position.x += self.size.width
            self.size.width = -self.size.width
            handle[1] = 1-handle[1]
            


    def finalizeHandle(self, handle, x, y):
        pass


    def objectWithinRect(self, x, y, width, height):
        """ Return True iff this object falls completely within the given rect.
        """
        if x          > self.position.x:                    return False
        if x + width  < self.position.x + self.size.width:  return False
        if y          > self.position.y:                    return False
        if y + height < self.position.y + self.size.height: return False
        return True

    # =====================
    # == Private Methods ==
    # =====================

    def _privateDraw(self, dc, position, selected):
        """ Private routine to draw this DrawingObject.

            'dc' is the device context to use for drawing, while 'position' is
            the position in which to draw the object.
        """
        pass

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
class LineDrawingObject(DrawingObject):
    """ DrawingObject subclass that represents one line segment.

        Adds the following members to the base DrawingObject:
            'startPt'       The point, relative to the object's position, where
                            the line starts.
            'endPt'         The point, relative to the object's position, where
                            the line ends.
            """

    def __init__(self, startPt=wx.Point(0,0), endPt=wx.Point(0,0), *varg, **kwarg):
        DrawingObject.__init__(self, *varg, **kwarg)

        self.startPt           = wx.Point(startPt.x,startPt.y)
        self.endPt             = wx.Point(endPt.x,endPt.y)

    # ============================
    # == Object Drawing Methods ==
    # ============================

    def drawHandles(self, dc):
        """Draw selection handles for this DrawingObject"""

        dc.SetPen(wx.BLACK_PEN)
        dc.SetBrush(wx.BLACK_BRUSH)

        x,y = self.position
        # Draw selection handles at the start and end points.
        self._drawSelHandle(dc, x + self.startPt.x, y + self.startPt.y)
        self._drawSelHandle(dc, x + self.endPt.x,   y + self.endPt.y)



    # =======================
    # == Selection Methods ==
    # =======================


    handle_START_POINT = 1
    handle_END_POINT = 2

    def getSelectionHandleContainingPoint(self, x, y):
        """ Return the selection handle containing the given point, if any.

            We return one of the predefined selection handle ID codes.
        """
        # We have selection handles at the start and end points.
        if self._pointInSelRect(x, y, self.position.x + self.startPt.x,
                                      self.position.y + self.startPt.y):
            return self.handle_START_POINT
        elif self._pointInSelRect(x, y, self.position.x + self.endPt.x,
                                        self.position.y + self.endPt.y):
            return self.handle_END_POINT
        else:
            return None
        
    def moveHandle(self, handle, x, y):
        """Move the handle to specified handle to the specified canvas coordinates
        """
        ptTrans = wx.Point(x-self.position.x, y-self.position.y)
        if handle == self.handle_START_POINT:
            self.startPt = ptTrans
        elif handle == self.handle_END_POINT:
            self.endPt = ptTrans
        else:
            raise ValueError("Bad handle type for a line")

        self._updateBoundingBox()

    # =============================
    # == Object Property Methods ==
    # =============================

    def getData(self):
        """ Return a copy of the object's internal data.

            This is used to save this DrawingObject to disk.
        """
        # get the basics
        data = DrawingObject.getData(self)
        # add our specifics
        data += [self.startPt.x, self.startPt.y,
                 self.endPt.x,   self.endPt.y]
        return data

    def setData(self, data):
        """ Set the object's internal data.

            'data' is a copy of the object's saved data, as returned by
            getData() above.  This is used to restore a previously saved
            DrawingObject.
        """
        #data = copy.deepcopy(data) # Needed?

        d = DrawingObject.setData(self, data)

        try:
            self.startPt           = wx.Point(d.next(), d.next())
            self.endPt             = wx.Point(d.next(), d.next())
        except StopIteration:
            raise ValueError('Not enough data in setData call')

        return d


    def setStartPt(self, startPt):
        """ Set the starting point for this line DrawingObject.
        """
        self.startPt = startPt - self.position
        self._updateBoundingBox()


    def getStartPt(self):
        """ Return the starting point for this line DrawingObject.
        """
        return self.startPt + self.position


    def setEndPt(self, endPt):
        """ Set the ending point for this line DrawingObject.
        """
        self.endPt = endPt - self.position
        self._updateBoundingBox()


    def getEndPt(self):
        """ Return the ending point for this line DrawingObject.
        """
        return self.endPt + self.position


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
        dc.DrawLine(position.x + self.startPt.x,
                    position.y + self.startPt.y,
                    position.x + self.endPt.x,
                    position.y + self.endPt.y)
            
    def _updateBoundingBox(self):
        x = [self.startPt.x, self.endPt.x]; x.sort()
        y = [self.startPt.y, self.endPt.y]; y.sort()

        dp = wx.Point(-x[0],-y[0])
        self.position.x += x[0]
        self.position.y += y[0]
        self.size.width = x[1]-x[0]
        self.size.height = y[1]-y[0]

        self.startPt += dp
        self.endPt += dp

#----------------------------------------------------------------------------
class PolygonDrawingObject(DrawingObject):
    """ DrawingObject subclass that represents a poly-line or polygon
    """
    def __init__(self, points=[], *varg, **kwarg):
        DrawingObject.__init__(self, *varg, **kwarg)
        self.points = list(points)

    # =======================
    # == Selection Methods ==
    # =======================

    def getSelectionHandleContainingPoint(self, x, y):
        """ Return the selection handle containing the given point, if any.

            We return one of the predefined selection handle ID codes.
        """
        # We have selection handles at the start and end points.
        for i,p in enumerate(self.points):
            if self._pointInSelRect(x, y,
                                    self.position.x + p[0],
                                    self.position.y + p[1]):
                return i+1

        return None
        

    def addPoint(self, x,y):
        self.points.append((x-self.position.x,y-self.position.y))
        self._updateBoundingBox()

    def getPoint(self, idx):
        x,y = self.points[idx]
        return (x+self.position.x,y+self.position.y)

    def movePoint(self, idx, x,y):
        self.points[idx] = (x-self.position.x,y-self.position.y)
        self._updateBoundingBox()

    def popPoint(self, idx=-1):
        self.points.pop(idx)
        self._updateBoundingBox()

    # =====================
    # == Drawing Methods ==
    # =====================

    def drawHandles(self, dc):
        """Draw selection handles for this DrawingObject"""

        dc.SetPen(wx.BLACK_PEN)
        dc.SetBrush(wx.BLACK_BRUSH)

        x,y = self.position
        # Draw selection handles at the start and end points.
        for p in self.points:
            self._drawSelHandle(dc, x + p[0], y + p[1])

    def moveHandle(self, handle, x, y):
        """Move the specified handle"""
        self.movePoint(handle-1,x,y)


    # =============================
    # == Object Property Methods ==
    # =============================

    def getData(self):
        """ Return a copy of the object's internal data.

            This is used to save this DrawingObject to disk.
        """
        # get the basics
        data = DrawingObject.getData(self)
        # add our specifics
        data += [list(self.points)]

        return data


    def setData(self, data):
        """ Set the object's internal data.

            'data' is a copy of the object's saved data, as returned by
            getData() above.  This is used to restore a previously saved
            DrawingObject.
        """
        #data = copy.deepcopy(data) # Needed?
        d = DrawingObject.setData(self, data)

        try:
            self.points            = d.next()
        except StopIteration:
            raise ValueError('Not enough data in setData call')

        return d
        

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
        dc.DrawPolygon(self.points, position.x, position.y)

    def _updateBoundingBox(self):
        x = min([p[0] for p in self.points])
        y = min([p[1] for p in self.points])
        x2 = max([p[0] for p in self.points])
        y2 = max([p[1] for p in self.points])
        dx = -x
        dy = -y
        self.position.x += x
        self.position.y += y
        self.size.width = x2-x
        self.size.height = y2-y
        # update coords also because they're relative to self.position
        for i,p in enumerate(self.points):
            self.points[i] = (p[0]+dx,p[1]+dy)
            

#----------------------------------------------------------------------------
class ScribbleDrawingObject(DrawingObject):
    """ DrawingObject subclass that represents a poly-line or polygon
    """
    def __init__(self, points=[], *varg, **kwarg):
        DrawingObject.__init__(self, *varg, **kwarg)
        self.points = list(points)

    # =======================
    # == Selection Methods ==
    # =======================

    def addPoint(self, x,y):
        self.points.append((x-self.position.x,y-self.position.y))
        self._updateBoundingBox()

    def getPoint(self, idx):
        x,y = self.points[idx]
        return (x+self.position.x,y+self.position.y)

    def movePoint(self, idx, x,y):
        self.points[idx] = (x-self.position.x,y-self.position.y)
        self._updateBoundingBox()

    def popPoint(self, idx=-1):
        self.points.pop(idx)
        self._updateBoundingBox()


    # =============================
    # == Object Property Methods ==
    # =============================

    def getData(self):
        """ Return a copy of the object's internal data.

            This is used to save this DrawingObject to disk.
        """
        # get the basics
        data = DrawingObject.getData(self)
        # add our specifics
        data += [list(self.points)]

        return data


    def setData(self, data):
        """ Set the object's internal data.

            'data' is a copy of the object's saved data, as returned by
            getData() above.  This is used to restore a previously saved
            DrawingObject.
        """
        #data = copy.deepcopy(data) # Needed?
        d = DrawingObject.setData(self, data)

        try:
            self.points            = d.next()
        except StopIteration:
            raise ValueError('Not enough data in setData call')

        return d
        

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
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.DrawLines(self.points, position.x, position.y)

    def _updateBoundingBox(self):
        x = min([p[0] for p in self.points])
        y = min([p[1] for p in self.points])
        x2 = max([p[0] for p in self.points])
        y2 = max([p[1] for p in self.points])
        dx = -x
        dy = -y
        self.position = wx.Point(self.position.x + x,self.position.y + y)
        self.size = wx.Size(x2-x, y2-y)
        #self.position.x += x
        #self.position.y += y
        #self.size.width = x2-x
        #self.size.height = y2-y
        # update coords also because they're relative to self.position
        for i,p in enumerate(self.points):
            self.points[i] = (p[0]+dx,p[1]+dy)

#----------------------------------------------------------------------------
class RectDrawingObject(DrawingObject):
    """ DrawingObject subclass that represents an axis-aligned rectangle.
    """
    def __init__(self, *varg, **kwarg):
        DrawingObject.__init__(self, *varg, **kwarg)

    def objectContainsPoint(self, x, y):
        """ Returns True iff this object contains the given point.

            This is used to determine if the user clicked on the object.
        """
        # Firstly, ignore any points outside of the object's bounds.

        if x < self.position.x: return False
        if x > self.position.x + self.size.x: return False
        if y < self.position.y: return False
        if y > self.position.y + self.size.y: return False

        # Rectangles are easy -- they're always selected if the
        # point is within their bounds.
        return True

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
        dc.DrawRectangle(position.x, position.y,
                         self.size.width, self.size.height)


#----------------------------------------------------------------------------
class EllipseDrawingObject(DrawingObject):
    """ DrawingObject subclass that represents an axis-aligned ellipse.
    """
    def __init__(self, *varg, **kwarg):
        DrawingObject.__init__(self, *varg, **kwarg)

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
        dc.DrawEllipse(position.x, position.y,
                       self.size.width, self.size.height)




#----------------------------------------------------------------------------
class TextDrawingObject(DrawingObject):
    """ DrawingObject subclass that holds text.

        Adds the following members to the base DrawingObject:
            'text'          The object's text (obj_TEXT objects only).
            'textFont'      The text object's font name.
            """

    def __init__(self, text=None, *varg, **kwarg):
        DrawingObject.__init__(self, *varg, **kwarg)

        self.text              = text
        self.textFont          = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)


    # =============================
    # == Object Property Methods ==
    # =============================

    def getData(self):
        """ Return a copy of the object's internal data.

            This is used to save this DrawingObject to disk.
        """
        # get the basics
        data = DrawingObject.getData(self)
        # add our specifics
        data += [self.text, self.textFont.GetNativeFontInfoDesc()]

        return data


    def setData(self, data):
        """ Set the object's internal data.

            'data' is a copy of the object's saved data, as returned by
            getData() above.  This is used to restore a previously saved
            DrawingObject.
        """
        d = DrawingObject.setData(self, data)

        try:
            self.text              = d.next()
            desc                   = d.next()
            self.textFont = wx.FontFromNativeInfoString(desc)
        except StopIteration:
            raise ValueError('Not enough data in setData call')

        return d
        

    def hasPropertyEditor(self):
        return True

    def doPropertyEdit(self, parent):
        editor = parent.getTextEditor()
        editor.SetTitle("Edit Text Object")
        editor.objectToDialog(self)
        if editor.ShowModal() == wx.ID_CANCEL:
            editor.Hide()
            return False

        parent.saveUndoInfo()

        editor.dialogToObject(self)
        editor.Hide()

        return True


    def setText(self, text):
        """ Set the text for this DrawingObject.
        """
        self.text = text


    def getText(self):
        """ Return this DrawingObject's text.
        """
        return self.text


    def setFont(self, font):
        """ Set the font for this text DrawingObject.
        """
        self.textFont = font


    def getFont(self):
        """ Return this text DrawingObject's font.
        """
        return self.textFont



    # ============================
    # == Object Drawing Methods ==
    # ============================

    def draw(self, dc, selected):
        """ Draw this DrawingObject into our window.

            'dc' is the device context to use for drawing.  If 'selected' is
            True, the object is currently selected and should be drawn as such.
        """
        dc.SetTextForeground(self.penColour)
        dc.SetTextBackground(self.fillColour)

        self._privateDraw(dc, self.position, selected)

    def objectContainsPoint(self, x, y):
        """ Returns True iff this object contains the given point.

            This is used to determine if the user clicked on the object.
        """
        # Firstly, ignore any points outside of the object's bounds.

        if x < self.position.x: return False
        if x > self.position.x + self.size.x: return False
        if y < self.position.y: return False
        if y > self.position.y + self.size.y: return False

        # Text is easy -- it's always selected if the
        # point is within its bounds.
        return True


    def fitToText(self):
        """ Resize a text DrawingObject so that it fits it's text exactly.
        """

        dummyWindow = wx.Frame(None, -1, "")
        dummyWindow.SetFont(self.textFont)
        width, height = dummyWindow.GetTextExtent(self.text)
        dummyWindow.Destroy()

        self.size = wx.Size(width, height)

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
        dc.SetFont(self.textFont)
        dc.DrawText(self.text, position.x, position.y)



#----------------------------------------------------------------------------
class ToolPaletteToggleX(wx.ToggleButton):
    """ An icon appearing in the tool palette area of our sketching window.

        Note that this is actually implemented as a wx.Bitmap rather
        than as a wx.Icon.  wx.Icon has a very specific meaning, and isn't
        appropriate for this more general use.
    """

    def __init__(self, parent, iconID, iconName, toolTip, mode = wx.ITEM_NORMAL):
        """ Standard constructor.

            'parent'   is the parent window this icon will be part of.
            'iconID'   is the internal ID used for this icon.
            'iconName' is the name used for this icon.
            'toolTip'  is the tool tip text to show for this icon.
            'mode'     is one of wx.ITEM_NORMAL, wx.ITEM_CHECK, wx.ITEM_RADIO

            The icon name is used to get the appropriate bitmap for this icon.
        """
        bmp = wx.Bitmap("images/" + iconName + "Icon.bmp", wx.BITMAP_TYPE_BMP)
        bmpsel = wx.Bitmap("images/" + iconName + "IconSel.bmp", wx.BITMAP_TYPE_BMP)

        wx.ToggleButton.__init__(self, parent, iconID,
                                 size=(bmp.GetWidth()+1, bmp.GetHeight()+1)
                                 )
        self.SetLabel( iconName )
        self.SetToolTip(wx.ToolTip(toolTip))
        #self.SetBitmapLabel(bmp)
        #self.SetBitmapSelected(bmpsel)

        self.iconID     = iconID
        self.iconName   = iconName

class ToolPaletteToggle(GenBitmapToggleButton):
    """ An icon appearing in the tool palette area of our sketching window.

        Note that this is actually implemented as a wx.Bitmap rather
        than as a wx.Icon.  wx.Icon has a very specific meaning, and isn't
        appropriate for this more general use.
    """

    def __init__(self, parent, iconID, iconName, toolTip, mode = wx.ITEM_NORMAL):
        """ Standard constructor.

            'parent'   is the parent window this icon will be part of.
            'iconID'   is the internal ID used for this icon.
            'iconName' is the name used for this icon.
            'toolTip'  is the tool tip text to show for this icon.
            'mode'     is one of wx.ITEM_NORMAL, wx.ITEM_CHECK, wx.ITEM_RADIO

            The icon name is used to get the appropriate bitmap for this icon.
        """
        bmp = wx.Bitmap("images/" + iconName + "Icon.bmp", wx.BITMAP_TYPE_BMP)
        bmpsel = wx.Bitmap("images/" + iconName + "IconSel.bmp", wx.BITMAP_TYPE_BMP)

        GenBitmapToggleButton.__init__(self, parent, iconID, bitmap=bmp, 
                                       size=(bmp.GetWidth()+1, bmp.GetHeight()+1),
                                       style=wx.BORDER_NONE)

        self.SetToolTip(wx.ToolTip(toolTip))
        self.SetBitmapLabel(bmp)
        self.SetBitmapSelected(bmpsel)

        self.iconID     = iconID
        self.iconName   = iconName


class ToolPaletteButton(GenBitmapButton):
    """ An icon appearing in the tool palette area of our sketching window.

        Note that this is actually implemented as a wx.Bitmap rather
        than as a wx.Icon.  wx.Icon has a very specific meaning, and isn't
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
        bmp = wx.Bitmap("images/" + iconName + "Icon.bmp", wx.BITMAP_TYPE_BMP)
        GenBitmapButton.__init__(self, parent, iconID, bitmap=bmp, 
                                 size=(bmp.GetWidth()+1, bmp.GetHeight()+1),
                                 style=wx.BORDER_NONE)
        self.SetToolTip(wx.ToolTip(toolTip))
        self.SetBitmapLabel(bmp)

        self.iconID     = iconID
        self.iconName   = iconName



#----------------------------------------------------------------------------

class ToolOptionIndicator(wx.Window):
    """ A visual indicator which shows the current tool options.
    """
    def __init__(self, parent):
        """ Standard constructor.
        """
        wx.Window.__init__(self, parent, -1, wx.DefaultPosition, wx.Size(52, 32))

        self.penColour  = wx.BLACK
        self.fillColour = wx.WHITE
        self.lineSize   = 1

        # Win95 can only handle a 8x8 stipple bitmaps max
        #self.stippleBitmap = wx.BitmapFromBits("\xf0"*4 + "\x0f"*4,8,8)
        # ...but who uses Win95?
        self.stippleBitmap = wx.BitmapFromBits("\xff\x00"*8+"\x00\xff"*8,16,16)
        self.stippleBitmap.SetMask(wx.Mask(self.stippleBitmap))

        self.Bind(wx.EVT_PAINT, self.onPaint)


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


    def onPaint(self, event):
        """ Paint our tool option indicator.
        """
        dc = wx.PaintDC(self)
        dc.BeginDrawing()

        dc.SetPen(wx.BLACK_PEN)
        bgbrush = wx.Brush(wx.WHITE, wx.STIPPLE_MASK_OPAQUE)
        bgbrush.SetStipple(self.stippleBitmap)
        dc.SetTextForeground(wx.LIGHT_GREY)
        dc.SetTextBackground(wx.WHITE)
        dc.SetBrush(bgbrush)
        dc.DrawRectangle(0, 0, self.GetSize().width,self.GetSize().height)

        if self.lineSize == 0:
            dc.SetPen(wx.Pen(self.penColour, self.lineSize, wx.TRANSPARENT))
        else:
            dc.SetPen(wx.Pen(self.penColour, self.lineSize, wx.SOLID))
        dc.SetBrush(wx.Brush(self.fillColour, wx.SOLID))

        size = self.GetSize()
        ctrx = size.x/2
        ctry = size.y/2
        radius = min(size)//2 - 5
        dc.DrawCircle(ctrx, ctry, radius)

        dc.EndDrawing()

#----------------------------------------------------------------------------

class EditTextObjectDialog(wx.Dialog):
    """ Dialog box used to edit the properties of a text object.

        The user can edit the object's text, font, size, and text style.
    """

    def __init__(self, parent, title):
        """ Standard constructor.
        """
        wx.Dialog.__init__(self, parent, -1, title,
                           style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER)

        self.textCtrl = wx.TextCtrl(
            self, 1001, "Enter text here", style=wx.TE_PROCESS_ENTER|wx.TE_RICH,
            validator=TextObjectValidator()
            )
        extent = self.textCtrl.GetFullTextExtent("Hy")
        lineHeight = extent[1] + extent[3]
        self.textCtrl.SetSize(wx.Size(-1, lineHeight * 4))
        self.curFont = self.textCtrl.GetFont()
        self.curClr = wx.BLACK

        self.Bind(wx.EVT_TEXT_ENTER, self._doEnter, id=1001)

        fontBtn = wx.Button(self, -1, "Select Font...")
        self.Bind(wx.EVT_BUTTON, self.OnSelectFont, fontBtn)

        gap = wx.LEFT | wx.TOP | wx.RIGHT

        self.okButton     = wx.Button(self, wx.ID_OK,     "&OK")
        self.okButton.SetDefault()
        self.cancelButton = wx.Button(self, wx.ID_CANCEL, "&Cancel")

        btnSizer = wx.StdDialogButtonSizer()

        btnSizer.Add(self.okButton,     0, gap, 5)
        btnSizer.Add(self.cancelButton, 0, gap, 5)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.textCtrl, 1, gap | wx.EXPAND,       5)
        sizer.Add(fontBtn,    0, gap | wx.ALIGN_RIGHT, 5)
        sizer.Add((10, 10)) # Spacer.
        btnSizer.Realize()
        sizer.Add(btnSizer,      0, gap | wx.ALIGN_CENTRE, 5)

        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)

        self.textCtrl.SetFocus()


    def OnSelectFont(self, evt):
        """Shows the font dialog and sets the font of the sample text"""
        data = wx.FontData()
        data.EnableEffects(True)
        data.SetColour(self.curClr)         # set colour
        data.SetInitialFont(self.curFont)

        dlg = wx.FontDialog(self, data)
        
        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetFontData()
            font = data.GetChosenFont()
            colour = data.GetColour()

            self.curFont = font
            self.curClr = colour

            self.textCtrl.SetFont(font)
            # Update dialog for the new height of the text
            self.GetSizer().Fit(self)

        dlg.Destroy()


    def objectToDialog(self, obj):
        """ Copy the properties of the given text object into the dialog box.
        """
        self.textCtrl.SetValue(obj.getText())
        self.textCtrl.SetSelection(0, len(obj.getText()))

        self.curFont = obj.getFont()
        self.textCtrl.SetFont(self.curFont)



    def dialogToObject(self, obj):
        """ Copy the properties from the dialog box into the given text object.
        """
        obj.setText(self.textCtrl.GetValue())
        obj.setFont(self.curFont)
        obj.fitToText()

    # ======================
    # == Private Routines ==
    # ======================

    def _doEnter(self, event):
        """ Respond to the user hitting the ENTER key.

            We simulate clicking on the "OK" button.
        """
        if self.Validate(): self.Show(False)

#----------------------------------------------------------------------------

class TextObjectValidator(wx.PyValidator):
    """ This validator is used to ensure that the user has entered something
        into the text object editor dialog's text field.
    """
    def __init__(self):
        """ Standard constructor.
        """
        wx.PyValidator.__init__(self)


    def Clone(self):
        """ Standard cloner.

            Note that every validator must implement the Clone() method.
        """
        return TextObjectValidator()


    def Validate(self, win):
        """ Validate the contents of the given text control.
        """
        textCtrl = self.GetWindow()
        text = textCtrl.GetValue()

        if len(text) == 0:
            wx.MessageBox("A text object must contain some text!", "Error")
            return False
        else:
            return True


    def TransferToWindow(self):
        """ Transfer data from validator to window.

            The default implementation returns False, indicating that an error
            occurred.  We simply return True, as we don't do any data transfer.
        """
        return True # Prevent wx.Dialog from complaining.


    def TransferFromWindow(self):
        """ Transfer data from window to validator.

            The default implementation returns False, indicating that an error
            occurred.  We simply return True, as we don't do any data transfer.
        """
        return True # Prevent wx.Dialog from complaining.

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

            f = open("errors.txt", "a")
            f.write(s)
            f.close()

            if s[:9] == "Traceback":
                # Tell the user than an exception occurred.
                wx.MessageBox("An internal error has occurred.\nPlease " + \
                             "refer to the 'errors.txt' file for details.",
                             "Error", wx.OK | wx.CENTRE | wx.ICON_EXCLAMATION)


        except:
            pass # Don't recursively crash on errors.

#----------------------------------------------------------------------------

class SketchApp(wx.App):
    """ The main pySketch application object.
    """
    def OnInit(self):
        """ Initialise the application.
        """
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


