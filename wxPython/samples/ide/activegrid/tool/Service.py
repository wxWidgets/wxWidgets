#----------------------------------------------------------------------------
# Name:         Service.py
# Purpose:      Basic Reusable Service View for wx.lib.pydocview
#
# Author:       Morgan Hua
#
# Created:      11/4/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import wx.lib.docview
import wx.lib.pydocview
_ = wx.GetTranslation


FLOATING_MINIFRAME = -1


class ServiceView(wx.EvtHandler):
    """ Basic Service View.
    """
    bottomTab = None
    
    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self, service):
        wx.EvtHandler.__init__(self)
        self._viewFrame = None
        self._service = service
        self._control = None
        self._embeddedWindow = None


    def Destroy(self):
        wx.EvtHandler.Destroy(self)


    def GetFrame(self):
        return self._viewFrame


    def SetFrame(self, frame):
        self._viewFrame = frame


    def _CreateControl(self, parent, id):
        return None
        
    
    def GetControl(self):
        return self._control


    def SetControl(self, control):
        self._control = control


    def OnCreate(self, doc, flags):
        config = wx.ConfigBase_Get()
        windowLoc = self._service.GetEmbeddedWindowLocation()
        if windowLoc == FLOATING_MINIFRAME:
            pos = config.ReadInt(self._service.GetServiceName() + "FrameXLoc", -1), config.ReadInt(self._service.GetServiceName() + "FrameYLoc", -1)
            # make sure frame is visible
            screenWidth = wx.SystemSettings.GetMetric(wx.SYS_SCREEN_X)
            screenHeight = wx.SystemSettings.GetMetric(wx.SYS_SCREEN_Y)
            if pos[0] < 0 or pos[0] >= screenWidth or pos[1] < 0 or pos[1] >= screenHeight:
                pos = wx.DefaultPosition

            size = wx.Size(config.ReadInt(self._service.GetServiceName() + "FrameXSize", -1), config.ReadInt(self._service.GetServiceName() + "FrameYSize", -1))
            title = _(self._service.GetServiceName())
            if wx.GetApp().GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI and wx.GetApp().GetAppName():
                title =  title + " - " + wx.GetApp().GetAppName()
            frame = wx.MiniFrame(wx.GetApp().GetTopWindow(), -1, title, pos = pos, size = size, style = wx.CLOSE_BOX|wx.CAPTION|wx.SYSTEM_MENU)
            wx.EVT_CLOSE(frame, self.OnCloseWindow)
        elif wx.GetApp().IsMDI():
            self._embeddedWindow = wx.GetApp().GetTopWindow().GetEmbeddedWindow(windowLoc)
            frame = self._embeddedWindow
        else:
            pos = config.ReadInt(self._service.GetServiceName() + "FrameXLoc", -1), config.ReadInt(self._service.GetServiceName() + "FrameYLoc", -1)
            # make sure frame is visible
            screenWidth = wx.SystemSettings.GetMetric(wx.SYS_SCREEN_X)
            screenHeight = wx.SystemSettings.GetMetric(wx.SYS_SCREEN_Y)
            if pos[0] < 0 or pos[0] >= screenWidth or pos[1] < 0 or pos[1] >= screenHeight:
                pos = wx.DefaultPosition

            size = wx.Size(config.ReadInt(self._service.GetServiceName() + "FrameXSize", -1), config.ReadInt(self._service.GetServiceName() + "FrameYSize", -1))
            title = _(self._service.GetServiceName())
            if wx.GetApp().GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI and wx.GetApp().GetAppName():
                title =  title + " - " + wx.GetApp().GetAppName()
            frame = wx.GetApp().CreateDocumentFrame(self, doc, flags, pos = pos, size = size)
            frame.SetTitle(title)
            if config.ReadInt(self._service.GetServiceName() + "FrameMaximized", False):
                frame.Maximize(True)
            wx.EVT_CLOSE(frame, self.OnCloseWindow)

        self.SetFrame(frame)
        sizer = wx.BoxSizer(wx.VERTICAL)
        
        windowLoc = self._service.GetEmbeddedWindowLocation()
        if self._embeddedWindow or windowLoc == FLOATING_MINIFRAME:
            if (self._service.GetEmbeddedWindowLocation() == wx.lib.pydocview.EMBEDDED_WINDOW_BOTTOM):
                if ServiceView.bottomTab == None:
                    ServiceView.bottomTab = wx.Notebook(frame, wx.NewId(), (0,0), (100,100), wx.LB_DEFAULT, "Bottom Tab")
                    sizer.Add(ServiceView.bottomTab, 1, wx.TOP|wx.EXPAND, 4)
                    def OnFrameResize(event):
                        ServiceView.bottomTab.SetSize(ServiceView.bottomTab.GetParent().GetSize())
                    frame.Bind(wx.EVT_SIZE, OnFrameResize)
                # Factor this out.
                self._control = self._CreateControl(ServiceView.bottomTab, wx.NewId())
                if self._control != None:
                    ServiceView.bottomTab.AddPage(self._control, self._service.GetServiceName())
                ServiceView.bottomTab.Layout()
            else:
                # Factor this out.
                self._control = self._CreateControl(frame, wx.NewId())
                sizer.Add(self._control)
        else:
            # Factor this out.
            self._control = self._CreateControl(frame, wx.NewId())
            sizer.Add(self._control, 1, wx.EXPAND, 0)
        frame.SetSizer(sizer)
        frame.Layout()

        return True


    def OnCloseWindow(self, event):
        frame = self.GetFrame()
        config = wx.ConfigBase_Get()
        if frame and not self._embeddedWindow:
            if not frame.IsMaximized():
                config.WriteInt(self._service.GetServiceName() + "FrameXLoc", frame.GetPositionTuple()[0])
                config.WriteInt(self._service.GetServiceName() + "FrameYLoc", frame.GetPositionTuple()[1])
                config.WriteInt(self._service.GetServiceName() + "FrameXSize", frame.GetSizeTuple()[0])
                config.WriteInt(self._service.GetServiceName() + "FrameYSize", frame.GetSizeTuple()[1])
            config.WriteInt(self._service.GetServiceName() + "FrameMaximized", frame.IsMaximized())

        if not self._embeddedWindow:
            windowLoc = self._service.GetEmbeddedWindowLocation()
            if windowLoc == FLOATING_MINIFRAME:
                # don't destroy it, just hide it
                frame.Hide()
            else:
                # Call the original OnCloseWindow, could have subclassed SDIDocFrame and MDIDocFrame but this is easier since it will work for both SDI and MDI frames without subclassing both
                frame.OnCloseWindow(event)


    def Activate(self, activate = True):
        """ Dummy function for SDI mode """
        pass


    def Close(self, deleteWindow = True):
        """
        Closes the view by calling OnClose. If deleteWindow is true, this
        function should delete the window associated with the view.
        """
        if deleteWindow:
            self.Destroy()

        return True


    #----------------------------------------------------------------------------
    # Callback Methods
    #----------------------------------------------------------------------------

    def SetCallback(self, callback):
        """ Sets in the event table for a doubleclick to invoke the given callback.
            Additional calls to this method overwrites the previous entry and only the last set callback will be invoked.
        """
        wx.stc.EVT_STC_DOUBLECLICK(self.GetControl(), self.GetControl().GetId(), callback)


    #----------------------------------------------------------------------------
    # Display Methods
    #----------------------------------------------------------------------------

    def IsShown(self):
        if not self.GetFrame():
            return False
        return self.GetFrame().IsShown()


    def Hide(self):
        self.Show(False)


    def Show(self, show = True):
        if self.GetFrame():
            self.GetFrame().Show(show)
            if self._embeddedWindow:
                mdiParentFrame = wx.GetApp().GetTopWindow()
                mdiParentFrame.ShowEmbeddedWindow(self.GetFrame(), show)


class Service(wx.lib.pydocview.DocService):


    #----------------------------------------------------------------------------
    # Constants
    #----------------------------------------------------------------------------
    SHOW_WINDOW = wx.NewId()  # keep this line for each subclass, need unique ID for each Service


    def __init__(self, serviceName, embeddedWindowLocation = wx.lib.pydocview.EMBEDDED_WINDOW_LEFT):
        self._serviceName = serviceName
        self._embeddedWindowLocation = embeddedWindowLocation
        self._view = None


    def GetEmbeddedWindowLocation(self):
        return self._embeddedWindowLocation


    def SetEmbeddedWindowLocation(self, embeddedWindowLocation):
        self._embeddedWindowLocation = embeddedWindowLocation


    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        viewMenu = menuBar.GetMenu(menuBar.FindMenu(_("&View")))
        menuItemPos = self.GetMenuItemPos(viewMenu, viewMenu.FindItem(_("&Status Bar"))) + 1

        viewMenu.InsertCheckItem(menuItemPos, self.SHOW_WINDOW, self.GetMenuString(), self.GetMenuDescr())
        wx.EVT_MENU(frame, self.SHOW_WINDOW, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, self.SHOW_WINDOW, frame.ProcessUpdateUIEvent)

        return True


    def GetServiceName(self):
        """ String used to save out Service View configuration information """
        return self._serviceName


    def GetMenuString(self):
        """ Need to override this method to provide menu item for showing Service View """
        return _(self.GetServiceName())


    def GetMenuDescr(self):
        """ Need to override this method to provide menu item for showing Service View """
        return _("Show or hides the %s window") % self.GetMenuString()


    #----------------------------------------------------------------------------
    # Event Processing Methods
    #----------------------------------------------------------------------------

    def ProcessEvent(self, event):
        id = event.GetId()
        if id == self.SHOW_WINDOW:
            self.ToggleWindow(event)
            return True
        else:
            return False


    def ProcessUpdateUIEvent(self, event):
        id = event.GetId()
        if id == self.SHOW_WINDOW:
            event.Check(self._view != None and self._view.IsShown())
            event.Enable(True)
            return True
        else:
            return False


    #----------------------------------------------------------------------------
    # View Methods
    #----------------------------------------------------------------------------

    def _CreateView(self):
        """ This method needs to be overridden with corresponding ServiceView """
        return ServiceView(self)


    def GetView(self):
        # Window Menu Service Method
        return self._view


    def SetView(self, view):
        self._view = view


    def ShowWindow(self, show = True):
        if show:
            if self._view:
                if not self._view.IsShown():
                    self._view.Show()
            else:
                view = self._CreateView()
                view.OnCreate(None, flags = 0)
                self.SetView(view)
        else:
            if self._view:
                if self._view.IsShown():
                    self._view.Hide()


    def HideWindow(self):
        self.ShowWindow(False)


    def ToggleWindow(self, event):
        show = event.IsChecked()
        wx.ConfigBase_Get().WriteInt(self.GetServiceName()+"Shown", show)
        self.ShowWindow(show)


    def OnCloseFrame(self, event):
        if not self._view:
            return True

        if wx.GetApp().IsMDI():
            self._view.OnCloseWindow(event)
        # This is called when any SDI frame is closed, so need to check if message window is closing or some other window
        elif self._view == event.GetEventObject().GetView():
            self.SetView(None)
        
        return True
        
