#----------------------------------------------------------------------------
# Name:         pydocview.py
# Purpose:      Python extensions to the wxWindows docview framework
#
# Author:       Peter Yared, Morgan Hua, Matt Fryer
#
# Created:      5/15/03
# CVS-ID:       $Id$
# Copyright:    (c) 2003-2006 ActiveGrid, Inc.
# License:      wxWindows license
#----------------------------------------------------------------------------


import wx
import wx.lib.docview
import sys
import getopt
from wx.lib.rcsizer import RowColSizer
import os
import os.path
import time
import string
import pickle
import tempfile
import mmap
_ = wx.GetTranslation
if wx.Platform == '__WXMSW__':
    _WINDOWS = True
else:
    _WINDOWS = False

#----------------------------------------------------------------------------
# Constants
#----------------------------------------------------------------------------

VIEW_TOOLBAR_ID = wx.NewId()
VIEW_STATUSBAR_ID = wx.NewId()

EMBEDDED_WINDOW_TOP = 1
EMBEDDED_WINDOW_BOTTOM = 2
EMBEDDED_WINDOW_LEFT = 4
EMBEDDED_WINDOW_RIGHT = 8
EMBEDDED_WINDOW_TOPLEFT = 16
EMBEDDED_WINDOW_BOTTOMLEFT = 32
EMBEDDED_WINDOW_TOPRIGHT = 64
EMBEDDED_WINDOW_BOTTOMRIGHT = 128
EMBEDDED_WINDOW_ALL = EMBEDDED_WINDOW_TOP | EMBEDDED_WINDOW_BOTTOM | EMBEDDED_WINDOW_LEFT | EMBEDDED_WINDOW_RIGHT | \
                      EMBEDDED_WINDOW_TOPLEFT | EMBEDDED_WINDOW_BOTTOMLEFT | EMBEDDED_WINDOW_TOPRIGHT | EMBEDDED_WINDOW_BOTTOMRIGHT

SAVEALL_ID = wx.NewId()

WINDOW_MENU_NUM_ITEMS = 9


class DocFrameMixIn:
    """
    Class with common code used by DocMDIParentFrame, DocTabbedParentFrame, and
    DocSDIFrame.
    """


    def GetDocumentManager(self):
        """
        Returns the document manager associated with the DocMDIParentFrame.
        """
        return self._docManager


    def InitializePrintData(self):
        """
        Initializes the PrintData that is used when printing.
        """
        self._printData = wx.PrintData()
        self._printData.SetPaperId(wx.PAPER_LETTER)


    def CreateDefaultMenuBar(self, sdi=False):
        """
        Creates the default MenuBar.  Contains File, Edit, View, Tools, and Help menus.
        """
        menuBar = wx.MenuBar()

        fileMenu = wx.Menu()
        fileMenu.Append(wx.ID_NEW, _("&New...\tCtrl+N"), _("Creates a new document"))
        fileMenu.Append(wx.ID_OPEN, _("&Open...\tCtrl+O"), _("Opens an existing document"))
        fileMenu.Append(wx.ID_CLOSE, _("&Close"), _("Closes the active document"))
        if not sdi:
            fileMenu.Append(wx.ID_CLOSE_ALL, _("Close A&ll"), _("Closes all open documents"))
        fileMenu.AppendSeparator()
        fileMenu.Append(wx.ID_SAVE, _("&Save\tCtrl+S"), _("Saves the active document"))
        fileMenu.Append(wx.ID_SAVEAS, _("Save &As..."), _("Saves the active document with a new name"))
        fileMenu.Append(SAVEALL_ID, _("Save All\tCtrl+Shift+A"), _("Saves the all active documents"))
        wx.EVT_MENU(self, SAVEALL_ID, self.ProcessEvent)
        wx.EVT_UPDATE_UI(self, SAVEALL_ID, self.ProcessUpdateUIEvent)
        fileMenu.AppendSeparator()
        fileMenu.Append(wx.ID_PRINT, _("&Print\tCtrl+P"), _("Prints the active document"))
        fileMenu.Append(wx.ID_PREVIEW, _("Print Pre&view"), _("Displays full pages"))
        fileMenu.Append(wx.ID_PRINT_SETUP, _("Page Set&up"), _("Changes page layout settings"))
        fileMenu.AppendSeparator()
        if wx.Platform == '__WXMAC__':
            fileMenu.Append(wx.ID_EXIT, _("&Quit"), _("Closes this program"))
        else:
            fileMenu.Append(wx.ID_EXIT, _("E&xit"), _("Closes this program"))
        self._docManager.FileHistoryUseMenu(fileMenu)
        self._docManager.FileHistoryAddFilesToMenu()
        menuBar.Append(fileMenu, _("&File"));

        editMenu = wx.Menu()
        editMenu.Append(wx.ID_UNDO, _("&Undo\tCtrl+Z"), _("Reverses the last action"))
        editMenu.Append(wx.ID_REDO, _("&Redo\tCtrl+Y"), _("Reverses the last undo"))
        editMenu.AppendSeparator()
        #item = wxMenuItem(self.editMenu, wxID_CUT, _("Cu&t\tCtrl+X"), _("Cuts the selection and puts it on the Clipboard"))
        #item.SetBitmap(getCutBitmap())
        #editMenu.AppendItem(item)
        editMenu.Append(wx.ID_CUT, _("Cu&t\tCtrl+X"), _("Cuts the selection and puts it on the Clipboard"))
        wx.EVT_MENU(self, wx.ID_CUT, self.ProcessEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_CUT, self.ProcessUpdateUIEvent)
        editMenu.Append(wx.ID_COPY, _("&Copy\tCtrl+C"), _("Copies the selection and puts it on the Clipboard"))
        wx.EVT_MENU(self, wx.ID_COPY, self.ProcessEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_COPY, self.ProcessUpdateUIEvent)
        editMenu.Append(wx.ID_PASTE, _("&Paste\tCtrl+V"), _("Inserts Clipboard contents"))
        wx.EVT_MENU(self, wx.ID_PASTE, self.ProcessEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PASTE, self.ProcessUpdateUIEvent)
        editMenu.Append(wx.ID_CLEAR, _("&Delete"), _("Erases the selection"))
        wx.EVT_MENU(self, wx.ID_CLEAR, self.ProcessEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_CLEAR, self.ProcessUpdateUIEvent)
        editMenu.AppendSeparator()
        editMenu.Append(wx.ID_SELECTALL, _("Select A&ll\tCtrl+A"), _("Selects all available data"))
        wx.EVT_MENU(self, wx.ID_SELECTALL, self.ProcessEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_SELECTALL, self.ProcessUpdateUIEvent)
        menuBar.Append(editMenu, _("&Edit"))
        if sdi:
            if self.GetDocument() and self.GetDocument().GetCommandProcessor():
                self.GetDocument().GetCommandProcessor().SetEditMenu(editMenu)

        viewMenu = wx.Menu()
        viewMenu.AppendCheckItem(VIEW_TOOLBAR_ID, _("&Toolbar"), _("Shows or hides the toolbar"))
        wx.EVT_MENU(self, VIEW_TOOLBAR_ID, self.OnViewToolBar)
        wx.EVT_UPDATE_UI(self, VIEW_TOOLBAR_ID, self.OnUpdateViewToolBar)
        viewMenu.AppendCheckItem(VIEW_STATUSBAR_ID, _("&Status Bar"), _("Shows or hides the status bar"))
        wx.EVT_MENU(self, VIEW_STATUSBAR_ID, self.OnViewStatusBar)
        wx.EVT_UPDATE_UI(self, VIEW_STATUSBAR_ID, self.OnUpdateViewStatusBar)
        menuBar.Append(viewMenu, _("&View"))

        helpMenu = wx.Menu()
        helpMenu.Append(wx.ID_ABOUT, _("&About" + " " + wx.GetApp().GetAppName()), _("Displays program information, version number, and copyright"))
        menuBar.Append(helpMenu, _("&Help"))

        wx.EVT_MENU(self, wx.ID_ABOUT, self.OnAbout)
        wx.EVT_UPDATE_UI(self, wx.ID_ABOUT, self.ProcessUpdateUIEvent)  # Using ID_ABOUT to update the window menu, the window menu items are not triggering

        if sdi:  # TODO: Is this really needed?
            wx.EVT_COMMAND_FIND_CLOSE(self, -1, self.ProcessEvent)

        return menuBar


    def CreateDefaultStatusBar(self):
        """
        Creates the default StatusBar.
        """
        wx.Frame.CreateStatusBar(self)
        self.GetStatusBar().Show(wx.ConfigBase_Get().ReadInt("ViewStatusBar", True))
        self.UpdateStatus()
        return self.GetStatusBar()


    def CreateDefaultToolBar(self):
        """
        Creates the default ToolBar.
        """
        self._toolBar = self.CreateToolBar(wx.TB_HORIZONTAL | wx.NO_BORDER | wx.TB_FLAT)
        self._toolBar.AddSimpleTool(wx.ID_NEW, getNewBitmap(), _("New"), _("Creates a new document"))
        self._toolBar.AddSimpleTool(wx.ID_OPEN, getOpenBitmap(), _("Open"), _("Opens an existing document"))
        self._toolBar.AddSimpleTool(wx.ID_SAVE, getSaveBitmap(), _("Save"), _("Saves the active document"))
        self._toolBar.AddSimpleTool(SAVEALL_ID, getSaveAllBitmap(), _("Save All"), _("Saves all the active documents"))
        self._toolBar.AddSeparator()
        self._toolBar.AddSimpleTool(wx.ID_PRINT, getPrintBitmap(), _("Print"), _("Displays full pages"))
        self._toolBar.AddSimpleTool(wx.ID_PREVIEW, getPrintPreviewBitmap(), _("Print Preview"), _("Prints the active document"))
        self._toolBar.AddSeparator()
        self._toolBar.AddSimpleTool(wx.ID_CUT, getCutBitmap(), _("Cut"), _("Cuts the selection and puts it on the Clipboard"))
        self._toolBar.AddSimpleTool(wx.ID_COPY, getCopyBitmap(), _("Copy"), _("Copies the selection and puts it on the Clipboard"))
        self._toolBar.AddSimpleTool(wx.ID_PASTE, getPasteBitmap(), _("Paste"), _("Inserts Clipboard contents"))
        self._toolBar.AddSimpleTool(wx.ID_UNDO, getUndoBitmap(), _("Undo"), _("Reverses the last action"))
        self._toolBar.AddSimpleTool(wx.ID_REDO, getRedoBitmap(), _("Redo"), _("Reverses the last undo"))
        self._toolBar.Realize()
        self._toolBar.Show(wx.ConfigBase_Get().ReadInt("ViewToolBar", True))

        return self._toolBar


    def OnFileSaveAll(self, event):
        """
        Saves all of the currently open documents.
        """
        docs = wx.GetApp().GetDocumentManager().GetDocuments()

        # save child documents first
        for doc in docs:
            if isinstance(doc, wx.lib.pydocview.ChildDocument):
                doc.Save()

        # save parent and other documents later
        for doc in docs:
            if not isinstance(doc, wx.lib.pydocview.ChildDocument):
                doc.Save()


    def OnAbout(self, event):
        """
        Invokes the about dialog.
        """
        aboutService = wx.GetApp().GetService(AboutService)
        if aboutService:
            aboutService.ShowAbout()


    def OnViewToolBar(self, event):
        """
        Toggles whether the ToolBar is visible.
        """
        self._toolBar.Show(not self._toolBar.IsShown())
        self._LayoutFrame()


    def OnUpdateViewToolBar(self, event):
        """
        Updates the View ToolBar menu item.
        """
        event.Check(self.GetToolBar().IsShown())


    def OnViewStatusBar(self, event):
        """
        Toggles whether the StatusBar is visible.
        """
        self.GetStatusBar().Show(not self.GetStatusBar().IsShown())
        self._LayoutFrame()


    def OnUpdateViewStatusBar(self, event):
        """
        Updates the View StatusBar menu item.
        """
        event.Check(self.GetStatusBar().IsShown())


    def UpdateStatus(self, message = _("Ready")):
        """
        Updates the StatusBar.
        """
        # wxBug: Menubar and toolbar help strings don't pop the status text back
        if self.GetStatusBar().GetStatusText() != message:
            self.GetStatusBar().PushStatusText(message)


class DocMDIParentFrameMixIn:
    """
    Class with common code used by DocMDIParentFrame and DocTabbedParentFrame.
    """


    def _GetPosSizeFromConfig(self, pos, size):
        """
        Adjusts the position and size of the frame using the saved config position and size.
        """
        config = wx.ConfigBase_Get()
        if pos == wx.DefaultPosition and size == wx.DefaultSize and config.ReadInt("MDIFrameMaximized", False):
            pos = [0, 0]
            size = wx.DisplaySize()
            # wxBug: Need to set to fill screen to get around bug where maximize is leaving shadow of statusbar, check out maximize call at end of this function
        else:
            if pos == wx.DefaultPosition:
                pos = config.ReadInt("MDIFrameXLoc", -1), config.ReadInt("MDIFrameYLoc", -1)

            if wx.Display_GetFromPoint(pos) == -1:  # Check if the frame position is offscreen
                pos = wx.DefaultPosition

            if size == wx.DefaultSize:
                size = wx.Size(config.ReadInt("MDIFrameXSize", 450), config.ReadInt("MDIFrameYSize", 300))
        return pos, size


    def _InitFrame(self, embeddedWindows, minSize):
        """
        Initializes the frame and creates the default menubar, toolbar, and status bar.
        """
        self._embeddedWindows = []
        self.SetDropTarget(_DocFrameFileDropTarget(self._docManager, self))

        if wx.GetApp().GetDefaultIcon():
            self.SetIcon(wx.GetApp().GetDefaultIcon())

        wx.EVT_MENU(self, wx.ID_ABOUT, self.OnAbout)
        wx.EVT_SIZE(self, self.OnSize)

        self.InitializePrintData()

        toolBar = self.CreateDefaultToolBar()
        self.SetToolBar(toolBar)
        menuBar = self.CreateDefaultMenuBar()
        statusBar = self.CreateDefaultStatusBar()

        config = wx.ConfigBase_Get()
        if config.ReadInt("MDIFrameMaximized", False):
            # wxBug: On maximize, statusbar leaves a residual that needs to be refereshed, happens even when user does it
            self.Maximize()

        self.CreateEmbeddedWindows(embeddedWindows, minSize)
        self._LayoutFrame()

        if wx.Platform == '__WXMAC__':
            self.SetMenuBar(menuBar)  # wxBug: Have to set the menubar at the very end or the automatic MDI "window" menu doesn't get put in the right place when the services add new menus to the menubar

        wx.GetApp().SetTopWindow(self)  # Need to do this here in case the services are looking for wx.GetApp().GetTopWindow()
        for service in wx.GetApp().GetServices():
            service.InstallControls(self, menuBar = menuBar, toolBar = toolBar, statusBar = statusBar)
            if hasattr(service, "ShowWindow"):
                service.ShowWindow()  # instantiate service windows for correct positioning, we'll hide/show them later based on user preference

        if wx.Platform != '__WXMAC__':
            self.SetMenuBar(menuBar)  # wxBug: Have to set the menubar at the very end or the automatic MDI "window" menu doesn't get put in the right place when the services add new menus to the menubar


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        id = event.GetId()
        if id == SAVEALL_ID:
            self.OnFileSaveAll(event)
            return True

        return wx.GetApp().ProcessEvent(event)


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        id = event.GetId()
        if id == wx.ID_CUT:
            event.Enable(False)
            return True
        elif id == wx.ID_COPY:
            event.Enable(False)
            return True
        elif id == wx.ID_PASTE:
            event.Enable(False)
            return True
        elif id == wx.ID_CLEAR:
            event.Enable(False)
            return True
        elif id == wx.ID_SELECTALL:
            event.Enable(False)
            return True
        elif id == SAVEALL_ID:
            filesModified = False
            docs = wx.GetApp().GetDocumentManager().GetDocuments()
            for doc in docs:
                if doc.IsModified():
                    filesModified = True
                    break

            event.Enable(filesModified)
            return True
        else:
            return wx.GetApp().ProcessUpdateUIEvent(event)


    def CreateEmbeddedWindows(self, windows=0, minSize=20):
        """
        Create the specified embedded windows around the edges of the frame.
        """
        frameSize = self.GetSize()   # TODO: GetClientWindow.GetSize is still returning 0,0 since the frame isn't fully constructed yet, so using full frame size
        defaultHSize = max(minSize, int(frameSize[0] / 6))
        defaultVSize = max(minSize, int(frameSize[1] / 7))
        defaultSubVSize = int(frameSize[1] / 2)
        config = wx.ConfigBase_Get()
        if windows & (EMBEDDED_WINDOW_LEFT | EMBEDDED_WINDOW_TOPLEFT | EMBEDDED_WINDOW_BOTTOMLEFT):
            self._leftEmbWindow = self._CreateEmbeddedWindow(self, (max(minSize,config.ReadInt("MDIEmbedLeftSize", defaultHSize)), -1), wx.LAYOUT_VERTICAL, wx.LAYOUT_LEFT, visible = config.ReadInt("MDIEmbedLeftVisible", 1), sash = wx.SASH_RIGHT)
        else:
            self._leftEmbWindow = None
        if windows & EMBEDDED_WINDOW_TOPLEFT:
            self._topLeftEmbWindow = self._CreateEmbeddedWindow(self._leftEmbWindow, (-1, config.ReadInt("MDIEmbedTopLeftSize", defaultSubVSize)), wx.LAYOUT_HORIZONTAL, wx.LAYOUT_TOP, visible = config.ReadInt("MDIEmbedTopLeftVisible", 1), sash = wx.SASH_BOTTOM)
        else:
            self._topLeftEmbWindow = None
        if windows & EMBEDDED_WINDOW_BOTTOMLEFT:
            self._bottomLeftEmbWindow = self._CreateEmbeddedWindow(self._leftEmbWindow, (-1, config.ReadInt("MDIEmbedBottomLeftSize", defaultSubVSize)), wx.LAYOUT_HORIZONTAL, wx.LAYOUT_BOTTOM, visible = config.ReadInt("MDIEmbedBottomLeftVisible", 1))
        else:
            self._bottomLeftEmbWindow = None
        if windows & (EMBEDDED_WINDOW_RIGHT | EMBEDDED_WINDOW_TOPRIGHT | EMBEDDED_WINDOW_BOTTOMRIGHT):
            self._rightEmbWindow = self._CreateEmbeddedWindow(self, (max(minSize,config.ReadInt("MDIEmbedRightSize", defaultHSize)), -1), wx.LAYOUT_VERTICAL, wx.LAYOUT_RIGHT, visible = config.ReadInt("MDIEmbedRightVisible", 1), sash = wx.SASH_LEFT)
        else:
            self._rightEmbWindow = None
        if windows & EMBEDDED_WINDOW_TOPRIGHT:
            self._topRightEmbWindow = self._CreateEmbeddedWindow(self._rightEmbWindow, (-1, config.ReadInt("MDIEmbedTopRightSize", defaultSubVSize)), wx.LAYOUT_HORIZONTAL, wx.LAYOUT_TOP, visible = config.ReadInt("MDIEmbedTopRightVisible", 1), sash = wx.SASH_BOTTOM)
        else:
            self._topRightEmbWindow = None
        if windows & EMBEDDED_WINDOW_BOTTOMRIGHT:
            self._bottomRightEmbWindow = self._CreateEmbeddedWindow(self._rightEmbWindow, (-1, config.ReadInt("MDIEmbedBottomRightSize", defaultSubVSize)), wx.LAYOUT_HORIZONTAL, wx.LAYOUT_BOTTOM, visible = config.ReadInt("MDIEmbedBottomRightVisible", 1))
        else:
            self._bottomRightEmbWindow = None
        if windows & EMBEDDED_WINDOW_TOP:
            self._topEmbWindow = self._CreateEmbeddedWindow(self, (-1, max(minSize,config.ReadInt("MDIEmbedTopSize", defaultVSize))), wx.LAYOUT_HORIZONTAL, wx.LAYOUT_TOP, visible = config.ReadInt("MDIEmbedTopVisible", 1), sash = wx.SASH_BOTTOM)
        else:
            self._topEmbWindow = None
        if windows & EMBEDDED_WINDOW_BOTTOM:
            self._bottomEmbWindow = self._CreateEmbeddedWindow(self, (-1, max(minSize,config.ReadInt("MDIEmbedBottomSize", defaultVSize))), wx.LAYOUT_HORIZONTAL, wx.LAYOUT_BOTTOM, visible = config.ReadInt("MDIEmbedBottomVisible", 1), sash = wx.SASH_TOP)
        else:
            self._bottomEmbWindow = None


    def SaveEmbeddedWindowSizes(self):
        """
        Saves the sizes of the embedded windows.
        """
        config = wx.ConfigBase_Get()
        if not self.IsMaximized():
            config.WriteInt("MDIFrameXLoc", self.GetPositionTuple()[0])
            config.WriteInt("MDIFrameYLoc", self.GetPositionTuple()[1])
            config.WriteInt("MDIFrameXSize", self.GetSizeTuple()[0])
            config.WriteInt("MDIFrameYSize", self.GetSizeTuple()[1])
        config.WriteInt("MDIFrameMaximized", self.IsMaximized())
        config.WriteInt("ViewToolBar", self._toolBar.IsShown())
        config.WriteInt("ViewStatusBar", self.GetStatusBar().IsShown())

        if self._leftEmbWindow:
            config.WriteInt("MDIEmbedLeftSize", self._leftEmbWindow.GetSize()[0])
            config.WriteInt("MDIEmbedLeftVisible", self._leftEmbWindow.IsShown())
        if self._topLeftEmbWindow:
            if self._topLeftEmbWindow._sizeBeforeHidden:
                size = self._topLeftEmbWindow._sizeBeforeHidden[1]
            else:
                size = self._topLeftEmbWindow.GetSize()[1]
            config.WriteInt("MDIEmbedTopLeftSize", size)
            config.WriteInt("MDIEmbedTopLeftVisible", self._topLeftEmbWindow.IsShown())
        if self._bottomLeftEmbWindow:
            if self._bottomLeftEmbWindow._sizeBeforeHidden:
                size = self._bottomLeftEmbWindow._sizeBeforeHidden[1]
            else:
                size = self._bottomLeftEmbWindow.GetSize()[1]
            config.WriteInt("MDIEmbedBottomLeftSize", size)
            config.WriteInt("MDIEmbedBottomLeftVisible", self._bottomLeftEmbWindow.IsShown())
        if self._rightEmbWindow:
            config.WriteInt("MDIEmbedRightSize", self._rightEmbWindow.GetSize()[0])
            config.WriteInt("MDIEmbedRightVisible", self._rightEmbWindow.IsShown())
        if self._topRightEmbWindow:
            if self._topRightEmbWindow._sizeBeforeHidden:
                size = self._topRightEmbWindow._sizeBeforeHidden[1]
            else:
                size = self._topRightEmbWindow.GetSize()[1]
            config.WriteInt("MDIEmbedTopRightSize", size)
            config.WriteInt("MDIEmbedTopRightVisible", self._topRightEmbWindow.IsShown())
        if self._bottomRightEmbWindow:
            if self._bottomRightEmbWindow._sizeBeforeHidden:
                size = self._bottomRightEmbWindow._sizeBeforeHidden[1]
            else:
                size = self._bottomRightEmbWindow.GetSize()[1]
            config.WriteInt("MDIEmbedBottomRightSize", size)
            config.WriteInt("MDIEmbedBottomRightVisible", self._bottomRightEmbWindow.IsShown())
        if self._topEmbWindow:
            config.WriteInt("MDIEmbedTopSize", self._topEmbWindow.GetSize()[1])
            config.WriteInt("MDIEmbedTopVisible", self._topEmbWindow.IsShown())
        if self._bottomEmbWindow:
            config.WriteInt("MDIEmbedBottomSize", self._bottomEmbWindow.GetSize()[1])
            config.WriteInt("MDIEmbedBottomVisible", self._bottomEmbWindow.IsShown())


    def GetEmbeddedWindow(self, loc):
        """
        Returns the instance of the embedded window specified by the embedded window location constant.
        """
        if loc == EMBEDDED_WINDOW_TOP:
            return self._topEmbWindow
        elif loc == EMBEDDED_WINDOW_BOTTOM:
            return self._bottomEmbWindow
        elif loc == EMBEDDED_WINDOW_LEFT:
            return self._leftEmbWindow
        elif loc == EMBEDDED_WINDOW_RIGHT:
            return self._rightEmbWindow
        elif loc == EMBEDDED_WINDOW_TOPLEFT:
            return self._topLeftEmbWindow
        elif loc == EMBEDDED_WINDOW_BOTTOMLEFT:
            return self._bottomLeftEmbWindow
        elif loc == EMBEDDED_WINDOW_TOPRIGHT:
            return self._topRightEmbWindow
        elif loc == EMBEDDED_WINDOW_BOTTOMRIGHT:
            return self._bottomRightEmbWindow
        return None


    def _CreateEmbeddedWindow(self, parent, size, orientation, alignment, visible=True, sash=None):
        """
        Creates the embedded window with the specified size, orientation, and alignment.  If the
        window is not visible it will retain the size with which it was last viewed.
        """
        window = wx.SashLayoutWindow(parent, wx.NewId(), style = wx.NO_BORDER | wx.SW_3D)
        window.SetDefaultSize(size)
        window.SetOrientation(orientation)
        window.SetAlignment(alignment)
        if sash != None:  # wx.SASH_TOP is 0 so check for None instead of just doing "if sash:"
            window.SetSashVisible(sash, True)
        ####
        def OnEmbeddedWindowSashDrag(event):
            if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE:
                return
            sashWindow = event.GetEventObject()
            if sashWindow.GetAlignment() == wx.LAYOUT_TOP or sashWindow.GetAlignment() == wx.LAYOUT_BOTTOM:
                size = wx.Size(-1, event.GetDragRect().height)
            else:
                size = wx.Size(event.GetDragRect().width, -1)
            event.GetEventObject().SetDefaultSize(size)
            self._LayoutFrame()
            sashWindow.Refresh()
            if isinstance(sashWindow.GetParent(), wx.SashLayoutWindow):
                sashWindow.Show()
                parentSashWindow = sashWindow.GetParent()  # Force a refresh
                parentSashWindow.Layout()
                parentSashWindow.Refresh()
                parentSashWindow.SetSize((parentSashWindow.GetSize().width + 1, parentSashWindow.GetSize().height + 1))
        ####
        wx.EVT_SASH_DRAGGED(window, window.GetId(), OnEmbeddedWindowSashDrag)
        window._sizeBeforeHidden = None
        if not visible:
            window.Show(False)
            if isinstance(parent, wx.SashLayoutWindow): # It's a window embedded in another sash window so remember its actual size to show it again
                window._sizeBeforeHidden = size
        return window


    def ShowEmbeddedWindow(self, window, show=True):
        """
        Shows or hides the embedded window specified by the embedded window location constant.
        """
        window.Show(show)
        if isinstance(window.GetParent(), wx.SashLayoutWindow):  # It is a parent sashwindow with multiple embedded sashwindows
            parentSashWindow = window.GetParent()
            if show:  # Make sure it is visible in case all of the subwindows were hidden
                parentSashWindow.Show()
            if show and window._sizeBeforeHidden:
                if window._sizeBeforeHidden[1] == parentSashWindow.GetClientSize()[1]:
                    if window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMLEFT) and self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPLEFT).IsShown():
                        window.SetDefaultSize((window._sizeBeforeHidden[0], window._sizeBeforeHidden[0] - self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPLEFT).GetSize()[1]))
                    elif window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPLEFT) and self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMLEFT).IsShown():
                        window.SetDefaultSize((window._sizeBeforeHidden[0], window._sizeBeforeHidden[0] - self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMLEFT).GetSize()[1]))
                    elif window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMRIGHT) and self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPRIGHT).IsShown():
                        window.SetDefaultSize((window._sizeBeforeHidden[0], window._sizeBeforeHidden[0] - self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPRIGHT).GetSize()[1]))
                    elif window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPRIGHT) and self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMRIGHT).IsShown():
                        window.SetDefaultSize((window._sizeBeforeHidden[0], window._sizeBeforeHidden[0] - self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMRIGHT).GetSize()[1]))
                else:
                    window.SetDefaultSize(window._sizeBeforeHidden)
                    # If it is not the size of the full parent sashwindow set the other window's size so that if it gets shown it will have a cooresponding size
                    if window._sizeBeforeHidden[1] < parentSashWindow.GetClientSize()[1]:
                        otherWindowSize = (-1, parentSashWindow.GetClientSize()[1] - window._sizeBeforeHidden[1])
                        if window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMLEFT):
                            self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPLEFT).SetDefaultSize(otherWindowSize)
                        elif window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPLEFT):
                            self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMLEFT).SetDefaultSize(otherWindowSize)
                        elif window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMRIGHT):
                            self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPRIGHT).SetDefaultSize(otherWindowSize)
                        elif window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPRIGHT):
                            self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMRIGHT).SetDefaultSize(otherWindowSize)

            if not show:
                if window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMRIGHT) and not self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPRIGHT).IsShown() \
                    or window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPRIGHT) and not self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMRIGHT).IsShown() \
                    or window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMLEFT) and not self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPLEFT).IsShown() \
                    or window == self.GetEmbeddedWindow(EMBEDDED_WINDOW_TOPLEFT) and not self.GetEmbeddedWindow(EMBEDDED_WINDOW_BOTTOMLEFT).IsShown():
                    parentSashWindow.Hide()  # Hide the parent sashwindow if all of the children are hidden
            parentSashWindow.Layout()   # Force a refresh
            parentSashWindow.Refresh()
            parentSashWindow.SetSize((parentSashWindow.GetSize().width + 1, parentSashWindow.GetSize().height + 1))
        self._LayoutFrame()


    def HideEmbeddedWindow(self, window):
        """
        Hides the embedded window specified by the embedded window location constant.
        """
        self.ShowEmbeddedWindow(window, show=False)


class DocTabbedChildFrame(wx.Panel):
    """
    The wxDocMDIChildFrame class provides a default frame for displaying
    documents on separate windows. This class can only be used for MDI child
    frames.

    The class is part of the document/view framework supported by wxWindows,
    and cooperates with the wxView, wxDocument, wxDocManager and wxDocTemplate
    classes.
    """


    def __init__(self, doc, view, frame, id, title, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE, name="frame"):
        """
        Constructor.  Note that the event table must be rebuilt for the
        frame since the EvtHandler is not virtual.
        """
        wx.Panel.__init__(self, frame.GetNotebook(), id)
        self._childDocument = doc
        self._childView = view
        frame.AddNotebookPage(self, doc.GetPrintableName())
        if view:
            view.SetFrame(self)


    def GetIcon(self):
        """
        Dummy method since the icon of tabbed frames are managed by the notebook.
        """
        return None


    def SetIcon(self, icon):
        """
        Dummy method since the icon of tabbed frames are managed by the notebook.
        """
        pass


    def Destroy(self):
        """
        Removes the current notebook page.
        """
        wx.GetApp().GetTopWindow().RemoveNotebookPage(self)


    def SetFocus(self):
        """
        Activates the current notebook page.
        """
        wx.GetApp().GetTopWindow().ActivateNotebookPage(self)


    def Activate(self):  # Need this in case there are embedded sash windows and such, OnActivate is not getting called
        """
        Activates the current view.
        """
        # Called by Project Editor
        if self._childView:
            self._childView.Activate(True)


    def GetTitle(self):
        """
        Returns the frame's title.
        """
        return wx.GetApp().GetTopWindow().GetNotebookPageTitle(self)


    def SetTitle(self, title):
        """
        Sets the frame's title.
        """
        wx.GetApp().GetTopWindow().SetNotebookPageTitle(self, title)


    def OnTitleIsModified(self):
        """
        Add/remove to the frame's title an indication that the document is dirty.
        If the document is dirty, an '*' is appended to the title
        """
        title = self.GetTitle()
        if title:
            if self.GetDocument().IsModified():
                if not title.endswith("*"):
                    title = title + "*"
                    self.SetTitle(title)
            else:
                if title.endswith("*"):
                    title = title[:-1]
                    self.SetTitle(title)
        

    def ProcessEvent(event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if not self._childView or not self._childView.ProcessEvent(event):
            if not isinstance(event, wx.CommandEvent) or not self.GetParent() or not self.GetParent().ProcessEvent(event):
                return False
            else:
                return True
        else:
            return True


    def GetDocument(self):
        """
        Returns the document associated with this frame.
        """
        return self._childDocument


    def SetDocument(self, document):
        """
        Sets the document for this frame.
        """
        self._childDocument = document


    def GetView(self):
        """
        Returns the view associated with this frame.
        """
        return self._childView


    def SetView(self, view):
        """
        Sets the view for this frame.
        """
        self._childView = view


class DocTabbedParentFrame(wx.Frame, DocFrameMixIn, DocMDIParentFrameMixIn):
    """
    The DocTabbedParentFrame class provides a default top-level frame for
    applications using the document/view framework. This class can only be
    used for MDI parent frames that use a tabbed interface.

    It cooperates with the wxView, wxDocument, wxDocManager and wxDocTemplate
    classes.
    """


    def __init__(self, docManager, frame, id, title, pos = wx.DefaultPosition, size = wx.DefaultSize, style = wx.DEFAULT_FRAME_STYLE, name = "DocTabbedParentFrame", embeddedWindows = 0, minSize=20):
        """
        Constructor.  Note that the event table must be rebuilt for the
        frame since the EvtHandler is not virtual.
        """
        pos, size = self._GetPosSizeFromConfig(pos, size)
        wx.Frame.__init__(self, frame, id, title, pos, size, style, name)

        # From docview.MDIParentFrame
        self._docManager = docManager

        wx.EVT_CLOSE(self, self.OnCloseWindow)

        wx.EVT_MENU(self, wx.ID_EXIT, self.OnExit)
        wx.EVT_MENU_RANGE(self, wx.ID_FILE1, wx.ID_FILE9, self.OnMRUFile)

        wx.EVT_MENU(self, wx.ID_NEW, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_OPEN, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_CLOSE_ALL, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_CLOSE, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_REVERT, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_SAVE, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_SAVEAS, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_UNDO, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_REDO, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PRINT, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PRINT_SETUP, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PREVIEW, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_ABOUT, self.OnAbout)

        wx.EVT_UPDATE_UI(self, wx.ID_NEW, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_OPEN, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE_ALL, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_REVERT, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVE, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVEAS, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_UNDO, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_REDO, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT_SETUP, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PREVIEW, self.ProcessUpdateUIEvent)
        # End From docview.MDIParentFrame

        self.CreateNotebook()
        self._InitFrame(embeddedWindows, minSize)


    def _LayoutFrame(self):
        """
        Lays out the frame.
        """
        wx.LayoutAlgorithm().LayoutFrame(self, self._notebook)


    def CreateNotebook(self):
        """
        Creates the notebook to use for the tabbed document interface.
        """
        if wx.Platform != "__WXMAC__":
            self._notebook = wx.Notebook(self, wx.NewId())
        else:
            self._notebook = wx.Listbook(self, wx.NewId(), style=wx.LB_LEFT)
        # self._notebook.SetSizer(wx.NotebookSizer(self._notebook))
        if wx.Platform != "__WXMAC__":
            wx.EVT_NOTEBOOK_PAGE_CHANGED(self, self._notebook.GetId(), self.OnNotebookPageChanged)
        else:
            wx.EVT_LISTBOOK_PAGE_CHANGED(self, self._notebook.GetId(), self.OnNotebookPageChanged)
        wx.EVT_RIGHT_DOWN(self._notebook, self.OnNotebookRightClick)
        wx.EVT_MIDDLE_DOWN(self._notebook, self.OnNotebookMiddleClick)

        # wxBug: wx.Listbook does not implement HitTest the same way wx.Notebook
        # does, so for now don't fire MouseOver events.
        if wx.Platform != "__WXMAC__":
            wx.EVT_MOTION(self._notebook, self.OnNotebookMouseOver)

        templates = wx.GetApp().GetDocumentManager().GetTemplates()
        iconList = wx.ImageList(16, 16, initialCount = len(templates))
        self._iconIndexLookup = []
        for template in templates:
            icon = template.GetIcon()
            if icon:
                if icon.GetHeight() != 16 or icon.GetWidth() != 16:
                    icon.SetHeight(16)
                    icon.SetWidth(16)
                    if wx.GetApp().GetDebug():
                        print "Warning: icon for '%s' isn't 16x16, not crossplatform" % template._docTypeName
                iconIndex = iconList.AddIcon(icon)
                self._iconIndexLookup.append((template, iconIndex))

        icon = getBlankIcon()
        if icon.GetHeight() != 16 or icon.GetWidth() != 16:
            icon.SetHeight(16)
            icon.SetWidth(16)
            if wx.GetApp().GetDebug():
                print "Warning: getBlankIcon isn't 16x16, not crossplatform"
        self._blankIconIndex = iconList.AddIcon(icon)
        self._notebook.AssignImageList(iconList)


    def GetNotebook(self):
        """
        Returns the notebook used by the tabbed document interface.
        """
        return self._notebook


    def GetActiveChild(self):
        """
        Returns the active notebook page, which to the framework is treated as
        a document frame.
        """
        index = self._notebook.GetSelection()
        if index == -1:
            return None
        return self._notebook.GetPage(index)


    def OnNotebookPageChanged(self, event):
        """
        Activates a notebook page's view when it is selected.
        """
        index = self._notebook.GetSelection()
        if index > -1:
            self._notebook.GetPage(index).GetView().Activate()


    def OnNotebookMouseOver(self, event):
        # wxBug: On Windows XP the tooltips don't automatically disappear when you move the mouse and it is on a notebook tab, has nothing to do with this code!!!
        index, type = self._notebook.HitTest(event.GetPosition())

        if index > -1:
            doc = self._notebook.GetPage(index).GetView().GetDocument()
            # wxBug: Tooltips no longer appearing on tabs except on
            # about a 2 pixel area between tab top and contents that will show tip.
            self._notebook.GetParent().SetToolTip(wx.ToolTip(doc.GetFilename()))
        else:
            self._notebook.SetToolTip(wx.ToolTip(""))
        event.Skip()


    def OnNotebookMiddleClick(self, event):
        """
        Handles middle clicks for the notebook, closing the document whose tab was
        clicked on.
        """
        index, type = self._notebook.HitTest(event.GetPosition())
        if index > -1:
            doc = self._notebook.GetPage(index).GetView().GetDocument()
            if doc:
                doc.DeleteAllViews()

    def OnNotebookRightClick(self, event):
        """
        Handles right clicks for the notebook, enabling users to either close
        a tab or select from the available documents if the user clicks on the
        notebook's white space.
        """
        index, type = self._notebook.HitTest(event.GetPosition())
        menu = wx.Menu()
        x, y = event.GetX(), event.GetY()
        if index > -1:
            doc = self._notebook.GetPage(index).GetView().GetDocument()
            id = wx.NewId()
            menu.Append(id, _("Close"))
            def OnRightMenuSelect(event):
                doc.DeleteAllViews()
            wx.EVT_MENU(self, id, OnRightMenuSelect)
            if self._notebook.GetPageCount() > 1:
                id = wx.NewId()
                menu.Append(id, _("Close All but \"%s\"" % doc.GetPrintableName()))
                def OnRightMenuSelect(event):
                    for i in range(self._notebook.GetPageCount()-1, -1, -1): # Go from len-1 to 0
                        if i != index:
                            doc = self._notebook.GetPage(i).GetView().GetDocument()
                            if not self.GetDocumentManager().CloseDocument(doc, False):
                                return
                wx.EVT_MENU(self, id, OnRightMenuSelect)
                menu.AppendSeparator()
                tabsMenu = wx.Menu()
                menu.AppendMenu(wx.NewId(), _("Select Tab"), tabsMenu)
        else:
            y = y - 25  # wxBug: It is offsetting click events in the blank notebook area
            tabsMenu = menu

        if self._notebook.GetPageCount() > 1:
            selectIDs = {}
            for i in range(0, self._notebook.GetPageCount()):
                id = wx.NewId()
                selectIDs[id] = i
                tabsMenu.Append(id, self._notebook.GetPageText(i))
                def OnRightMenuSelect(event):
                    self._notebook.SetSelection(selectIDs[event.GetId()])
                wx.EVT_MENU(self, id, OnRightMenuSelect)

        self._notebook.PopupMenu(menu, wx.Point(x, y))
        menu.Destroy()


    def AddNotebookPage(self, panel, title):
        """
        Adds a document page to the notebook.
        """
        self._notebook.AddPage(panel, title)
        index = self._notebook.GetPageCount() - 1
        self._notebook.SetSelection(index)

        found = False  # Now set the icon
        template = panel.GetDocument().GetDocumentTemplate()
        if template:
            for t, iconIndex in self._iconIndexLookup:
                if t is template:
                    self._notebook.SetPageImage(index, iconIndex)
                    found = True
                    break
        if not found:
            self._notebook.SetPageImage(index, self._blankIconIndex)

        # wxBug: the wxListbook used on Mac needs its tabs list resized
        # whenever a new tab is added, but the only way to do this is
        # to resize the entire control
        if wx.Platform == "__WXMAC__":
            content_size = self._notebook.GetSize()
            self._notebook.SetSize((content_size.x+2, -1))
            self._notebook.SetSize((content_size.x, -1))

        self._notebook.Layout()

        windowMenuService = wx.GetApp().GetService(WindowMenuService)
        if windowMenuService:
            windowMenuService.BuildWindowMenu(wx.GetApp().GetTopWindow())  # build file menu list when we open a file


    def RemoveNotebookPage(self, panel):
        """
        Removes a document page from the notebook.
        """
        index = self.GetNotebookPageIndex(panel)
        if index > -1:
            if self._notebook.GetPageCount() == 1 or index < 2:
                pass
            elif index >= 1:
                self._notebook.SetSelection(index - 1)
            elif index < self._notebook.GetPageCount():
                self._notebook.SetSelection(index + 1)
            self._notebook.DeletePage(index)
            self._notebook.GetParent().SetToolTip(wx.ToolTip(""))

        windowMenuService = wx.GetApp().GetService(WindowMenuService)
        if windowMenuService:
            windowMenuService.BuildWindowMenu(wx.GetApp().GetTopWindow())  # build file menu list when we open a file


    def ActivateNotebookPage(self, panel):
        """
        Sets the notebook to the specified panel.
        """
        index = self.GetNotebookPageIndex(panel)
        if index > -1:
            self._notebook.SetFocus()
            self._notebook.SetSelection(index)


    def GetNotebookPageTitle(self, panel):
        index = self.GetNotebookPageIndex(panel)
        if index != -1:
            return self._notebook.GetPageText(self.GetNotebookPageIndex(panel))
        else:
            return None


    def SetNotebookPageTitle(self, panel, title):
        self._notebook.SetPageText(self.GetNotebookPageIndex(panel), title)


    def GetNotebookPageIndex(self, panel):
        """
        Returns the index of particular notebook panel.
        """
        index = -1
        for i in range(self._notebook.GetPageCount()):
            if self._notebook.GetPage(i) == panel:
                index = i
                break
        return index


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if wx.GetApp().ProcessEventBeforeWindows(event):
            return True
        if self._docManager and self._docManager.ProcessEvent(event):
            return True
        return DocMDIParentFrameMixIn.ProcessEvent(self, event)


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if wx.GetApp().ProcessUpdateUIEventBeforeWindows(event):
            return True
        if self._docManager and self._docManager.ProcessUpdateUIEvent(event):
            return True
        return DocMDIParentFrameMixIn.ProcessUpdateUIEvent(self, event)


    def OnExit(self, event):
        """
        Called when File/Exit is chosen and closes the window.
        """
        self.Close()


    def OnMRUFile(self, event):
        """
        Opens the appropriate file when it is selected from the file history
        menu.
        """
        n = event.GetId() - wx.ID_FILE1
        filename = self._docManager.GetHistoryFile(n)
        if filename:
            self._docManager.CreateDocument(filename, wx.lib.docview.DOC_SILENT)
        else:
            self._docManager.RemoveFileFromHistory(n)
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("File Error")
            wx.MessageBox("The file '%s' doesn't exist and couldn't be opened.\nIt has been removed from the most recently used files list" % FileNameFromPath(file),
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION,
                          self)


    def OnSize(self, event):
        """
        Called when the frame is resized and lays out the client window.
        """
        # Needed in case there are splitpanels around the mdi frame
        self._LayoutFrame()


    def OnCloseWindow(self, event):
        """
        Called when the frame is closed.  Remembers the frame size.
        """
        self.SaveEmbeddedWindowSizes()

        # save and close services last
        for service in wx.GetApp().GetServices():
            if not service.OnCloseFrame(event):
                return

        # From docview.MDIParentFrame
        if self._docManager.Clear(not event.CanVeto()):
            self.Destroy()
        else:
            event.Veto()


class DocMDIChildFrame(wx.MDIChildFrame):
    """
    The wxDocMDIChildFrame class provides a default frame for displaying
    documents on separate windows. This class can only be used for MDI child
    frames.

    The class is part of the document/view framework supported by wxWindows,
    and cooperates with the wxView, wxDocument, wxDocManager and wxDocTemplate
    classes.
    """


    def __init__(self, doc, view, frame, id, title, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE, name="frame"):
        """
        Constructor.  Note that the event table must be rebuilt for the
        frame since the EvtHandler is not virtual.
        """
        wx.MDIChildFrame.__init__(self, frame, id, title, pos, size, style, name)
        self._childDocument = doc
        self._childView = view
        if view:
            view.SetFrame(self)
        # self.Create(doc, view, frame, id, title, pos, size, style, name)
        self._activeEvent = None
        self._activated = 0
        wx.EVT_ACTIVATE(self, self.OnActivate)
        wx.EVT_CLOSE(self, self.OnCloseWindow)

        if frame:  # wxBug: For some reason the EVT_ACTIVATE event is not getting triggered for the first mdi client window that is opened so we have to do it manually
            mdiChildren = filter(lambda x: isinstance(x, wx.MDIChildFrame), frame.GetChildren())
            if len(mdiChildren) == 1:
                self.Activate()


##    # Couldn't get this to work, but seems to work fine with single stage construction
##    def Create(self, doc, view, frame, id, title, pos, size, style, name):
##        self._childDocument = doc
##        self._childView = view
##        if wx.MDIChildFrame.Create(self, frame, id, title, pos, size, style, name):
##            if view:
##                view.SetFrame(self)
##                return True
##        return False



    def Activate(self):  # Need this in case there are embedded sash windows and such, OnActivate is not getting called
        """
        Activates the current view.
        """
        if self._childView:
            self._childView.Activate(True)


    def OnTitleIsModified(self):
        """
        Add/remove to the frame's title an indication that the document is dirty.
        If the document is dirty, an '*' is appended to the title
        """
        title = self.GetTitle()
        if title:
            if self.GetDocument().IsModified():
                if title.endswith("*"):
                    return
                else:
                    title = title + "*"
                    self.SetTitle(title)
            else:
                if title.endswith("*"):
                    title = title[:-1]
                    self.SetTitle(title)                
                else:
                    return


    def ProcessEvent(event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if self._activeEvent == event:
            return False

        self._activeEvent = event  # Break recursion loops

        if self._childView:
            self._childView.Activate(True)

        if not self._childView or not self._childView.ProcessEvent(event):
            if not isinstance(event, wx.CommandEvent) or not self.GetParent() or not self.GetParent().ProcessEvent(event):
                ret = False
            else:
                ret = True
        else:
            ret = True

        self._activeEvent = None
        return ret


    def OnActivate(self, event):
        """
        Sets the currently active view to be the frame's view. You may need to
        override (but still call) this function in order to set the keyboard
        focus for your subwindow.
        """
        if self._activated != 0:
            return True
        self._activated += 1
        wx.MDIChildFrame.Activate(self)
        if event.GetActive() and self._childView:
            self._childView.Activate(event.GetActive())
        self._activated = 0


    def OnCloseWindow(self, event):
        """
        Closes and deletes the current view and document.
        """
        if self._childView:
            ans = False
            if not event.CanVeto():
                ans = True
            else:
                ans = self._childView.Close(deleteWindow = False)

            if ans:
                self._childView.Activate(False)
                self._childView.Destroy()
                self._childView = None
                if self._childDocument:
                    self._childDocument.Destroy()  # This isn't in the wxWindows codebase but the document needs to be disposed of somehow
                self._childDocument = None
                self.Destroy()
            else:
                event.Veto()
        else:
            event.Veto()


    def GetDocument(self):
        """
        Returns the document associated with this frame.
        """
        return self._childDocument


    def SetDocument(self, document):
        """
        Sets the document for this frame.
        """
        self._childDocument = document


    def GetView(self):
        """
        Returns the view associated with this frame.
        """
        return self._childView


    def SetView(self, view):
        """
        Sets the view for this frame.
        """
        self._childView = view


class DocService(wx.EvtHandler):
    """
    An abstract class used to add reusable services to a docview application.
    """


    def __init__(self):
        """Initializes the DocService."""
        pass


    def GetDocumentManager(self):
        """Returns the DocManager for the docview application."""
        return self._docManager


    def SetDocumentManager(self, docManager):
        """Sets the DocManager for the docview application."""
        self._docManager = docManager


    def InstallControls(self, frame, menuBar=None, toolBar=None, statusBar=None, document=None):
        """Called to install controls into the menubar and toolbar of a SDI or MDI window.  Override this method for a particular service."""
        pass


    def ProcessEventBeforeWindows(self, event):
        """
        Processes an event before the main window has a chance to process the window.
        Override this method for a particular service.
        """
        return False


    def ProcessUpdateUIEventBeforeWindows(self, event):
        """
        Processes a UI event before the main window has a chance to process the window.
        Override this method for a particular service.
        """
        return False


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        return False


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        return False


    def OnCloseFrame(self, event):
        """
        Called when the a docview frame is being closed.  Override this method
        so a service can either do cleanup or veto the frame being closed by
        returning false.
        """
        return True


    def OnExit(self):
        """
        Called when the the docview application is being closed.  Override this method
        so a service can either do cleanup or veto the frame being closed by
        returning false.
        """
        pass


    def GetMenuItemPos(self, menu, id):
        """
        Utility method used to find the position of a menu item so that services can
        easily find where to insert a menu item in InstallControls.
        """
        menuItems = menu.GetMenuItems()
        for i, menuItem in enumerate(menuItems):
            if menuItem.GetId() == id:
                return i
        return i


    def GetView(self):
        """
        Called by WindowMenuService to get views for services that don't
        have dedicated documents such as the Outline Service.
        """
        return None


class DocOptionsService(DocService):
    """
    A service that implements an options menu item and an options dialog with
    notebook tabs.  New tabs can be added by other services by calling the
    "AddOptionsPanel" method.
    """


    def __init__(self, showGeneralOptions=True, supportedModes=wx.lib.docview.DOC_SDI & wx.lib.docview.DOC_MDI):
        """
        Initializes the options service with the option of suppressing the default
        general options pane that is included with the options service by setting
        showGeneralOptions to False.  It allowModeChanges is set to False, the
        default general options pane will allow users to change the document
        interface mode between SDI and MDI modes.
        """
        DocService.__init__(self)
        self.ClearOptionsPanels()
        self._supportedModes = supportedModes
        self._toolOptionsID = wx.ID_PREFERENCES
        if showGeneralOptions:
            self.AddOptionsPanel(GeneralOptionsPanel)


    def InstallControls(self, frame, menuBar=None, toolBar=None, statusBar=None, document=None):
        """
        Installs a "Tools" menu with an "Options" menu item.
        """
        toolsMenuIndex = menuBar.FindMenu(_("&Tools"))
        if toolsMenuIndex > -1:
            toolsMenu = menuBar.GetMenu(toolsMenuIndex)
        else:
            toolsMenu = wx.Menu()
        if toolsMenuIndex == -1:
            formatMenuIndex = menuBar.FindMenu(_("&Format"))
            menuBar.Insert(formatMenuIndex + 1, toolsMenu, _("&Tools"))
        if toolsMenu:
            if toolsMenu.GetMenuItemCount():
                toolsMenu.AppendSeparator()
            toolsMenu.Append(self._toolOptionsID, _("&Options..."), _("Sets options"))
            wx.EVT_MENU(frame, self._toolOptionsID, frame.ProcessEvent)


    def ProcessEvent(self, event):
        """
        Checks to see if the "Options" menu item has been selected.
        """
        id = event.GetId()
        if id == self._toolOptionsID:
            self.OnOptions(event)
            return True
        else:
            return False


    def GetSupportedModes(self):
        """
        Return the modes supported by the application.  Use docview.DOC_SDI and
        docview.DOC_MDI flags to check if SDI and/or MDI modes are supported.
        """
        return self._supportedModes


    def SetSupportedModes(self, _supportedModessupportedModes):
        """
        Sets the modes supported by the application.  Use docview.DOC_SDI and
        docview.DOC_MDI flags to set if SDI and/or MDI modes are supported.
        """
        self._supportedModes = supportedModes


    def ClearOptionsPanels(self):
        """
        Clears all of the options panels that have been added into the
        options dialog.
        """
        self._optionsPanels = []


    def AddOptionsPanel(self, optionsPanel):
        """
        Adds an options panel to the options dialog.
        """
        self._optionsPanels.append(optionsPanel)


    def OnOptions(self, event):
        """
        Shows the options dialog, called when the "Options" menu item is selected.
        """
        if len(self._optionsPanels) == 0:
            return
        optionsDialog = OptionsDialog(wx.GetApp().GetTopWindow(), self._optionsPanels, self._docManager)
        optionsDialog.CenterOnParent()
        if optionsDialog.ShowModal() == wx.ID_OK:
            optionsDialog.OnOK(optionsDialog)  # wxBug: wxDialog should be calling this automatically but doesn't
        optionsDialog.Destroy()


class OptionsDialog(wx.Dialog):
    """
    A default options dialog used by the OptionsService that hosts a notebook
    tab of options panels.
    """


    def __init__(self, parent, optionsPanelClasses, docManager):
        """
        Initializes the options dialog with a notebook page that contains new
        instances of the passed optionsPanelClasses.
        """
        wx.Dialog.__init__(self, parent, -1, _("Options"))

        self._optionsPanels = []
        self._docManager = docManager

        HALF_SPACE = 5
        SPACE = 10

        sizer = wx.BoxSizer(wx.VERTICAL)

        if wx.Platform == "__WXMAC__":
            optionsNotebook = wx.Listbook(self, wx.NewId(), style=wx.LB_DEFAULT)
        else:
            optionsNotebook = wx.Notebook(self, wx.NewId(), style=wx.NB_MULTILINE)  # NB_MULTILINE is windows platform only
        sizer.Add(optionsNotebook, 0, wx.ALL | wx.EXPAND, SPACE)

        if wx.Platform == "__WXMAC__":
            iconList = wx.ImageList(16, 16, initialCount = len(optionsPanelClasses))
            self._iconIndexLookup = []

            for optionsPanelClass in optionsPanelClasses:
                optionsPanel = optionsPanelClass(optionsNotebook, -1)
                self._optionsPanels.append(optionsPanel)

                # We need to populate the image list before setting notebook images
                if hasattr(optionsPanel, "GetIcon"):
                    icon = optionsPanel.GetIcon()
                else:
                    icon = None
                if icon:
                    if icon.GetHeight() != 16 or icon.GetWidth() != 16:
                        icon.SetHeight(16)
                        icon.SetWidth(16)
                        if wx.GetApp().GetDebug():
                            print "Warning: icon for '%s' isn't 16x16, not crossplatform" % template._docTypeName
                    iconIndex = iconList.AddIcon(icon)
                    self._iconIndexLookup.append((optionsPanel, iconIndex))

                else:
                    # use -1 to represent that this panel has no icon
                    self._iconIndexLookup.append((optionsPanel, -1))

            optionsNotebook.AssignImageList(iconList)

            # Add icons to notebook
            for index in range(0, len(optionsPanelClasses)-1):
                iconIndex = self._iconIndexLookup[index][1]
                if iconIndex >= 0:
                    optionsNotebook.SetPageImage(index, iconIndex)
        else:
            for optionsPanelClass in optionsPanelClasses:
                optionsPanel = optionsPanelClass(optionsNotebook, -1)
                self._optionsPanels.append(optionsPanel)

        sizer.Add(self.CreateButtonSizer(wx.OK | wx.CANCEL), 0, wx.ALIGN_RIGHT | wx.RIGHT | wx.BOTTOM, HALF_SPACE)
        self.SetSizer(sizer)
        self.Layout()
        self.Fit()
        wx.CallAfter(self.DoRefresh)


    def DoRefresh(self):
        """
        wxBug: On Windows XP when using a multiline notebook the default page doesn't get
        drawn, but it works when using a single line notebook.
        """
        self.Refresh()


    def GetDocManager(self):
        """
        Returns the document manager passed to the OptionsDialog constructor.
        """
        return self._docManager


    def OnOK(self, event):
        """
        Calls the OnOK method of all of the OptionDialog's embedded panels
        """
        for optionsPanel in self._optionsPanels:
            optionsPanel.OnOK(event)


class GeneralOptionsPanel(wx.Panel):
    """
    A general options panel that is used in the OptionDialog to configure the
    generic properties of a pydocview application, such as "show tips at startup"
    and whether to use SDI or MDI for the application.
    """


    def __init__(self, parent, id):
        """
        Initializes the panel by adding an "Options" folder tab to the parent notebook and
        populating the panel with the generic properties of a pydocview application.
        """
        wx.Panel.__init__(self, parent, id)
        SPACE = 10
        HALF_SPACE = 5
        config = wx.ConfigBase_Get()
        self._showTipsCheckBox = wx.CheckBox(self, -1, _("Show tips at start up"))
        self._showTipsCheckBox.SetValue(config.ReadInt("ShowTipAtStartup", True))
        if self._AllowModeChanges():
            supportedModes = wx.GetApp().GetService(DocOptionsService).GetSupportedModes()
            choices = []
            self._sdiChoice = _("Show each document in its own window")
            self._mdiChoice = _("Show all documents in a single window with tabs")
            self._winMdiChoice = _("Show all documents in a single window with child windows")
            if supportedModes & wx.lib.docview.DOC_SDI:
                choices.append(self._sdiChoice)
            choices.append(self._mdiChoice)
            if wx.Platform == "__WXMSW__":
                choices.append(self._winMdiChoice)
            self._documentRadioBox = wx.RadioBox(self, -1, _("Document Display Style"),
                                          choices = choices,
                                          majorDimension=1,
                                          )
            if config.ReadInt("UseWinMDI", False):
                self._documentRadioBox.SetStringSelection(self._winMdiChoice)
            elif config.ReadInt("UseMDI", True):
                self._documentRadioBox.SetStringSelection(self._mdiChoice)
            else:
                self._documentRadioBox.SetStringSelection(self._sdiChoice)
            def OnDocumentInterfaceSelect(event):
                if not self._documentInterfaceMessageShown:
                    msgTitle = wx.GetApp().GetAppName()
                    if not msgTitle:
                        msgTitle = _("Document Options")
                    wx.MessageBox(_("Document interface changes will not appear until the application is restarted."),
                                  msgTitle,
                                  wx.OK | wx.ICON_INFORMATION,
                                  self.GetParent())
                    self._documentInterfaceMessageShown = True
            wx.EVT_RADIOBOX(self, self._documentRadioBox.GetId(), OnDocumentInterfaceSelect)
        optionsBorderSizer = wx.BoxSizer(wx.VERTICAL)
        optionsSizer = wx.BoxSizer(wx.VERTICAL)
        if self._AllowModeChanges():
            optionsSizer.Add(self._documentRadioBox, 0, wx.ALL, HALF_SPACE)
        optionsSizer.Add(self._showTipsCheckBox, 0, wx.ALL, HALF_SPACE)
        optionsBorderSizer.Add(optionsSizer, 0, wx.ALL, SPACE)
        self.SetSizer(optionsBorderSizer)
        self.Layout()
        self._documentInterfaceMessageShown = False
        parent.AddPage(self, _("General"))


    def _AllowModeChanges(self):
        supportedModes = wx.GetApp().GetService(DocOptionsService).GetSupportedModes()
        return supportedModes & wx.lib.docview.DOC_SDI and supportedModes & wx.lib.docview.DOC_MDI or wx.Platform == "__WXMSW__" and supportedModes & wx.lib.docview.DOC_MDI  # More than one mode is supported, allow selection


    def OnOK(self, optionsDialog):
        """
        Updates the config based on the selections in the options panel.
        """
        config = wx.ConfigBase_Get()
        config.WriteInt("ShowTipAtStartup", self._showTipsCheckBox.GetValue())
        if self._AllowModeChanges():
            config.WriteInt("UseMDI", (self._documentRadioBox.GetStringSelection() == self._mdiChoice))
            config.WriteInt("UseWinMDI", (self._documentRadioBox.GetStringSelection() == self._winMdiChoice))


    def GetIcon(self):
        """ Return icon for options panel on the Mac. """
        return wx.GetApp().GetDefaultIcon()


class DocApp(wx.PySimpleApp):
    """
    The DocApp class serves as the base class for pydocview applications and offers
    functionality such as services, creation of SDI and MDI frames, show tips,
    and a splash screen.
    """


    def OnInit(self):
        """
        Initializes the DocApp.
        """
        self._services = []
        self._defaultIcon = None
        self._registeredCloseEvent = False
        self._useTabbedMDI = True

        if not hasattr(self, "_debug"):  # only set if not already initialized
            self._debug = False
        if not hasattr(self, "_singleInstance"):  # only set if not already initialized
            self._singleInstance = True

        # if _singleInstance is TRUE only allow one single instance of app to run.
        # When user tries to run a second instance of the app, abort startup,
        # But if user also specifies files to open in command line, send message to running app to open those files
        if self._singleInstance:
            # create shared memory temporary file
            if wx.Platform == '__WXMSW__':
                tfile = tempfile.TemporaryFile(prefix="ag", suffix="tmp")
                fno = tfile.fileno()
                self._sharedMemory = mmap.mmap(fno, 1024, "shared_memory")
            else:
                tfile = file(os.path.join(tempfile.gettempdir(), tempfile.gettempprefix() + self.GetAppName() + '-' + wx.GetUserId() + "AGSharedMemory"), 'w+b')
                tfile.write("*")
                tfile.seek(1024)
                tfile.write(" ")
                tfile.flush()
                fno = tfile.fileno()
                self._sharedMemory = mmap.mmap(fno, 1024)

            self._singleInstanceChecker = wx.SingleInstanceChecker(self.GetAppName() + '-' + wx.GetUserId(), tempfile.gettempdir())
            if self._singleInstanceChecker.IsAnotherRunning():
                # have running single instance open file arguments
                data = pickle.dumps(sys.argv[1:])
                while 1:
                    self._sharedMemory.seek(0)
                    marker = self._sharedMemory.read_byte()
                    if marker == '\0' or marker == '*':        # available buffer
                        self._sharedMemory.seek(0)
                        self._sharedMemory.write_byte('-')     # set writing marker
                        self._sharedMemory.write(data)  # write files we tried to open to shared memory
                        self._sharedMemory.seek(0)
                        self._sharedMemory.write_byte('+')     # set finished writing marker
                        self._sharedMemory.flush()
                        break
                    else:
                        time.sleep(1)  # give enough time for buffer to be available

                return False
            else:
                self._timer = wx.PyTimer(self.DoBackgroundListenAndLoad)
                self._timer.Start(250)

        return True


    def OpenMainFrame(self):
        docManager = self.GetDocumentManager()
        if docManager.GetFlags() & wx.lib.docview.DOC_MDI:
            if self.GetUseTabbedMDI():
                frame = wx.lib.pydocview.DocTabbedParentFrame(docManager, None, -1, self.GetAppName())
            else:
                frame = wx.lib.pydocview.DocMDIParentFrame(docManager, None, -1, self.GetAppName())
            frame.Show(True)

    def MacOpenFile(self, filename):
        self.GetDocumentManager().CreateDocument(os.path.normpath(filename), wx.lib.docview.DOC_SILENT)

        # force display of running app
        topWindow = wx.GetApp().GetTopWindow()
        if topWindow.IsIconized():
            topWindow.Iconize(False)
        else:
            topWindow.Raise()

    def DoBackgroundListenAndLoad(self):
        """
        Open any files specified in the given command line argument passed in via shared memory
        """
        self._timer.Stop()

        self._sharedMemory.seek(0)
        if self._sharedMemory.read_byte() == '+':  # available data
            data = self._sharedMemory.read(1024-1)
            self._sharedMemory.seek(0)
            self._sharedMemory.write_byte("*")     # finished reading, set buffer free marker
            self._sharedMemory.flush()
            args = pickle.loads(data)
            for arg in args:
                if (wx.Platform != "__WXMSW__" or arg[0] != "/") and arg[0] != '-' and os.path.exists(arg):
                    self.GetDocumentManager().CreateDocument(os.path.normpath(arg), wx.lib.docview.DOC_SILENT)

            # force display of running app
            topWindow = wx.GetApp().GetTopWindow()
            if topWindow.IsIconized():
                topWindow.Iconize(False)
            else:
                topWindow.Raise()


        self._timer.Start(1000) # 1 second interval


    def OpenCommandLineArgs(self):
        """
        Called to open files that have been passed to the application from the
        command line.
        """
        args = sys.argv[1:]
        for arg in args:
            if (wx.Platform != "__WXMSW__" or arg[0] != "/") and arg[0] != '-' and os.path.exists(arg):
                self.GetDocumentManager().CreateDocument(os.path.normpath(arg), wx.lib.docview.DOC_SILENT)


    def GetDocumentManager(self):
        """
        Returns the document manager associated to the DocApp.
        """
        return self._docManager


    def SetDocumentManager(self, docManager):
        """
        Sets the document manager associated with the DocApp and loads the
        DocApp's file history into the document manager.
        """
        self._docManager = docManager
        config = wx.ConfigBase_Get()
        self.GetDocumentManager().FileHistoryLoad(config)


    def ProcessEventBeforeWindows(self, event):
        """
        Enables services to process an event before the main window has a chance to
        process the window.
        """
        for service in self._services:
            if service.ProcessEventBeforeWindows(event):
                return True
        return False


    def ProcessUpdateUIEventBeforeWindows(self, event):
        """
        Enables services to process a UI event before the main window has a chance
        to process the window.
        """
        for service in self._services:
            if service.ProcessUpdateUIEventBeforeWindows(event):
                return True
        return False


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        for service in self._services:
            if service.ProcessEvent(event):
                return True
        return False


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        for service in self._services:
            if service.ProcessUpdateUIEvent(event):
                return True
        return False


    def InstallService(self, service):
        """
        Installs an instance of a DocService into the DocApp.
        """
        service.SetDocumentManager(self._docManager)
        self._services.append(service)
        return service


    def GetServices(self):
        """
        Returns the DocService instances that have been installed into the DocApp.
        """
        return self._services


    def GetService(self, type):
        """
        Returns the instance of a particular type of service that has been installed
        into the DocApp.  For example, "wx.GetApp().GetService(pydocview.OptionsService)"
        returns the isntance of the OptionsService that is running within the DocApp.
        """
        for service in self._services:
            if isinstance(service, type):
                return service
        return None


    def OnExit(self):
        """
        Called when the DocApp is exited, enables the installed DocServices to exit
        and saves the DocManager's file history.
        """
        for service in self._services:
            service.OnExit()
        config = wx.ConfigBase_Get()
        self._docManager.FileHistorySave(config)

        if hasattr(self, "_singleInstanceChecker"):
            del self._singleInstanceChecker


    def GetDefaultDocManagerFlags(self):
        """
        Returns the default flags to use when creating the DocManager.
        """
        config = wx.ConfigBase_Get()
        if config.ReadInt("UseMDI", True) or config.ReadInt("UseWinMDI", False):
            flags = wx.lib.docview.DOC_MDI | wx.lib.docview.DOC_OPEN_ONCE
            if config.ReadInt("UseWinMDI", False):
                self.SetUseTabbedMDI(False)
        else:
            flags = wx.lib.docview.DOC_SDI | wx.lib.docview.DOC_OPEN_ONCE
        return flags


    def ShowTip(self, frame, tipProvider):
        """
        Shows the tip window, generally this is called when an application starts.
        A wx.TipProvider must be passed.
        """
        config = wx.ConfigBase_Get()
        showTip = config.ReadInt("ShowTipAtStartup", 1)
        if showTip:
            index = config.ReadInt("TipIndex", 0)
            showTipResult = wx.ShowTip(wx.GetApp().GetTopWindow(), tipProvider, showAtStartup = showTip)
            if showTipResult != showTip:
                config.WriteInt("ShowTipAtStartup", showTipResult)


    def GetEditMenu(self, frame):
        """
        Utility method that finds the Edit menu within the menubar of a frame.
        """
        menuBar = frame.GetMenuBar()
        if not menuBar:
            return None
        editMenuIndex = menuBar.FindMenu(_("&Edit"))
        if editMenuIndex == -1:
            return None
        return menuBar.GetMenu(editMenuIndex)


    def GetUseTabbedMDI(self):
        """
        Returns True if Windows MDI should use folder tabs instead of child windows.
        """
        return self._useTabbedMDI


    def SetUseTabbedMDI(self, useTabbedMDI):
        """
        Set to True if Windows MDI should use folder tabs instead of child windows.
        """
        self._useTabbedMDI = useTabbedMDI


    def CreateDocumentFrame(self, view, doc, flags, id = -1, title = "", pos = wx.DefaultPosition, size = wx.DefaultSize, style = wx.DEFAULT_FRAME_STYLE):
        """
        Called by the DocManager to create and return a new Frame for a Document.
        Chooses whether to create an MDIChildFrame or SDI Frame based on the
        DocManager's flags.
        """
        docflags = self.GetDocumentManager().GetFlags()
        if docflags & wx.lib.docview.DOC_SDI:
            frame = self.CreateSDIDocumentFrame(doc, view, id, title, pos, size, style)
            frame.Show()

            # wxBug: operating system bug, first window is set to the position of last window closed, ignoring passed in position on frame creation
            #        also, initial size is incorrect for the same reasons
            if frame.GetPosition() != pos:
                frame.Move(pos)
            if frame.GetSize() != size:
                frame.SetSize(size)

            if doc and doc.GetCommandProcessor():
                doc.GetCommandProcessor().SetEditMenu(self.GetEditMenu(frame))
        elif docflags & wx.lib.docview.DOC_MDI:
            if self.GetUseTabbedMDI():
                frame = self.CreateTabbedDocumentFrame(doc, view, id, title, pos, size, style)
            else:
                frame = self.CreateMDIDocumentFrame(doc, view, id, title, pos, size, style)
                if doc:
                    if doc.GetDocumentTemplate().GetIcon():
                        frame.SetIcon(doc.GetDocumentTemplate().GetIcon())
                    elif wx.GetApp().GetTopWindow().GetIcon():
                        frame.SetIcon(wx.GetApp().GetTopWindow().GetIcon())
            if doc and doc.GetCommandProcessor():
                doc.GetCommandProcessor().SetEditMenu(self.GetEditMenu(wx.GetApp().GetTopWindow()))
        if not frame.GetIcon() and self._defaultIcon:
            frame.SetIcon(self.GetDefaultIcon())
        view.SetFrame(frame)
        return frame


    def CreateSDIDocumentFrame(self, doc, view, id=-1, title="", pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE):
        """
        Creates and returns an SDI Document Frame.
        """
        frame = DocSDIFrame(doc, view, None, id, title, pos, size, style)
        return frame


    def CreateTabbedDocumentFrame(self, doc, view, id=-1, title="", pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE):
        """
        Creates and returns an MDI Document Frame for a Tabbed MDI view
        """
        frame = DocTabbedChildFrame(doc, view, wx.GetApp().GetTopWindow(), id, title, pos, size, style)
        return frame


    def CreateMDIDocumentFrame(self, doc, view, id=-1, title="", pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE):
        """
        Creates and returns an MDI Document Frame.
        """
        # if any child windows are maximized, then user must want any new children maximized
        # if no children exist, then use the default value from registry
        # wxBug:  Only current window is maximized, so need to check every child frame
        parentFrame = wx.GetApp().GetTopWindow()
        childrenMaximized = filter(lambda child: isinstance(child, wx.MDIChildFrame) and child.IsMaximized(), parentFrame.GetChildren())
        if childrenMaximized:
            maximize = True
        else:
            children = filter(lambda child: isinstance(child, wx.MDIChildFrame), parentFrame.GetChildren())
            if children:
                # other windows exist and none are maximized
                maximize = False
            else:
                # get default setting from registry
                maximize = wx.ConfigBase_Get().ReadInt("MDIChildFrameMaximized", False)

        frame = wx.lib.docview.DocMDIChildFrame(doc, view, wx.GetApp().GetTopWindow(), id, title, pos, size, style)
        if maximize:  # wxBug: Should already be maximizing new child frames if one is maximized but it's not so we have to force it to
            frame.Maximize(True)

##        wx.EVT_MAXIMIZE(frame, self.OnMaximize) # wxBug: This doesn't work, need to save MDIChildFrameMaximized state on close of windows instead
        wx.EVT_CLOSE(frame, self.OnCloseChildWindow)
        if not self._registeredCloseEvent:
            wx.EVT_CLOSE(parentFrame, self.OnCloseMainWindow) # need to check on this, but only once
            self._registeredCloseEvent = True

        return frame


    def SaveMDIDocumentFrameMaximizedState(self, maximized):
        """
        Remember in the config whether the MDI Frame is maximized so that it can be restored
        on open.
        """
        config = wx.ConfigBase_Get()
        maximizeFlag = config.ReadInt("MDIChildFrameMaximized", False)
        if maximized != maximizeFlag:
            config.WriteInt("MDIChildFrameMaximized", maximized)


    def OnCloseChildWindow(self, event):
        """
        Called when an MDI Child Frame is closed.  Calls SaveMDIDocumentFrameMaximizedState to
        remember whether the MDI Frame is maximized so that it can be restored on open.
        """
        self.SaveMDIDocumentFrameMaximizedState(event.GetEventObject().IsMaximized())
        event.Skip()


    def OnCloseMainWindow(self, event):
        """
        Called when the MDI Parent Frame is closed.  Remembers whether the MDI Parent Frame is
        maximized.
        """
        children = event.GetEventObject().GetChildren()
        childrenMaximized = filter(lambda child: isinstance(child, wx.MDIChildFrame)and child.IsMaximized(), children)
        if childrenMaximized:
            self.SaveMDIDocumentFrameMaximizedState(True)
        else:
            childrenNotMaximized = filter(lambda child: isinstance(child, wx.MDIChildFrame), children)

            if childrenNotMaximized:
                # other windows exist and none are maximized
                self.SaveMDIDocumentFrameMaximizedState(False)

        event.Skip()


    def GetDefaultIcon(self):
        """
        Returns the application's default icon.
        """
        return self._defaultIcon


    def SetDefaultIcon(self, icon):
        """
        Sets the application's default icon.
        """
        self._defaultIcon = icon


    def GetDebug(self):
        """
        Returns True if the application is in debug mode.
        """
        return self._debug


    def SetDebug(self, debug):
        """
        Sets the application's debug mode.
        """
        self._debug = debug


    def GetSingleInstance(self):
        """
        Returns True if the application is in single instance mode.  Used to determine if multiple instances of the application is allowed to launch.
        """
        return self._singleInstance


    def SetSingleInstance(self, singleInstance):
        """
        Sets application's single instance mode.
        """
        self._singleInstance = singleInstance



    def CreateChildDocument(self, parentDocument, documentType, objectToEdit, path=''):
        """
        Creates a child window of a document that edits an object.  The child window
        is managed by the parent document frame, so it will be prompted to close if its
        parent is closed, etc.  Child Documents are useful when there are complicated
        Views of a Document and users will need to tunnel into the View.
        """
        for document in self.GetDocumentManager().GetDocuments()[:]:  # Cloning list to make sure we go through all docs even as they are deleted
            if isinstance(document, ChildDocument) and document.GetParentDocument() == parentDocument:
                if document.GetData() == objectToEdit:
                    if hasattr(document.GetFirstView().GetFrame(), "SetFocus"):
                        document.GetFirstView().GetFrame().SetFocus()
                    return document
        for temp in wx.GetApp().GetDocumentManager().GetTemplates():
            if temp.GetDocumentType() == documentType:
                break
            temp = None
        newDoc = temp.CreateDocument(path, 0, data = objectToEdit, parentDocument = parentDocument)
        newDoc.SetDocumentName(temp.GetDocumentName())
        newDoc.SetDocumentTemplate(temp)
        if path == '':
            newDoc.OnNewDocument()
        else:
            if not newDoc.OnOpenDocument(path):
                newDoc.DeleteAllViews()  # Implicitly deleted by DeleteAllViews
                return None
        return newDoc


    def CloseChildDocuments(self, parentDocument):
        """
        Closes the child windows of a Document.
        """
        for document in self.GetDocumentManager().GetDocuments()[:]:  # Cloning list to make sure we go through all docs even as they are deleted
            if isinstance(document, ChildDocument) and document.GetParentDocument() == parentDocument:
                if document.GetFirstView().GetFrame():
                    document.GetFirstView().GetFrame().SetFocus()
                if not document.GetFirstView().OnClose():
                    return False
        return True


    def IsMDI(self):
        """
        Returns True if the application is in MDI mode.
        """
        return self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_MDI


    def IsSDI(self):
        """
        Returns True if the application is in SDI mode.
        """
        return self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI


    def ShowSplash(self, image):
        """
        Shows a splash window with the given image.  Input parameter 'image' can either be a wx.Bitmap or a filename.
        """
        if isinstance(image, wx.Bitmap):
            splash_bmp = image
        else:
            splash_bmp = wx.Image(image).ConvertToBitmap()
        self._splash = wx.SplashScreen(splash_bmp, wx.SPLASH_CENTRE_ON_SCREEN|wx.SPLASH_NO_TIMEOUT, 0, None, -1, style=wx.SIMPLE_BORDER|wx.FRAME_NO_TASKBAR)
        self._splash.Show()


    def CloseSplash(self):
        """
        Closes the splash window.
        """
        if self._splash:
            self._splash.Close(True)


class _DocFrameFileDropTarget(wx.FileDropTarget):
    """
    Class used to handle drops into the document frame.
    """

    def __init__(self, docManager, docFrame):
        """
        Initializes the FileDropTarget class with the active docManager and the docFrame.
        """
        wx.FileDropTarget.__init__(self)
        self._docManager = docManager
        self._docFrame = docFrame


    def OnDropFiles(self, x, y, filenames):
        """
        Called when files are dropped in the drop target and tells the docManager to open
        the files.
        """
        try:
            for file in filenames:
                self._docManager.CreateDocument(file, wx.lib.docview.DOC_SILENT)
        except:
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("File Error")
            wx.MessageBox("Could not open '%s'.  '%s'" % (wx.lib.docview.FileNameFromPath(file), sys.exc_value),
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION,
                          self._docManager.FindSuitableParent())


class DocMDIParentFrame(wx.lib.docview.DocMDIParentFrame, DocFrameMixIn, DocMDIParentFrameMixIn):
    """
    The DocMDIParentFrame is the primary frame which the DocApp uses to host MDI child windows.  It offers
    features such as a default menubar, toolbar, and status bar, and a mechanism to manage embedded windows
    on the edges of the DocMDIParentFrame.
    """


    def __init__(self, docManager, parent, id, title, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE, name="DocMDIFrame", embeddedWindows=0, minSize=20):
        """
        Initializes the DocMDIParentFrame with the default menubar, toolbar, and status bar.  Use the
        optional embeddedWindows parameter with the embedded window constants to create embedded
        windows around the edges of the DocMDIParentFrame.
        """
        pos, size = self._GetPosSizeFromConfig(pos, size)
        wx.lib.docview.DocMDIParentFrame.__init__(self, docManager, parent, id, title, pos, size, style, name)
        self._InitFrame(embeddedWindows, minSize)


    def _LayoutFrame(self):
        """
        Lays out the frame.
        """
        wx.LayoutAlgorithm().LayoutMDIFrame(self)
        self.GetClientWindow().Refresh()


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if wx.GetApp().ProcessEventBeforeWindows(event):
            return True
        if wx.lib.docview.DocMDIParentFrame.ProcessEvent(self, event):
            return True
        return DocMDIParentFrameMixIn.ProcessEvent(self, event)


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if wx.GetApp().ProcessUpdateUIEventBeforeWindows(event):
            return True
        if wx.lib.docview.DocMDIParentFrame.ProcessUpdateUIEvent(self, event):  # Let the views handle the event before the services
            return True
        if event.GetId() == wx.ID_ABOUT:  # Using ID_ABOUT to update the window menu, the window menu items are not triggering
            self.UpdateWindowMenu()
            return True
        return DocMDIParentFrameMixIn.ProcessUpdateUIEvent(self, event)


    def UpdateWindowMenu(self):
        """
        Updates the WindowMenu on Windows platforms.
        """
        if wx.Platform == '__WXMSW__':
            children = filter(lambda child: isinstance(child, wx.MDIChildFrame), self.GetChildren())
            windowCount = len(children)
            hasWindow = windowCount >= 1
            has2OrMoreWindows = windowCount >= 2

            windowMenu = self.GetWindowMenu()
            if windowMenu:
                windowMenu.Enable(wx.IDM_WINDOWTILE, hasWindow)
                windowMenu.Enable(wx.IDM_WINDOWTILEHOR, hasWindow)
                windowMenu.Enable(wx.IDM_WINDOWCASCADE, hasWindow)
                windowMenu.Enable(wx.IDM_WINDOWICONS, hasWindow)
                windowMenu.Enable(wx.IDM_WINDOWTILEVERT, hasWindow)
                wx.IDM_WINDOWPREV = 4006  # wxBug: Not defined for some reason
                windowMenu.Enable(wx.IDM_WINDOWPREV, has2OrMoreWindows)
                windowMenu.Enable(wx.IDM_WINDOWNEXT, has2OrMoreWindows)



    def OnSize(self, event):
        """
        Called when the DocMDIParentFrame is resized and lays out the MDI client window.
        """
        # Needed in case there are splitpanels around the mdi frame
        self._LayoutFrame()


    def OnCloseWindow(self, event):
        """
        Called when the DocMDIParentFrame is closed.  Remembers the frame size.
        """
        self.SaveEmbeddedWindowSizes()

        # save and close services last.
        for service in wx.GetApp().GetServices():
            if not service.OnCloseFrame(event):
                return

        # save and close documents
        # documents with a common view, e.g. project view, should save the document, but not close the window
        # and let the service close the window.
        wx.lib.docview.DocMDIParentFrame.OnCloseWindow(self, event)


class DocSDIFrame(wx.lib.docview.DocChildFrame, DocFrameMixIn):
    """
    The DocSDIFrame host DocManager Document windows.  It offers features such as a default menubar,
    toolbar, and status bar.
    """


    def __init__(self, doc, view, parent, id, title, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE, name="DocSDIFrame"):
        """
        Initializes the DocSDIFrame with the default menubar, toolbar, and status bar.
        """
        wx.lib.docview.DocChildFrame.__init__(self, doc, view, parent, id, title, pos, size, style, name)
        self._fileMenu = None
        if doc:
            self._docManager = doc.GetDocumentManager()
        else:
            self._docManager = None
        self.SetDropTarget(_DocFrameFileDropTarget(self._docManager, self))

        wx.EVT_MENU(self, wx.ID_ABOUT, self.OnAbout)
        wx.EVT_MENU(self, wx.ID_EXIT, self.OnExit)
        wx.EVT_MENU_RANGE(self, wx.ID_FILE1, wx.ID_FILE9, self.OnMRUFile)

        self.InitializePrintData()

        menuBar = self.CreateDefaultMenuBar(sdi=True)
        toolBar = self.CreateDefaultToolBar()
        self.SetToolBar(toolBar)
        statusBar = self.CreateDefaultStatusBar()

        for service in wx.GetApp().GetServices():
            service.InstallControls(self, menuBar = menuBar, toolBar = toolBar, statusBar = statusBar, document = doc)

        self.SetMenuBar(menuBar)  # wxBug: Need to do this in SDI to mimic MDI... because have to set the menubar at the very end or the automatic MDI "window" menu doesn't get put in the right place when the services add new menus to the menubar


    def _LayoutFrame(self):
        """
        Lays out the Frame.
        """
        self.Layout()


    def OnExit(self, event):
        """
        Called when the application is exitting.
        """
        if self._childView.GetDocumentManager().Clear(force = False):
            self.Destroy()
        else:
            event.Veto()


    def OnMRUFile(self, event):
        """
        Opens the appropriate file when it is selected from the file history
        menu.
        """
        n = event.GetId() - wx.ID_FILE1
        filename = self._docManager.GetHistoryFile(n)
        if filename:
            self._docManager.CreateDocument(filename, wx.lib.docview.DOC_SILENT)
        else:
            self._docManager.RemoveFileFromHistory(n)
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("File Error")
            wx.MessageBox("The file '%s' doesn't exist and couldn't be opened.\nIt has been removed from the most recently used files list" % docview.FileNameFromPath(file),
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION,
                          self)


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if wx.GetApp().ProcessEventBeforeWindows(event):
            return True
        if self._childView:
            self._childView.Activate(True)

        id = event.GetId()
        if id == SAVEALL_ID:
            self.OnFileSaveAll(event)
            return True

        if hasattr(self._childView, "GetDocumentManager") and self._childView.GetDocumentManager().ProcessEvent(event):  # Need to call docmanager here since super class relies on DocParentFrame which we are not using
            return True
        else:
            return wx.GetApp().ProcessEvent(event)


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if wx.GetApp().ProcessUpdateUIEventBeforeWindows(event):
            return True
        if self._childView:
            if hasattr(self._childView, "GetDocumentManager"):
                docMgr = self._childView.GetDocumentManager()
                if docMgr:
                    if docMgr.GetCurrentDocument() != self._childView.GetDocument():
                        return False
                    if docMgr.ProcessUpdateUIEvent(event):  # Let the views handle the event before the services
                        return True
        id = event.GetId()
        if id == wx.ID_CUT:
            event.Enable(False)
            return True
        elif id == wx.ID_COPY:
            event.Enable(False)
            return True
        elif id == wx.ID_PASTE:
            event.Enable(False)
            return True
        elif id == wx.ID_CLEAR:
            event.Enable(False)
            return True
        elif id == wx.ID_SELECTALL:
            event.Enable(False)
            return True
        elif id == SAVEALL_ID:
            filesModified = False
            docs = wx.GetApp().GetDocumentManager().GetDocuments()
            for doc in docs:
                if doc.IsModified():
                    filesModified = True
                    break

            event.Enable(filesModified)
            return True
        else:
            return wx.GetApp().ProcessUpdateUIEvent(event)


    def OnCloseWindow(self, event):
        """
        Called when the window is saved.  Enables services to help close the frame.
        """
        for service in wx.GetApp().GetServices():
            service.OnCloseFrame(event)
        wx.lib.docview.DocChildFrame.OnCloseWindow(self, event)
        if self._fileMenu and self._docManager:
            self._docManager.FileHistoryRemoveMenu(self._fileMenu)


class AboutService(DocService):
    """
    About Dialog Service that installs under the Help menu to show the properties of the current application.
    """

    def __init__(self, aboutDialog=None, image=None):
        """
        Initializes the AboutService.
        """
        if aboutDialog:
            self._dlg = aboutDialog
            self._image = None
        else:
            self._dlg = AboutDialog  # use default AboutDialog
            self._image = image


    def ShowAbout(self):
        """
        Show the AboutDialog
        """
        if self._image:
            dlg = self._dlg(wx.GetApp().GetTopWindow(), self._image)
        else:
            dlg = self._dlg(wx.GetApp().GetTopWindow())
        dlg.CenterOnParent()
        dlg.ShowModal()
        dlg.Destroy()


    def SetAboutDialog(self, dlg):
        """
        Customize the AboutDialog
        """
        self._dlg = dlg


class AboutDialog(wx.Dialog):
    """
    Opens an AboutDialog.  Shared by DocMDIParentFrame and DocSDIFrame.
    """

    def __init__(self, parent, image=None):
        """
        Initializes the about dialog.
        """
        wx.Dialog.__init__(self, parent, -1, _("About ") + wx.GetApp().GetAppName(), style = wx.DEFAULT_DIALOG_STYLE)

        sizer = wx.BoxSizer(wx.VERTICAL)
        if image:
            imageItem = wx.StaticBitmap(self, -1, image.ConvertToBitmap(), (0,0), (image.GetWidth(), image.GetHeight()))
            sizer.Add(imageItem, 0, wx.ALIGN_CENTER|wx.ALL, 0)
        sizer.Add(wx.StaticText(self, -1, wx.GetApp().GetAppName()), 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        btn = wx.Button(self, wx.ID_OK)
        sizer.Add(btn, 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        self.SetSizer(sizer)
        sizer.Fit(self)



class FilePropertiesService(DocService):
    """
    Service that installs under the File menu to show the properties of the file associated
    with the current document.
    """

    PROPERTIES_ID = wx.NewId()


    def __init__(self):
        """
        Initializes the PropertyService.
        """
        self._customEventHandlers = []


    def InstallControls(self, frame, menuBar=None, toolBar=None, statusBar=None, document=None):
        """
        Installs a File/Properties menu item.
        """
        fileMenu = menuBar.GetMenu(menuBar.FindMenu(_("&File")))
        exitMenuItemPos = self.GetMenuItemPos(fileMenu, wx.ID_EXIT)
        fileMenu.InsertSeparator(exitMenuItemPos)
        fileMenu.Insert(exitMenuItemPos, FilePropertiesService.PROPERTIES_ID, _("&Properties"), _("Show file properties"))
        wx.EVT_MENU(frame, FilePropertiesService.PROPERTIES_ID, self.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, FilePropertiesService.PROPERTIES_ID, self.ProcessUpdateUIEvent)


    def ProcessEvent(self, event):
        """
        Detects when the File/Properties menu item is selected.
        """
        id = event.GetId()
        if id == FilePropertiesService.PROPERTIES_ID:
            for eventHandler in self._customEventHandlers:
                if eventHandler.ProcessEvent(event):
                    return True

            self.ShowPropertiesDialog()
            return True
        else:
            return False


    def ProcessUpdateUIEvent(self, event):
        """
        Updates the File/Properties menu item.
        """
        id = event.GetId()
        if id == FilePropertiesService.PROPERTIES_ID:
            for eventHandler in self._customEventHandlers:
                if eventHandler.ProcessUpdateUIEvent(event):
                    return True

            event.Enable(wx.GetApp().GetDocumentManager().GetCurrentDocument() != None)
            return True
        else:
            return False


    def ShowPropertiesDialog(self, filename=None):
        """
        Shows the PropertiesDialog for the specified file.
        """
        if not filename:
            filename = wx.GetApp().GetDocumentManager().GetCurrentDocument().GetFilename()

        filePropertiesDialog = FilePropertiesDialog(wx.GetApp().GetTopWindow(), filename)
        filePropertiesDialog.CenterOnParent()
        if filePropertiesDialog.ShowModal() == wx.ID_OK:
            pass  # Handle OK
        filePropertiesDialog.Destroy()


    def GetCustomEventHandlers(self):
        """
        Returns the custom event handlers for the PropertyService.
        """
        return self._customEventHandlers


    def AddCustomEventHandler(self, handler):
        """
        Adds a custom event handlers for the PropertyService.  A custom event handler enables
        a different dialog to be provided for a particular file.
        """
        self._customEventHandlers.append(handler)


    def RemoveCustomEventHandler(self, handler):
        """
        Removes a custom event handler from the PropertyService.
        """
        self._customEventHandlers.remove(handler)


    def chopPath(self, text, length=36):
        """
        Simple version of textwrap.  textwrap.fill() unfortunately chops lines at spaces
        and creates odd word boundaries.  Instead, we will chop the path without regard to
        spaces, but pay attention to path delimiters.
        """
        chopped = ""
        textLen = len(text)
        start = 0

        while start < textLen:
            end = start + length
            if end > textLen:
                end = textLen

            # see if we can find a delimiter to chop the path
            if end < textLen:
                lastSep = text.rfind(os.sep, start, end + 1)
                if lastSep != -1 and lastSep != start:
                    end = lastSep

            if len(chopped):
                chopped = chopped + '\n' + text[start:end]
            else:
                chopped = text[start:end]

            start = end

        return chopped


class FilePropertiesDialog(wx.Dialog):
    """
    Dialog that shows the properties of a file.  Invoked by the PropertiesService.
    """


    def __init__(self, parent, filename):
        """
        Initializes the properties dialog.
        """
        wx.Dialog.__init__(self, parent, -1, _("File Properties"), size=(310, 330))

        HALF_SPACE = 5
        SPACE = 10

        filePropertiesService = wx.GetApp().GetService(FilePropertiesService)

        fileExists = os.path.exists(filename)

        notebook = wx.Notebook(self, -1)
        tab = wx.Panel(notebook, -1)

        gridSizer = RowColSizer()

        gridSizer.Add(wx.StaticText(tab, -1, _("Filename:")), flag=wx.RIGHT, border=HALF_SPACE, row=0, col=0)
        gridSizer.Add(wx.StaticText(tab, -1, os.path.basename(filename)), row=0, col=1)

        gridSizer.Add(wx.StaticText(tab, -1, _("Location:")), flag=wx.RIGHT, border=HALF_SPACE, row=1, col=0)
        gridSizer.Add(wx.StaticText(tab, -1, filePropertiesService.chopPath(os.path.dirname(filename))), flag=wx.BOTTOM, border=SPACE, row=1, col=1)

        gridSizer.Add(wx.StaticText(tab, -1, _("Size:")), flag=wx.RIGHT, border=HALF_SPACE, row=2, col=0)
        if fileExists:
            gridSizer.Add(wx.StaticText(tab, -1, str(os.path.getsize(filename)) + ' ' + _("bytes")), row=2, col=1)

        lineSizer = wx.BoxSizer(wx.VERTICAL)    # let the line expand horizontally without vertical expansion
        lineSizer.Add(wx.StaticLine(tab, -1, size = (10,-1)), 0, wx.EXPAND)
        gridSizer.Add(lineSizer, flag=wx.EXPAND|wx.ALIGN_CENTER_VERTICAL|wx.TOP, border=HALF_SPACE, row=3, col=0, colspan=2)

        gridSizer.Add(wx.StaticText(tab, -1, _("Created:")), flag=wx.RIGHT, border=HALF_SPACE, row=4, col=0)
        if fileExists:
            gridSizer.Add(wx.StaticText(tab, -1, time.ctime(os.path.getctime(filename))), row=4, col=1)

        gridSizer.Add(wx.StaticText(tab, -1, _("Modified:")), flag=wx.RIGHT, border=HALF_SPACE, row=5, col=0)
        if fileExists:
            gridSizer.Add(wx.StaticText(tab, -1, time.ctime(os.path.getmtime(filename))), row=5, col=1)

        gridSizer.Add(wx.StaticText(tab, -1, _("Accessed:")), flag=wx.RIGHT, border=HALF_SPACE, row=6, col=0)
        if fileExists:
            gridSizer.Add(wx.StaticText(tab, -1, time.ctime(os.path.getatime(filename))), row=6, col=1)

        # add a border around the inside of the tab
        spacerGrid = wx.BoxSizer(wx.VERTICAL)
        spacerGrid.Add(gridSizer, 0, wx.ALL, SPACE);
        tab.SetSizer(spacerGrid)
        notebook.AddPage(tab, _("General"))

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(notebook, 0, wx.ALL | wx.EXPAND, SPACE)
        sizer.Add(self.CreateButtonSizer(wx.OK), 0, wx.ALIGN_RIGHT | wx.RIGHT | wx.BOTTOM, HALF_SPACE)

        sizer.Fit(self)
        self.SetDimensions(-1, -1, 310, -1, wx.SIZE_USE_EXISTING)
        self.SetSizer(sizer)
        self.Layout()


class ChildDocument(wx.lib.docview.Document):
    """
    A ChildDocument is a document that represents a portion of a Document.  The child
    document is managed by the parent document, so it will be prompted to close if its
    parent is closed, etc.  Child Documents are useful when there are complicated
    Views of a Document and users will need to tunnel into the View.
    """


    def GetData(self):
        """
        Returns the data that the ChildDocument contains.
        """
        return self._data


    def SetData(self, data):
        """
        Sets the data that the ChildDocument contains.
        """
        self._data = data


    def GetParentDocument(self):
        """
        Returns the parent Document of the ChildDocument.
        """
        return self._parentDocument


    def SetParentDocument(self, parentDocument):
        """
        Sets the parent Document of the ChildDocument.
        """
        self._parentDocument = parentDocument


    def OnSaveDocument(self, filename):
        """
        Called when the ChildDocument is saved and does the minimum such that the
        ChildDocument looks like a real Document to the framework.
        """
        self.SetFilename(filename, True)
        self.Modify(False)
        self.SetDocumentSaved(True)
        return True


    def OnOpenDocument(self, filename):
        """
        Called when the ChildDocument is opened and does the minimum such that the
        ChildDocument looks like a real Document to the framework.
        """
        self.SetFilename(filename, True)
        self.Modify(False)
        self.SetDocumentSaved(True)
        self.UpdateAllViews()
        return True


    def Save(self):
        """
        Called when the ChildDocument is saved and does the minimum such that the
        ChildDocument looks like a real Document to the framework.
        """
        return self.OnSaveDocument(self._documentFile)


    def SaveAs(self):
        """
        Called when the ChildDocument is saved and does the minimum such that the
        ChildDocument looks like a real Document to the framework.
        """
        return self.OnSaveDocument(self._documentFile)


class ChildDocTemplate(wx.lib.docview.DocTemplate):
    """
    A ChildDocTemplate is a DocTemplate subclass that enables the creation of ChildDocuments
    that represents a portion of a Document.  The child document is managed by the parent document,
    so it will be prompted to close if its parent is closed, etc.  Child Documents are useful
    when there are complicated  Views of a Document and users will need to tunnel into the View.
    """


    def __init__(self, manager, description, filter, dir, ext, docTypeName, viewTypeName, docType, viewType, flags=wx.lib.docview.TEMPLATE_INVISIBLE, icon=None):
        """
        Initializes the ChildDocTemplate.
        """
        wx.lib.docview.DocTemplate.__init__(self, manager, description, filter, dir, ext, docTypeName, viewTypeName, docType, viewType, flags=flags, icon=icon)


    def CreateDocument(self, path, flags, data=None, parentDocument=None):
        """
        Called when a ChildDocument is to be created and does the minimum such that the
        ChildDocument looks like a real Document to the framework.
        """
        doc = self._docType()
        doc.SetFilename(path)
        doc.SetData(data)
        doc.SetParentDocument(parentDocument)
        doc.SetDocumentTemplate(self)
        self.GetDocumentManager().AddDocument(doc)
        doc.SetCommandProcessor(doc.OnCreateCommandProcessor())
        if doc.OnCreate(path, flags):
            return doc
        else:
            if doc in self.GetDocumentManager().GetDocuments():
                doc.DeleteAllViews()
            return None


class WindowMenuService(DocService):
    """
    The WindowMenuService is a service that implements a standard Window menu that is used
    by the DocSDIFrame.  The MDIFrame automatically includes a Window menu and does not use
    the WindowMenuService.
    """

    #----------------------------------------------------------------------------
    # Constants
    #----------------------------------------------------------------------------
    ARRANGE_WINDOWS_ID = wx.NewId()
    SELECT_MORE_WINDOWS_ID = wx.NewId()
    SELECT_NEXT_WINDOW_ID = wx.NewId()
    SELECT_PREV_WINDOW_ID = wx.NewId()
    CLOSE_CURRENT_WINDOW_ID = wx.NewId()


    def __init__(self):
        """
        Initializes the WindowMenu and its globals.
        """
        self._selectWinIds = []
        for i in range(0, 9):
            self._selectWinIds.append(wx.NewId())


    def InstallControls(self, frame, menuBar=None, toolBar=None, statusBar=None, document=None):
        """
        Installs the Window menu.
        """

        windowMenu = None
        if hasattr(frame, "GetWindowMenu"):
            windowMenu = frame.GetWindowMenu()
        if not windowMenu:
            needWindowMenu = True
            windowMenu = wx.Menu()
        else:
            needWindowMenu = False
            
        if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
            if not _WINDOWS:  # Arrange All and window navigation doesn't work on Linux
                return
                
            item = windowMenu.Append(self.ARRANGE_WINDOWS_ID, _("&Arrange All"), _("Arrange the open windows"))
            wx.EVT_MENU(frame, self.ARRANGE_WINDOWS_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, self.ARRANGE_WINDOWS_ID, frame.ProcessUpdateUIEvent)
            windowMenu.AppendSeparator()

            for i, id in enumerate(self._selectWinIds):
                wx.EVT_MENU(frame, id, frame.ProcessEvent)
            wx.EVT_MENU(frame, self.SELECT_MORE_WINDOWS_ID, frame.ProcessEvent)
        elif wx.GetApp().GetUseTabbedMDI():
            item = windowMenu.Append(self.SELECT_PREV_WINDOW_ID, _("Previous"), _("Previous Tab"))
            wx.EVT_MENU(frame, self.SELECT_PREV_WINDOW_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, self.SELECT_PREV_WINDOW_ID, frame.ProcessUpdateUIEvent)
            item = windowMenu.Append(self.SELECT_NEXT_WINDOW_ID, _("Next"), _("Next Tab"))
            wx.EVT_MENU(frame, self.SELECT_NEXT_WINDOW_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, self.SELECT_NEXT_WINDOW_ID, frame.ProcessUpdateUIEvent)
            item = windowMenu.Append(self.CLOSE_CURRENT_WINDOW_ID, _("Close Current\tCtrl+F4"), _("Close Current Tab"))
            wx.EVT_MENU(frame, self.CLOSE_CURRENT_WINDOW_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, self.CLOSE_CURRENT_WINDOW_ID, frame.ProcessUpdateUIEvent)
            self._sep = None

            for i, id in enumerate(self._selectWinIds):
                wx.EVT_MENU(frame, id, self.OnCtrlKeySelect)

        if needWindowMenu:
            helpMenuIndex = menuBar.FindMenu(_("&Help"))
            menuBar.Insert(helpMenuIndex, windowMenu, _("&Window"))

        self._lastFrameUpdated = None


    def OnCtrlKeySelect(self, event):
        i = self._selectWinIds.index(event.GetId())
        notebook = wx.GetApp().GetTopWindow()._notebook
        if i < notebook.GetPageCount():
            notebook.SetSelection(i)


    def ProcessEvent(self, event):
        """
        Processes a Window menu event.
        """
        id = event.GetId()
        if id == self.ARRANGE_WINDOWS_ID:
            self.OnArrangeWindows(event)
            return True
        elif id == self.SELECT_MORE_WINDOWS_ID:
            self.OnSelectMoreWindows(event)
            return True
        elif id in self._selectWinIds:
            self.OnSelectWindowMenu(event)
            return True
        elif wx.GetApp().GetUseTabbedMDI():
            if id == self.SELECT_NEXT_WINDOW_ID:
                notebook = wx.GetApp().GetTopWindow()._notebook
                i = notebook.GetSelection()
                notebook.SetSelection(i+1)
                return True
            elif id == self.SELECT_PREV_WINDOW_ID:
                notebook = wx.GetApp().GetTopWindow()._notebook
                i = notebook.GetSelection()
                notebook.SetSelection(i-1)
                return True
            elif id == self.CLOSE_CURRENT_WINDOW_ID:
                notebook = wx.GetApp().GetTopWindow()._notebook
                i = notebook.GetSelection()
                if i != -1:
                    doc = notebook.GetPage(i).GetView().GetDocument()
                    wx.GetApp().GetDocumentManager().CloseDocument(doc, False)
                return True
        else:
            return False


    def ProcessUpdateUIEvent(self, event):
        """
        Updates the Window menu items.
        """
        id = event.GetId()
        if id == self.ARRANGE_WINDOWS_ID:
            frame = event.GetEventObject()
            if not self._lastFrameUpdated or self._lastFrameUpdated != frame:
                self.BuildWindowMenu(frame)  # It's a new frame, so update the windows menu... this is as if the View::OnActivateMethod had been invoked
                self._lastFrameUpdated = frame
            return True
        elif wx.GetApp().GetUseTabbedMDI():
            if id == self.SELECT_NEXT_WINDOW_ID:
                self.BuildWindowMenu(event.GetEventObject())  # build file list only when we are updating the windows menu
                
                notebook = wx.GetApp().GetTopWindow()._notebook
                i = notebook.GetSelection()
                if i == -1:
                    event.Enable(False)
                    return True
                i += 1
                if i >= notebook.GetPageCount():
                    event.Enable(False)
                    return True
                event.Enable(True)
                return True
            elif id == self.SELECT_PREV_WINDOW_ID:
                notebook = wx.GetApp().GetTopWindow()._notebook
                i = notebook.GetSelection()
                if i == -1:
                    event.Enable(False)
                    return True
                i -= 1
                if i < 0:
                    event.Enable(False)
                    return True
                event.Enable(True)
                return True
            elif id == self.CLOSE_CURRENT_WINDOW_ID:
                event.Enable(wx.GetApp().GetTopWindow()._notebook.GetSelection() != -1)
                return True

            return False
        else:
            return False


    def BuildWindowMenu(self, currentFrame):
        """
        Builds the Window menu and adds menu items for all of the open documents in the DocManager.
        """
        if wx.GetApp().GetUseTabbedMDI():
            currentFrame = wx.GetApp().GetTopWindow()

        windowMenuIndex = currentFrame.GetMenuBar().FindMenu(_("&Window"))
        windowMenu = currentFrame.GetMenuBar().GetMenu(windowMenuIndex)

        if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
            frames = self._GetWindowMenuFrameList(currentFrame)
            max = WINDOW_MENU_NUM_ITEMS
            if max > len(frames):
                max = len(frames)
            i = 0
            for i in range(0, max):
                frame = frames[i]
                item = windowMenu.FindItemById(self._selectWinIds[i])
                label = '&' + str(i + 1) + ' ' + frame.GetTitle()
                if not item:
                    item = windowMenu.AppendCheckItem(self._selectWinIds[i], label)
                else:
                    windowMenu.SetLabel(self._selectWinIds[i], label)
                windowMenu.Check(self._selectWinIds[i], (frame == currentFrame))
            if len(frames) > WINDOW_MENU_NUM_ITEMS:  # Add the more items item
                if not windowMenu.FindItemById(self.SELECT_MORE_WINDOWS_ID):
                    windowMenu.Append(self.SELECT_MORE_WINDOWS_ID, _("&More Windows..."))
            else:  # Remove any extra items
                if windowMenu.FindItemById(self.SELECT_MORE_WINDOWS_ID):
                    windowMenu.Remove(self.SELECT_MORE_WINDOWS_ID)

                for j in range(i + 1, WINDOW_MENU_NUM_ITEMS):
                    if windowMenu.FindItemById(self._selectWinIds[j]):
                        windowMenu.Remove(self._selectWinIds[j])

        elif wx.GetApp().GetUseTabbedMDI():
            notebook = wx.GetApp().GetTopWindow()._notebook
            numPages = notebook.GetPageCount()

            for id in self._selectWinIds:
                item = windowMenu.FindItemById(id)
                if item:
                    windowMenu.DeleteItem(item)
            if numPages == 0 and self._sep:
                windowMenu.DeleteItem(self._sep)
                self._sep = None

            if numPages > len(self._selectWinIds):
                for i in range(len(self._selectWinIds), numPages):
                    self._selectWinIds.append(wx.NewId())
                    wx.EVT_MENU(currentFrame, self._selectWinIds[i], self.OnCtrlKeySelect)                    
            
            for i in range(0, numPages):
                if i == 0 and not self._sep:
                    self._sep = windowMenu.AppendSeparator()
                if i < 9:
                    menuLabel = "%s\tCtrl+%s" % (notebook.GetPageText(i), i+1)
                else:
                    menuLabel = notebook.GetPageText(i)
                windowMenu.Append(self._selectWinIds[i], menuLabel)


    def _GetWindowMenuIDList(self):
        """
        Returns a list of the Window menu item IDs.
        """
        return self._selectWinIds


    def _GetWindowMenuFrameList(self, currentFrame=None):
        """
        Returns the Frame associated with each menu item in the Window menu.
        """
        frameList = []
        # get list of windows for documents
        for doc in self._docManager.GetDocuments():
            for view in doc.GetViews():
                frame = view.GetFrame()
                if frame not in frameList:
                    if frame == currentFrame and len(frameList) >= WINDOW_MENU_NUM_ITEMS:
                        frameList.insert(WINDOW_MENU_NUM_ITEMS - 1, frame)
                    else:
                        frameList.append(frame)
        # get list of windows for general services
        for service in wx.GetApp().GetServices():
            view = service.GetView()
            if view:
                frame = view.GetFrame()
                if frame not in frameList:
                    if frame == currentFrame and len(frameList) >= WINDOW_MENU_NUM_ITEMS:
                        frameList.insert(WINDOW_MENU_NUM_ITEMS - 1, frame)
                    else:
                        frameList.append(frame)

        return frameList


    def OnArrangeWindows(self, event):
        """
        Called by Window/Arrange and tiles the frames on the desktop.
        """
        currentFrame = event.GetEventObject()

        tempFrame = wx.Frame(None, -1, "", pos = wx.DefaultPosition, size = wx.DefaultSize)
        sizex = tempFrame.GetSize()[0]
        sizey = tempFrame.GetSize()[1]
        tempFrame.Destroy()

        posx = 0
        posy = 0
        delta = 0
        frames = self._GetWindowMenuFrameList()
        frames.remove(currentFrame)
        frames.append(currentFrame) # Make the current frame the last frame so that it is the last one to appear
        for frame in frames:
            if delta == 0:
                delta = frame.GetClientAreaOrigin()[1]
            frame.SetPosition((posx, posy))
            frame.SetSize((sizex, sizey))
            # TODO: Need to loop around if posx + delta + size > displaysize
            frame.SetFocus()
            posx = posx + delta
            posy = posy + delta
            if posx + sizex > wx.DisplaySize()[0] or posy + sizey > wx.DisplaySize()[1]:
                posx = 0
                posy = 0
        currentFrame.SetFocus()


    def OnSelectWindowMenu(self, event):
        """
        Called when the Window menu item representing a Frame is selected and brings the selected
        Frame to the front of the desktop.
        """
        id = event.GetId()
        index = self._selectWinIds.index(id)
        if index > -1:
            currentFrame = event.GetEventObject()
            frame = self._GetWindowMenuFrameList(currentFrame)[index]
            if frame:
                wx.CallAfter(frame.Raise)


    def OnSelectMoreWindows(self, event):
        """
        Called when the "Window/Select More Windows..." menu item is selected and enables user to
        select from the Frames that do not in the Window list.  Useful when there are more than
        10 open frames in the application.
        """
        frames = self._GetWindowMenuFrameList()  # TODO - make the current window the first one
        strings = map(lambda frame: frame.GetTitle(), frames)
        # Should preselect the current window, but not supported by wx.GetSingleChoice
        res = wx.GetSingleChoiceIndex(_("Select a window to show:"),
                                      _("Select Window"),
                                      strings,
                                      self)
        if res == -1:
            return
        frames[res].SetFocus()


#----------------------------------------------------------------------------
# File generated by encode_bitmaps.py
#----------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
import cStringIO

#----------------------------------------------------------------------
def getNewData():
    return \
"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\x04IDAT8\x8d\xa5\x93\xbdj\x02A\x10\xc7\x7f{gme\xe5c\xe4\t\x82\x85\
\x85\x85oa\xe5+\xd8Z\xd8'e\xfa\x80\xd8\xd8X\x19R\xc4\x07\x90\x04\xd1J\x08\
\x17\x0cr\\V\xe1\xe4\xfc\x80\xb58\xf7\xd8\xbd\x0f\xa280\xec\xec2\xbf\xff\xce\
\xcc\xb2B8.\xf7X\xc9\xdc|L\x97J\xc7\xbe\x0c\x01\xf0\xd6\x01\x00RFtZu\x91Q\
\x10\x8e\x9b\xf8\xe4\xf3[-w*\xf1\xafm\xec\xcf\x83\x89\x1a\xad\x94\xea\xbe\
\x8c\x95\x99/\x1c\x17\xe7\xdaR\xcb%xh\xd4hw_\x95yn\xb5\xe0\xcb\x90\xea%\x0eO\
\xf1\xba\xd9\xc7\xe5\xbf\x0f\xdfX]\xda)\x140A\r\x03<6klO\xf0w\x84~\xef\xc9\
\xca/lA\xc3@\x02\xe7\x99U\x81\xb7\x0e\xa8\xec\xed\x04\x13\xde\x1c\xfe\x11\
\x902\xb2@\xc8\xc2\x8b\xd9\xbcX\xc0\x045\xac\xc1 Jg\xe6\x08\xe8)\xa7o\xd5\
\xb0\xbf\xcb\nd\x86x\x0b\x9c+p\x0b\x0c\xa9\x16~\xbc_\xeb\x9d\xd3\x03\xcb3q\
\xefo\xbc\xfa/\x14\xd9\x19\x1f\xfb\x8aa\x87\xf2\xf7\x16\x00\x00\x00\x00IEND\
\xaeB`\x82"

def getNewBitmap():
    return BitmapFromImage(getNewImage())

def getNewImage():
    stream = cStringIO.StringIO(getNewData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getOpenData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01gIDAT8\x8d\xa5\x93=KBQ\x18\xc7\x7fWo)5\x1594DC\x04!\xd1\x0bM-\xd1\
\xd0T\x81\xba\xb7\xf8\x01Z\x9a\xdb\xfa\x08AC\x10\x0e\xb5\x86\xbaDC`CMaN\xd9\
\x0bQF7\xe2z\xc1kz\xcd\xc4\x97\xd3\xa0\xde\xbc\\oE\xfd\xa7s\xce\xf3\xfc\x7f\
\xe7y\xce\x8b$\xb9\xdc\xfcG2@\xf1bC\x00\x18%\xcd\x12\x1c^\xdc\x97~\x04\x18\
\xe7K\xa2of\x05\x80\xfe\x8e@\xc3\xc8\xf2zJ\x13\xac+\xe6\xfax(a\x81\xca\xa2w\
\x8a\x86\x91\x85\xaanE\xf7\x0c\xe0\xf3\xcf\x03P}|3\x97\x93\x11U\xcc\x85\xd3&\
D\xee\xf4\x88\xb2\xfa5)\xab(\x99"\x00\xb9\x87c\x0b;\x19\xf1\x0b\x80\xb9pZ\
\xb2\x00\x00\xd3T\xcb\xa5\x00(\xe4Uf\xd7\xb6m\xbd\xa7\x0e\xd6\x89\xc7\xa2\
\xc2\x04<_\xdf\xe3\x15\x1a\xb5V\xbfc\xab\x9b6S7\xc9FIC\xbf\xcb\xe0\x15\x1a\
\xbe\xe9e|\xad@C\xbfu4\x9d\xecnQ\x99\xdci\x02\x00\xea\x1f\x1a\x15]a\xa8pcK\
\xae\xbf?9\x82\x02\xc1\x90$\x1b\xba\x82<\xe8\xeb\x9a\\\xcb)\xdd|\x14r\x15<\
\xad\xb1\xab\x99\x98bdb\xd4q\xa7\xefd\xbb\x05\xa7\xdd\x8f\x0e/\x9d\x01\x85\
\xbc\nX+8K\\\x99\xe5\x02x\x16\xf6\xba\x02$\xc9\xe56\x1fF[\xda\x8bn\x9er\xa7\
\x02\xc1\x90\xedoH\xed\xdf\x18\x8fE\xc5o\x0c\x8e\x80\xbf\xea\x13\xa8\x18\x89\
5\xe7L\xb3:\x00\x00\x00\x00IEND\xaeB`\x82'

def getOpenBitmap():
    return BitmapFromImage(getOpenImage())

def getOpenImage():
    stream = cStringIO.StringIO(getOpenData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getCopyData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01_IDAT8\x8d\x8d\x92\xbfK\x02a\x18\xc7?w\xfa\'\xd8\xd0\xa0\xe4v\xd0$M\
\x8dB\x11\x11\xa5B\x7f@C\xd0RC{k8E\x834\xb45\n\x15\xfd\x80hhh\xd2\xadI\x82\
\xa4!\xb8\x84\xca\xd4;\xa5\xf2R\xe1m\xd0\xfb\xf5^\x1e~\xe1\xe5^\x9e{\xbe\x9f\
\xf7\xfb\xbcwJ\xa9\xa2\x0bFj\x98\xdf\x00\xd4\xea\x06\x00f\xdbbosQ!L\xa5\x8a.\
\xaa_"\xb0\x8e\xce\xcb\xa2\xfc)\xc4N\xfeT(j\x84\xb1\xabT\xd1E,\x19w\x80\x8d\
\x97Ww?A"\xd5n\xf2*\x96\x8c\x13K\xc6\xd1R\x1aZJcai\x1e\x80\xf4j\x9a\xed\xfd\
\xa2\xf0\x01B\xe7\x1b\xa9\xd9\x1d>;\x03X\xd9X\xf7AToC\xb3\xeb\xc6\x96e\xb6-\
\x1en\xef\xb999\x03\xe0\xea\xf2B\x00Dku\x83)\xcd\x85\x8c;}n9\r\x80\xd1\x87b\
\xbe\x00\xb33\xc3\x04f\xdbr\x9a;\x03\xbfI\x86\x1a\xfd\xe0\x01\xaam\xec\x0c\
\x86\r\xf6\x8d{\xcd\xf6;\x00\xb3\'\x01\xde?\x9a>\xba\x9cH6\xb7,x~\xaa:=Q\x9f\
\xb9\xe7\x1fE\xae\xb7\\\xb6\x1f\xe0\x8d\x15H$\x99\x1b?\x12@\xd7\xdf\xd0\x0f\
\nN!\x91\x98\x9e\xd8\x0c\x10\xbd>\xdeU\xeco\np\xf7\xf8\xebK\x14fvF\xc8ds\xce\
\xff\xbd\xb6u(\xbc\x89\xbc\x17\xf6\x9f\x14E\x8d\x04\x8a\xdeDa\xcads\xca\x1f\
\x0cI\xd4\xda\x88E\x9d\xc4\x00\x00\x00\x00IEND\xaeB`\x82'

def getCopyBitmap():
    return BitmapFromImage(getCopyImage())

def getCopyImage():
    stream = cStringIO.StringIO(getCopyData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getPasteData():
    return \
"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\x90IDAT8\x8d\x8d\x93?H\x02a\x18\x87\x9fSw\xb1\xa9!q\xc8\xb0?B\xd0\
\x98\x10DS\x10\x98\x82C\xd1\x12\x1a\xcd\rb&\xad\x1a\x144F`[\xd4 hBPKC\x83P\
\x8b4\xe4\xa9tP\x82\x98\x88`$\x82\x8b\xd8p\xddu\xa7\xa5\xfd\x96{\xbf\xef\xfd\
\xbd\xcf\xf7~w\xf7\n\x82\xc1\x08@M\xect\xd1(x\x12ef\xcaN./\x11\\\xdc\xd3\xa6\
pz\x8d\x82\x12\x0b\x82\xc1HM\xect-c\xf7\xaa!\x10\xc9\xe0]rR\xac\xb4\x01\xc8\
\xe5%\xe2\xbbF5_|\x0c\xa9\x10\x03=\nD2\x00$\xef\x9e\xc9\xe5%ryI\xde?\xe8\xe8\
|\xe9\xabT\x17\xc0\xd4\x0b\xd8\nl\xa8q\xfd\xa3%\xb7\xd9x\xe1\xad=\xc2q\xba\
\xc2\x8e\xfbU\xe7\xef\x03\x00\x98m\xd6\xef\xa7\xb23\xc9\xdbm\x06\xfb\x8a\x8f\
\xe0y\x8a\xc0\xc4\x10\x00\xc0\xcdEB\x8d\x97\xd7}j\xbc\xb0\xe6!~\x99d\xd11\
\x04\xa0-R$]'\xa84M4\xca\x05p8\x7f\x07\xd4?Z\x98mr\x07\x95\xa6\x9c\xf6o{\xb0\
\xce\xbb\x00\xb0\x03\xe9\xc3\xd8\xf0+h;x\xf9\xfc\xcb\xd5\x0bh>Pzw1>\x0bg\xa7\
)]\xaaQ.\x00`\xdb\x0c\x0f\x00hN\xf4o{~=\xf9\xa9\x0eY\xb1\x8awI\xf3\x0ej\x05\
\xb0\x98\x1f\x00x-\xd5\xb0\xce\xc3\xd1~LW\x98\x15\xab\xccM\x8f\xfe\xaf\x03\
\x00w0\xccS\xfdgm\xfb\xc3\xd7\xf7++w\xd5\x16\x0f\x92\t\xe4\xe9zN\x86\xbe\xa7\
1\xaa\xfbLY\xb1:\x10 (\xe3\x0c?\x03\xf2_\xb9W=\xc2\x17\x1c\xf8\x87\x9a\x03\
\x12\xd7\xb9\x00\x00\x00\x00IEND\xaeB`\x82"

def getPasteBitmap():
    return BitmapFromImage(getPasteImage())

def getPasteImage():
    stream = cStringIO.StringIO(getPasteData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getSaveData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\x1dIDAT8\x8d\x9d\x93=N\xc3@\x10\x85\xbf\xf5\xa2-\xf1\x11\xa0\x8dC\
\x8f\x82\xa0\xe5\xa7\xa6\xe2\x04\xb4p\x00\x1a\xfb\x02\x11T\xf4\xa4\xa0\xc1\
\xc1\\\x01\x90R"\xc5\xa4\x89RD\x14\x04$\xa2@\x01\xb1\x04C\xe1\xc8\xb1`\x1dC^\
5;?\xef\xcd\x8cv\x94r4\xf1\xc5\xa7P\x82a\xff\xb7o\xfd@+\x94\xa3\xb9o"2\xa8K\
\x18\x86R\x84\xc1\x87\xc8\xdd\xf3X|\xdf\x17\x11\x91\x9bc$\x8a"q\xf2\x8cZk\
\xab\xfa\xd3\x18\x1e\xdf\x12\xba\xef\x06\x80\xdb\x13\x95\xc5\x1ckE\t\xd6\xb6\
\xf7\xec\x04I\x92\x94\xaa\xff\xc4\\\x1d\xf0\xd2\xfd\x1bA\x99:\xc0B\xfe\xb1\
\xbb\xf1@\x10\x043\xc5\x8f6\xaf\x00\xe8u\xc0]\x9e\x10\x0c\xfb@m\x92\xb0\xbf8\
\xcd\x1e\xb5\xacm\xdb;\x18\xb5\xc0]%8}\xcd\x85+\x99\xd5\x8e\xbf2\xfb\xfc\xb0\
g\x1f!U\xac\xe0y^\xe62\xc6p\xd6h\x14\x8e4s\x89\xc6\xa4\xcb[\xa9V\xffG\xa0\
\xb5\xce\x8a\x97j[\xb4\xe3\xb8\x90@)\'\xfd\xbe\xd7\xf5\xe2\x83\xeau\xec~w\'\
\x9a\x12\x00\\6\xc3\xd2\xab,\xec`^|\x03\xb6\xdf|Q.\xa7\x15\x89\x00\x00\x00\
\x00IEND\xaeB`\x82'

def getSaveBitmap():
    return BitmapFromImage(getSaveImage())

def getSaveImage():
    stream = cStringIO.StringIO(getSaveData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getSaveAllData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01UIDAT8\x8d\x9d\x93\xbfK\xc3@\x1c\xc5\xdf%\x01g\xeb \x1d\x8a\x8b\x83M\
\x11\xe9\x16\x8a\x8b\xff@\xa0\xdd\x14\'\x17\x17A2\xe9,\x08\xc9\x14\x82n.nn\
\x9a\xde?\xe0R;\xb88\x99v\xe8`\x86\n."\x81\xb6\xb4\xb4~\x1d\xd2\xc4^\x92j\
\xf5\x03\xc7\xfd~\xf7\xeeq\xc7<\x17\x84)\xa3\x1e\x04\x863\xfd\xf10\xac\xb7\
\x8fe&,\xf2\\\x10\xf9\x06q\xce)I\x7fL\xf4\xda\'2M\x93\x88\x88\x1e.@\x9csb\
\x92\x8c\xb8x.\xa8X6\xd0z\xb2c\xd1?9\x89\x1c\xfc\xd7\x89\x82\x04\xeb\x9f:Z\
\xf5l\';9\xe0\xf1\xea\x14\xca\x12\xb0\xe2\xebh8 ))\x00\x00\xc5\xb2\x81\x8e\
\xc4\xb1\xb5GB\xd9< \x14\xf6\t\xf7\xef&*Ga\xf6\x99\x02Y\x0c&\xc0\xc7\x08x\
\xe9\x01A\x10\xa0y\xc9\x16\x17\x98\xdd\x1cQ\xd1\x8d\x9f\x05<\xcf\x136\xcf#\
\x15b\xc4\xc9\xee\x1b,\xcb\x8a\xfbA\x10\xc4\xed\xf3\xc3\x01\x00\xc0o\x03J\
\xa9&\xb3\x86c\xd3r![\xe47\x14 |\x14\xcf\xb7\x13JNZ7\xab\xc2\xe9\xddn7\x9e\
\xbb>\xcb\x01\x98\xc9\xa0T\x93Y\x93\xdbH\xa2\xaa*4MC\xb5Z\xcdt \x84\x98\xfa(\
S\xf2\xf9\xfc\xdc+0&\xc9\xa9\xc1\x86\xf3}\x1d\xbf\r\xacm\x84\xf5\xc2\x02\x00\
Pw\xefR\x99d\xf1\x05z\x94\xd0b\xcb S\xf3\x00\x00\x00\x00IEND\xaeB`\x82'

def getSaveAllBitmap():
    return BitmapFromImage(getSaveAllImage())

def getSaveAllImage():
    stream = cStringIO.StringIO(getSaveAllData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getPrintData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\xa7IDAT8\x8d\xa5S=K\xc3P\x14=\xef\xb5 \x0e\xf6\x17\xb8$X\x10\xa7\
\x82\xb4\n]\x05A\x07\xebd%\xfe\x02\x97\x82\xe0\xa0\x83\xa3\x88\xb5E\xfd\x07j\
\x0bq\xea\x07\x18(8:5\x16\xa2H\xf1\x8bN\x12\tt\xe9\x8b\xddZ\x9eC|i\x93\xd4\
\x0f\xf0\xc0\xe1\xe6\xbe\xdc{\xde\xb9\xc9{\x84\xd0\x10\xfe\x83\xb0x8m\xf6\
\xb8i\xf7=/\xfb\xad\x07O\x9e]\x9f%\x01\x05BC 4\x84\x1d\xbd\xc7\xfdx\xb2\x1d^\
\x99\x9c\x1f\xe6\x8ey\xb5Z\xe5\xa2^\x90\n\xa1\x83\xb91\xb2{;p\xf0\xfc\xe1\
\xc4W\xdb\x89\xe3\xcb\x19\xa8\xaa\x8aJ\xb9\xc4\x87\r\xd0\xe1\xc4o\xf9/\x08\
\x03\xc0\xc5\xf9\x19\x07\x80\xfb\xaf\x9d\xc5\xae-6(4\xed>\x9aoA\x01zq~\xc6\
\x15E\x81\xa2(\xee\xe2\xd4\x84\x13\xe5H\xb0\xc1?\x06\x05\x80b\xb1\xe8\x16\
\xbc\xda\x0e[\xcc\xa1i\xf71\xfcw\xf2\xf9\xbcG\x84\x14\n\x05\x1e\x8b\xc5\xa0\
\xd5\xae\xb1\xbd\x95\x81eY#gm\xb7\xdb\x9e|cs\x1fw7\x97$lZm\xc4\x00,-. \x9b?\
\xc1tT\x1e)\xc0\x18C$\x12\x01c\xce\x87\xe9\xbe\xeb\xa8\x94K\x9cNGeh\xb5k\x00\
\x80\xd1\xa8#\x91H@\x96\xe5\x00%I\xc2\xe3K\x0b\x9a\xa6A\x92$W8\xbc\x92Z%\xeb\
\xe95n4\xea\x01\xab\x9dN\xc7\xe3"9\x1fGr>\xeeYs\x8fr:\x9d\x06c\x0c\x86ax\nL\
\xcb;\xbb\x1f\x84\xd0\x10*\xe5\x12WU\x15\xcd7`f\xf2\xc7z\x00\x80\xae\xeb\xc8\
\xe5rXI\xad\x12"nc\xa5\\\xe2{G*\xba\xef\xfa\xaf\x02\xa2\xd9u \xe0?\xe7\xdfA4\
\x03\xc0\'\xe3\x82\xc9\x18g\x90\x8e]\x00\x00\x00\x00IEND\xaeB`\x82'

def getPrintBitmap():
    return BitmapFromImage(getPrintImage())

def getPrintImage():
    stream = cStringIO.StringIO(getPrintData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getPrintPreviewData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01mIDAT8\x8d\x8d\x92\xbdK\x02a\x1c\xc7?w\x1a\x85E\x04588XHa\xd1\x14!AB\
\r.\xbd\x07m-By.M\xfe\x03N\x05\x0e\xed\xf9\x124\x045\x04\x15\xdc\xda&4\xb5DC\
J\x8a\x81E\t.\x82\x918\xd8\xf0pOw\xde\x19}\xe1\xe1w\xf7;>\xdf\xdf\xcbs\xca\
\xddC\xb9C\x97\x1e\x8bU\xf9\x9c\xd8]V\xba\xbf\x9b\xa5\x02\xf8\xa6\xc6-ge=\
\x0c@p)\xcc\xc1\xe1\xa5\xad\x80\xcd\xa0\x97\x86\xfb`5\xba\xf3\xa7\x89\xdb)Y\
\xff\x16\xf1"{%s\xb77\xd7\x9d\xcd\xadm\xdb86\x03\x03\x0eE\xc2\x04\xdbPk\xc1y\
2Edf\xday\x84\xe6\xdb\x93\x84\x8c\xd8h\x8bSk\xf5j\xdcdPj\x8eX`C\x06\x9c?\x8a\
\xe3\xef/\xa3\xeb:\xb1\xfd=\xdb.,#4\xdav\x18-m\x01b\xd0\xc9\xe6N\xe5.Ts\xcbN\
pz\x0e\xa2~\x91\x0bx\x00-m\xe9D-W>%h\xc0\x1f_\xbf\x15\xef\xeb\x90\xaf\xc1\
\xe2\x18x="\x82\xb8\x15\xd9\x81yYf\x18\xe0\xac"\xc0\xc0\x10\x84\xc6D4\xcb\
\xf2#u\xc3\xb2m`t\x00&\x07E4\xcb]x.QH\xa6\xec$\x13\xf83q^\xb44^\x8f\xb8\xa5"\
p\x9c\x88\xa3\x91\xe1\x9d5\x00\x14Eu\xc9y\x9c\xa4\xeb\xba\xe5}\xb6\x9a\x01`\
\xc1\x07\xf39\x97\xa2(\xaa\xab\x17+\xd5]\xe0\xf5dC\x9a\xfc\xcb\xc0\xc9\xd00\
\xf9\x011\xc9\x87\xf3\xb4\xd1t\xaf\x00\x00\x00\x00IEND\xaeB`\x82'

def getPrintPreviewBitmap():
    return BitmapFromImage(getPrintPreviewImage())

def getPrintPreviewImage():
    stream = cStringIO.StringIO(getPrintPreviewData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getCutData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01HIDAT8\x8d\x85\x92OK\x02Q\x14\xc5\x7f\xa3\x05}\x1b\xa1\xc0\x9d\xb4\
\xaaf6\x93a\x10\xe3^\x83l\xdf\xc6\xa5\x1bIA\xb4\xa0\x9cM\xe5"\x84\x18\xff\
\x108\xbb\xf0\x93\xb4v\x15h\xa9\xaf\x16\xaf\x85\xbcat^\xd3\x81\xb79\xf7\xdc\
\xf3\xce{\xf7b$\x92\x84O\xa7\xd3\x91\x9b\\\xf8\xd4\xeb\xb5\xb5z\x02\r\x9e\
\x1e\x1f\xa4\x8eo5\x1b\x12`\xd0\xef\x05u\xadA.\x97\xc3u\xef\xd7LZ\xcd\x86\
\xb4\xedlD\xab5\xd0A\x08\xc1l6e>_\xc4\x1b\x88o\x01@\xde\xc9\x07\x91k\xd7Ui\
\x9a\x96\xd6xk\x93(\x14\xce\r@\x1e\x1e\x1cE\xc4\x9e\xe7\x91J\xa58\xce\x9e\
\x18\x7f\x1a\x00,\x17\xab\x98\xb6\x9dE\x08!M\xd3\x8aDW0\x8cDR[P\xb1U\xa3\xef\
\x8f"\xb7C\xcc\'\xee\xbdw\xf1</h\xceL\x86Z\x9d\xf6\to\x17\xbb2m90z\xc6\xf7!3\
\x19\x92\xb6\x1c\xc6\xdd\xab\x886v\x8ci\xcb\t\x9a\x15\xc2K\xa45P\xb7\x17o+\
\x00,\xa6\x9f\x00\x14o+\xec\x9f\x15X\xba\x97\xf1\tTC\x1c\xfe]e\x80v\xa9\xcc\
\xb8\xeb2\xfb\xf8\xe2\xf5\xaeA\xbbT\xd6\xea"c\x1c\xf4{r\xfbe\xf5Y?\xa7\xd5\
\x80W\xd1w\n7k\xa3\xd4\xee\x81\x8a\x18\x16\xea8\x80_\\\xa2\x8b\x88!\xd2S\x08\
\x00\x00\x00\x00IEND\xaeB`\x82'

def getCutBitmap():
    return BitmapFromImage(getCutImage())

def getCutImage():
    stream = cStringIO.StringIO(getCutData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getUndoData():
    return \
"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\xa7IDAT8\x8d\xa5\x90\xbfK[Q\x18\x86\x9fs#\x8d\x7fBu\xc8\xd6\xc9\xc1\
\xa1\x83\xd0\x16\xa1C@*\x98\xc4\xa2\x12\xda\x8e5\x9b\x83\x04\x07Aph\x17)\x16\
\xdd\xd4\xc1\xa1Z\x1b\xc5&9\xa6P\xbaw\xa8\x9b\x9b\xa0S\xb0\xe2\x8f\\%1^\x8d\
\xde\xfa9\x84s\xf1\xea\xa5\x06<p\x86\xc3\xf9\x9e\xe7\xbc\xefQ\xca\n\xf1\x90\
\xd5t\xdf@\xba\x10\x95r\xcd\x01`\xee\xf5o\xd5\xb0 ]\x88\n@\xd7\xb3^\x00.\xaf\
\xce\xd8\x9d>\x10\x80\x1fC[\x9eH\x05UH\x17\xa2r\x13\xac\x9d_Pq\x8f\x01(96\
\xdf\x16\xd7X\xff\xb8\xaf\x02\x05\x066\xa0+5\xe6\xb3\x0b\x1c\xeeW\x00x\xd1\
\xf3\x14\x80\xaf\x93\xbf\xd8\xcb\xb8\xeaN\x05\xd3\xd7\xbc\x9a\xd1\xdf\x19\
\x8cL@\xa4~\x9f\x9a\xec\xa3\xb3\xa7\r\x80|.+>\xc1\xfb\xd5\xe72\xf0\xf2-U\xa7\
\xec\x83c\xf1\x84\xd79\x9f\xcbJj\xa9/\xf8\x13\xcb\xe7U.\xaf\xcep\xa5\x06P\
\x8f\x1d\xf1'\x8c\xc5\x13*\x9f\xcb\x8a'\xe8_l\x17\x80\xe57\x1b\xea\xd4\xae\
\xc7w\xfe9\x94\x1c\xdb\x83\x1e\x0f4\t\xc0^\xc6UFb\xee\xacS\xdba\xf8\xd5\x08\
\xdd\xd3O\xc4t7\xab\xb8m\x93Z\xf2w\xbe\xfdgJk-\xb3\xc5\x11\xc6\xde\x8dS\x95\
\x8a\xd7\xbf\xe4\xd8\xec\x9c\xecr\xb2Sfm\xf9\x0f3\xc9\x15\xdf\xcb^\x82X<\xa1\
\x06#\x13\x0c}\x1a\x06 \xdc\xfc\xc87\xf0?\xb8\x1e\xc1\n\xa1\xac\x10Zk\xe9\
\x18k\x95\x9fGS\xf2\xa58*\x9f7S\xd2\x92\x0c\x8b\xd6Z\xccL\xd0\xf6\x1d\xb4\
\xd6\xd2\x92\x0c\xcb\xea\xdf\x0f\r\xc1w\x047%\x8d\xc0\x81\x02#i\x04VV\x88k\
\x82\xbe\xde\xc2\xb0\xb2\xea\xa7\x00\x00\x00\x00IEND\xaeB`\x82"

def getUndoBitmap():
    return BitmapFromImage(getUndoImage())

def getUndoImage():
    stream = cStringIO.StringIO(getUndoData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getRedoData():
    return \
"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\x88IDAT8\x8d\xa5\x92\xc1K\x02A\x14\xc6\xbfQ\t\xbc\x14tJ\xfb\x0f2\
\x08\xbaD\xdd:\xe5!\xd2\xad$/\x82FP\x06\x99\x87\x04\xa1\x83D\x10\x0b\x85\xd4\
\xa9\x8c (\x82<\xad\xce\xa9\xff\xc0[\xd2)\xbcu\t\xb2\xd0\xa5\xb5\x94\x14z\
\x1dd\x87]\x1bBh\xe0\xc1\xf0\xde\xfb~3\xef\x9ba\xcc\xe1\xc4\x7f\x96K\x96\xdc\
\xd6\xfcd\xeeO\x94;\xd67\xc0\x14Fg\xd7E\xae~\xa5S\xe3\xd3@!\xfe(\x051s\x84m\
\xcdOV!\x004\xbf\r\x00\x80\xde\xae\xe2B\xbb\x94B\\\x00\x10\xb9\x9a\x12\xe2,W\
Eqc~S\xec\xd7\x94\x18\xaa\xafY*e^l\x10\x87\xf5\xb4,W\xb1<\x98\x16q\x98W\xa1\
\xb7\xab\x00\x80F\xa7\x0e\x00(\x164\xb2\x02\xc0\x1cN(\xb9qRr\xe3\xc49'\xe6p\
\xc2\x1a3\xfb\xa3t\xfb\xbcK\xe7O[\xa4V\xc2\xe4K\x0e\xdb\xfa\\\x00\x10\xf3\
\x1c\x00\x00\x02AEj\x94\xd11P\xffz\x93\x95\xba\x80^\xe1\xf4\xde\x08\x01@)\
\xf3\xc2\xdek-!\xae5u\xe8\xcf-\x00\x80gi\x80l\x1e\xf4\xae\xc4j\x14c\x89!1o\
\xad\xa9\x8b\xda\xc6\xf5\n\x16v&\xbb\x16\xc8~b\xb1\xa0\x91\xfa\x10G4\xb2h;\
\xbd\xd1\xfe\x10=\xfc\xe8\x1eg\x91\xbc\xfc\x06\x81\xa0\xc2\xd2\x13\xa789\xbe\
\x91\xde\xce\x14\x07\x82\nC\xaf\xeb\xd6\xe0\x9c\x93/9Lj%L\xa9\xf2\x1c\xa5\
\xcas\xe4\r\xb9m\xaf\xf0'\xc0\x84xCnR+\xe1_\xe2\xbe\x00V\x88\xec\x9f\xf4\x05\
0!\xb2\xfc\x0f\xe0\xc4\xb6\xad\x97R\xe5z\x00\x00\x00\x00IEND\xaeB`\x82"

def getRedoBitmap():
    return BitmapFromImage(getRedoImage())

def getRedoImage():
    stream = cStringIO.StringIO(getRedoData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------------

def getBlankData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00]IDAT8\x8d\xed\x931\x0e\xc00\x08\x03m\x92\xff\xff8q\x87\xb6C\x11\x89\
\xa8X:\xd4\x13\x03:\x1b\x01\xa45T\xd4\xefBsh\xd7Hk\xdc\x02\x00@\x8a\x19$\xa1\
9\x14A,\x95\xf3\x82G)\xd3\x00\xf24\xf7\x90\x1ev\x07\xee\x1e\xf4:\xc1J?\xe0\
\x0b\x80\xc7\x1d\xf8\x1dg\xc4\xea7\x96G8\x00\xa8\x91\x19(\x85#P\x7f\x00\x00\
\x00\x00IEND\xaeB`\x82'


def getBlankBitmap():
    return BitmapFromImage(getBlankImage())

def getBlankImage():
    stream = cStringIO.StringIO(getBlankData())
    return ImageFromStream(stream)

def getBlankIcon():
    return wx.IconFromBitmap(getBlankBitmap())


