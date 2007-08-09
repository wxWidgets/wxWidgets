"""
A Panel that includes the FloatCanvas and Navigation controls

"""

import wx
import FloatCanvas, Resources


class NavCanvas(wx.Panel):
    """
    NavCanvas.py

    This is a high level window that encloses the FloatCanvas in a panel
    and adds a Navigation toolbar.

    """

    def __init__(self,
                   parent,
                   id = wx.ID_ANY,
                   size = wx.DefaultSize,
                   **kwargs): # The rest just get passed into FloatCanvas
        wx.Panel.__init__(self, parent, id, size=size)

        self.BuildToolbar()
        ## Create the vertical sizer for the toolbar and Panel
        box = wx.BoxSizer(wx.VERTICAL)
        box.Add(self.ToolBar, 0, wx.ALL | wx.ALIGN_LEFT | wx.GROW, 4)

        self.Canvas = FloatCanvas.FloatCanvas(self, **kwargs)
        box.Add(self.Canvas, 1, wx.GROW)

        self.SetSizerAndFit(box)


        import GUIMode # here so that it doesn't get imported before wx.App()
        self.GUIZoomIn  =  GUIMode.GUIZoomIn(self.Canvas)
        self.GUIZoomOut =  GUIMode.GUIZoomOut(self.Canvas)
        self.GUIMove    =  GUIMode.GUIMove(self.Canvas)
        self.GUIMouse   =  GUIMode.GUIMouse(self.Canvas)

        # default to Mouse mode
        self.ToolBar.ToggleTool(self.PointerTool.GetId(), True)
        self.Canvas.SetMode(self.GUIMouse)

        return None

    def BuildToolbar(self):
        tb = wx.ToolBar(self)
        self.ToolBar = tb
        tb.SetToolBitmapSize((24,24))

        self.PointerTool = tb.AddRadioTool(wx.ID_ANY, bitmap=Resources.getPointerBitmap(), shortHelp = "Pointer")
        self.Bind(wx.EVT_TOOL, lambda evt : self.SetMode(Mode=self.GUIMouse), self.PointerTool)

        self.ZoomInTool = tb.AddRadioTool(wx.ID_ANY, bitmap=Resources.getMagPlusBitmap(), shortHelp = "Zoom In")
        self.Bind(wx.EVT_TOOL, lambda evt : self.SetMode(Mode=self.GUIZoomIn), self.ZoomInTool)
    
        self.ZoomOutTool = tb.AddRadioTool(wx.ID_ANY, bitmap=Resources.getMagMinusBitmap(), shortHelp = "Zoom Out")
        self.Bind(wx.EVT_TOOL, lambda evt : self.SetMode(Mode=self.GUIZoomOut), self.ZoomOutTool)

        self.MoveTool = tb.AddRadioTool(wx.ID_ANY, bitmap=Resources.getHandBitmap(), shortHelp = "Move")
        self.Bind(wx.EVT_TOOL, lambda evt : self.SetMode(Mode=self.GUIMove), self.MoveTool)

        tb.AddSeparator()

        self.ZoomButton = wx.Button(tb, label="Zoom To Fit")
        tb.AddControl(self.ZoomButton)
        self.ZoomButton.Bind(wx.EVT_BUTTON, self.ZoomToFit)

        tb.Realize()
        ## fixme: remove this when the bug is fixed!
        wx.CallAfter(self.HideShowHack) # this required on wxPython 2.8.3 on OS-X

        return tb

    def HideShowHack(self):
        ##fixme: remove this when the bug is fixed!
        """
        Hack to hide and show button on toolbar to get around OS-X bug on
        wxPython2.8 on OS-X
        """
        self.ZoomButton.Hide()
        self.ZoomButton.Show()

    def SetMode(self, Mode):
        self.Canvas.SetMode(Mode)

    def ZoomToFit(self,Event):
        self.Canvas.ZoomToBB()
        self.Canvas.SetFocus() # Otherwise the focus stays on the Button, and wheel events are lost.

