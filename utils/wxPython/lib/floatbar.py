#----------------------------------------------------------------------------
# Name:         floatbar.py
# Purpose:      Contains floating toolbar class
#
# Author:       Bryn Keller
#
# Created:      10/4/99
#----------------------------------------------------------------------------
from wxPython.wx import *

class wxFloatBar(wxToolBar):
    """
    wxToolBar subclass which can be dragged off its frame and later
    replaced there.  Drag on the toolbar to release it, close it like
    a normal window to make it return to its original
    position. Programmatically, call SetFloatable(true) and then
    Float(true) to float, Float(false) to dock.
    """
    def __init__(self,*_args,**_kwargs):
        """
        In addition to the usual arguments, wxFloatBar accepts keyword
        args of: title(string): the title that should appear on the
        toolbar's frame when it is floating.  floatable(bool): whether
        user actions (i.e., dragging) can float the toolbar or not.
        """
        args = (self,) + _args
        apply(wxToolBar.__init__, args, _kwargs)
        if _kwargs.has_key('floatable'):
            self.floatable = _kwargs['floatable']
            assert type(self.floatable) == type(0)
        else:
            self.floatable = 0
        self.floating = 0
        if _kwargs.has_key('title'):
            self.title = _kwargs['title']
            assert type(self.title) == type("")
        else:
            self.title = ""
        EVT_MOUSE_EVENTS(self, self.OnMouse)
        self.parentframe = wxPyTypeCast(args[1], 'wxFrame')
    def IsFloatable(self):
        return self.floatable
    def SetFloatable(self, float):
        self.floatable = float
        #Find the size of a title bar.
        if not hasattr(self, 'titleheight'):
            test = wxFrame(NULL, -1, "TEST")
            test.SetClientSize(wxSize(0,0))
            self.titleheight = test.GetSizeTuple()[1]
            test.Destroy()
    def IsFloating(self):
        return self.floating
    def Realize(self):
        wxToolBar.Realize(self)
        self.barheight = -1
    def GetTitle(self):
        return self.title
    def SetTitle(self, title):
        self.title = title
        if self.IsFloating():
            self.floatframe.SetTitle(self.title)
    def GetHome(self):
        """
        Returns the frame which this toolbar will return to when
        docked, or the parent if currently docked.
        """
        if hasattr(self, 'parentframe'):
            return self.parentframe
        else:
            return wxPyTypeCast(self.GetParent(), 'wxFrame')
    def SetHome(self, frame):
        """
        Called when docked, this will remove the toolbar from its
        current frame and attach it to another.  If called when
        floating, it will dock to the frame specified when the toolbar
        window is closed.
        """
        if self.IsFloating():
            self.parentframe = frame
            self.floatframe.Reparent(frame)
        else:
            parent = wxPyTypeCast(self.GetParent(), 'wxFrame')
            self.Reparent(frame)
            parent.SetToolBar(None)
            size = parent.GetSize()
            parent.SetSize(wxSize(0,0))
            parent.SetSize(size)
            frame.SetToolBar(self)
            size = frame.GetSize()
            frame.SetSize(wxSize(0,0))
            frame.SetSize(size)
    def Float(self, bool):
        "Floats or docks the toolbar programmatically."
        if bool:
            self.parentframe = wxPyTypeCast(self.GetParent(), 'wxFrame')
            clientsize = self.parentframe.GetClientSizeTuple()
            self.floatframe = wxMiniFrame(self.parentframe, -1, self.title, wxDefaultPosition, wxDefaultSize, wxTHICK_FRAME)
            self.Reparent(self.floatframe)
            self.parentframe.SetToolBar(None)
            self.floating = 1
            size = self.parentframe.GetSize()
            self.parentframe.SetSize(wxSize(0,0))
            self.parentframe.SetSize(size)
            self.floatframe.SetToolBar(self)
            self.oldcolor = self.GetBackgroundColour()
            barsize = self.GetSizeTuple()
            self.floatframe.SetSize(wxSize(barsize[0], barsize[1] + self.titleheight))
            self.floatframe.SetClientSize(wxSize(barsize[0], barsize[1]))
            newpos = self.parentframe.GetPosition()
            newpos.y = newpos.y + self.titleheight
            self.floatframe.SetPosition(newpos)
            self.floatframe.Show(true)
            EVT_CLOSE(self.floatframe, self.OnDock)
#            EVT_MOVE(self.floatframe, self.OnMove)
        else:
            self.Reparent(self.parentframe)
            self.parentframe.SetToolBar(self)
            self.floating = 0
            self.floatframe.Destroy()
            size = self.parentframe.GetSize()
            self.parentframe.SetSize(wxSize(0,0))
            self.parentframe.SetSize(size)
            self.SetBackgroundColour(self.oldcolor)
    def OnDock(self, e):
        self.Float(0)
        if hasattr(self, 'oldpos'):
            del self.oldpos

    def OnMove(self, e):
        homepos = self.parentframe.GetPositionTuple()
        homepos = homepos[0], homepos[1] + self.titleheight
        floatpos = self.floatframe.GetPositionTuple()
        if abs(homepos[0]-floatpos[0]) < 35 and abs(homepos[1]-floatpos[1]) < 35:
            self._SetFauxBarVisible(true)
        else:
            self._SetFauxBarVisible(false)

    def OnMouse(self, e):
        if not self.IsFloatable():
            e.Skip()
            return
        if e.ButtonDown() or e.ButtonUp() or e.ButtonDClick(1) or e.ButtonDClick(2) or e.ButtonDClick(3):
            e.Skip()
        if e.ButtonDown():
            self.oldpos = (e.GetX(), e.GetY())
        if e.Entering():
            self.oldpos = (e.GetX(), e.GetY())
        if e.ButtonUp():
            if self.IsFloating():
                homepos = self.parentframe.GetPositionTuple()
                homepos = homepos[0], homepos[1] + self.titleheight
                floatpos = self.floatframe.GetPositionTuple()
                if abs(homepos[0]-floatpos[0]) < 25 and abs(homepos[1]-floatpos[1]) < 25:
                    self.Float(0)
                    return
        if self.IsFloatable():
            if e.Dragging():
                if not self.IsFloating():
                    self.Float(true)
                    self.oldpos = (e.GetX(), e.GetY())
                else:
                    if hasattr(self, 'oldpos'):
                        loc = self.floatframe.GetPosition()
                        pt = wxPoint(loc.x - (self.oldpos[0]-e.GetX()), loc.y - (self.oldpos[1]-e.GetY()))
                        self.floatframe.SetPosition(pt)

    def _SetFauxBarVisible(self, vis):
#        return
        if vis:
            if self.parentframe.GetToolBar() == None:
                if not hasattr(self, 'nullbar'):
                    self.nullbar = wxToolBar(self.parentframe, -1)
                print "Adding fauxbar."
                self.nullbar.Reparent(self.parentframe)
                print "Reparented."
                self.parentframe.SetToolBar(self.nullbar)
                print "Set toolbar"
                col = wxNamedColour("GREY")
                self.nullbar.SetBackgroundColour(col)
                print "Set color"
                size = self.parentframe.GetSize()
                self.parentframe.SetSize(wxSize(0,0))
                self.parentframe.SetSize(size)
                print "Set size"
            else:
                print self.parentframe.GetToolBar()
        else:
            if self.parentframe.GetToolBar() != None:
                print "Removing fauxbar"
                self.nullbar.Reparent(self.floatframe)
                self.parentframe.SetToolBar(None)
                size = self.parentframe.GetSize()
                self.parentframe.SetSize(wxSize(0,0))
                self.parentframe.SetSize(size)













