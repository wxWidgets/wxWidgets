"""
A Panel that includes the FloatCanvas and Navigation controls

"""

import wx

import FloatCanvas, Resources

ID_ZOOM_IN_BUTTON = wx.NewId()
ID_ZOOM_OUT_BUTTON = wx.NewId()
ID_ZOOM_TO_FIT_BUTTON = wx.NewId()
ID_MOVE_MODE_BUTTON = wx.NewId()
ID_POINTER_BUTTON = wx.NewId()


#---------------------------------------------------------------------------
    
class NavCanvas(wx.Panel):
    """
    NavCanvas.py

    This is a high level window that encloses the FloatCanvas in a panel
    and adds a Navigation toolbar.

    Copyright: wxWindows Software Foundation (Assigned by: Christopher Barker)

    License: Same as the version of wxPython you are using it with

    Please let me know if you're using this!!!

    Contact me at:

    Chris.Barker@noaa.gov

    """ 
    
    def __init__(self, parent, id = -1,
                 size = wx.DefaultSize,
                 **kwargs): # The rest just get passed into FloatCanvas

        wx.Panel.__init__( self, parent, id, wx.DefaultPosition, size)

        ## Create the vertical sizer for the toolbar and Panel
        box = wx.BoxSizer(wx.VERTICAL)
        box.Add(self.BuildToolbar(), 0, wx.ALL | wx.ALIGN_LEFT | wx.GROW, 4)
        
        self.Canvas = FloatCanvas.FloatCanvas( self, wx.NewId(),
                                   size = wx.DefaultSize,
                                   **kwargs)
        box.Add(self.Canvas,1,wx.GROW)

        box.Fit(self)
        self.SetSizer(box)

        # default to Mouse mode
        self.ToolBar.ToggleTool(ID_POINTER_BUTTON,1)
        self.Canvas.SetMode("Mouse")
        
        return None

    def __getattr__(self, name):
        """
        Delegate all extra methods to the Canvas
        """
        attrib = getattr(self.Canvas, name)
        ## add the attribute to this module's dict for future calls
        self.__dict__[name] = attrib
        return attrib

    def BuildToolbar(self):
        tb = wx.ToolBar(self,-1)
        self.ToolBar = tb
        
        tb.SetToolBitmapSize((23,23))
        
        tb.AddTool(ID_POINTER_BUTTON, Resources.GetPointerBitmap(), isToggle=True, shortHelpString = "Pointer")
        wx.EVT_TOOL(self, ID_POINTER_BUTTON, self.SetToolMode)

        tb.AddTool(ID_ZOOM_IN_BUTTON, Resources.GetPlusBitmap(), isToggle=True, shortHelpString = "Zoom In")
        wx.EVT_TOOL(self, ID_ZOOM_IN_BUTTON, self.SetToolMode)
        
        tb.AddTool(ID_ZOOM_OUT_BUTTON, Resources.GetMinusBitmap(), isToggle=True, shortHelpString = "Zoom Out")
        wx.EVT_TOOL(self, ID_ZOOM_OUT_BUTTON, self.SetToolMode)
        
        tb.AddTool(ID_MOVE_MODE_BUTTON, Resources.GetHandBitmap(), isToggle=True, shortHelpString = "Move")
        wx.EVT_TOOL(self, ID_MOVE_MODE_BUTTON, self.SetToolMode)
        
        tb.AddSeparator()
        
        tb.AddControl(wx.Button(tb, ID_ZOOM_TO_FIT_BUTTON, "Zoom To Fit",wx.DefaultPosition, wx.DefaultSize))
        wx.EVT_BUTTON(self, ID_ZOOM_TO_FIT_BUTTON, self.ZoomToFit)

        tb.Realize()
        S = tb.GetSize()
        tb.SetSizeHints(S[0],S[1])
        return tb

    def SetToolMode(self,event):
        for id in [ID_ZOOM_IN_BUTTON,
                   ID_ZOOM_OUT_BUTTON,
                   ID_MOVE_MODE_BUTTON,
                   ID_POINTER_BUTTON]:
            self.ToolBar.ToggleTool(id,0)
        self.ToolBar.ToggleTool(event.GetId(),1)
        if event.GetId() == ID_ZOOM_IN_BUTTON:
            self.Canvas.SetMode("ZoomIn")
        elif event.GetId() == ID_ZOOM_OUT_BUTTON:
            self.Canvas.SetMode("ZoomOut")
        elif event.GetId() == ID_MOVE_MODE_BUTTON:
            self.Canvas.SetMode("Move")
        elif event.GetId() == ID_POINTER_BUTTON:
            self.Canvas.SetMode("Mouse")


    def ZoomToFit(self,Event):
        self.Canvas.ZoomToBB()

