#----------------------------------------------------------------------------
# Name:         floatbar.py
# Purpose:      Contains floating toolbar class
#
# Author:       Bryn Keller
#
# Created:      10/4/99
#----------------------------------------------------------------------------
# 12/02/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 Compatability changes
#
# 12/07/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Added deprecation warning.
#
# 12/18/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxFloatBar -> FloatBar
#

"""
NOTE: This module is *not* supported in any way.  Use it however you
      wish, but be warned that dealing with any consequences is
      entirly up to you.
      --Robin
"""

import  warnings
import  wx

warningmsg = r"""\

################################################\
# This module is not supported in any way!      |
#                                               |
# See cource code for wx.lib.floatbar for more  |
# information.                                  |
################################################/

"""

warnings.warn(warningmsg, DeprecationWarning, stacklevel=2)
    
if wx.Platform == '__WXGTK__':
    #
    # For wxGTK all we have to do is set the wxTB_DOCKABLE flag
    #
    class FloatBar(wx.ToolBar):
        def __init__(self, parent, ID,
                     pos = wx.DefaultPosition,
                     size = wx.DefaultSize,
                     style = 0,
                     name = 'toolbar'):
            wx.ToolBar.__init__(self, parent, ID, pos, size,
                               style|wx.TB_DOCKABLE, name)

        # these other methods just become no-ops
        def SetFloatable(self, float):
            pass

        def IsFloating(self):
            return 1

        def GetTitle(self):
            return ""


        def SetTitle(self, title):
            pass

else:
    _DOCKTHRESHOLD = 25

    class FloatBar(wx.ToolBar):
        """
        wxToolBar subclass which can be dragged off its frame and later
        replaced there.  Drag on the toolbar to release it, close it like
        a normal window to make it return to its original
        position. Programmatically, call SetFloatable(True) and then
        Float(True) to float, Float(False) to dock.
        """

        def __init__(self,*_args,**_kwargs):
            """
            In addition to the usual arguments, wxFloatBar accepts keyword
            args of: title(string): the title that should appear on the
            toolbar's frame when it is floating.  floatable(bool): whether
            user actions (i.e., dragging) can float the toolbar or not.
            """
            args = (self,) + _args
            apply(wx.ToolBar.__init__, args, _kwargs)
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
            self.Bind(wx.EVT_MOUSE_EVENTS, self.OnMouse)
            self.parentframe = args[1]


        def IsFloatable(self):
            return self.floatable


        def SetFloatable(self, float):
            self.floatable = float
            #Find the size of a title bar.
            if not hasattr(self, 'titleheight'):
                test = wx.MiniFrame(None, -1, "TEST")
                test.SetClientSize((0,0))
                self.titleheight = test.GetSize()[1]
                test.Destroy()


        def IsFloating(self):
            return self.floating


        def Realize(self):
            wx.ToolBar.Realize(self)


        def GetTitle(self):
            return self.title


        def SetTitle(self, title):
            print 'SetTitle', title
            self.title = title
            if self.IsFloating():
                self.floatframe.SetTitle(self.title)


    ##     def GetHome(self):
    ##         """
    ##         Returns the frame which this toolbar will return to when
    ##         docked, or the parent if currently docked.
    ##         """
    ##         if hasattr(self, 'parentframe'):
    ##             return self.parentframe
    ##         else:
    ##             return (self.GetParent())


    ##     def SetHome(self, frame):
    ##         """
    ##         Called when docked, this will remove the toolbar from its
    ##         current frame and attach it to another.  If called when
    ##         floating, it will dock to the frame specified when the toolbar
    ##         window is closed.
    ##         """
    ##         if self.IsFloating():
    ##             self.parentframe = frame
    ##             self.floatframe.Reparent(frame)
    ##         else:
    ##             parent = self.GetParent()
    ##             self.Reparent(frame)
    ##             parent.SetToolBar(None)
    ##             size = parent.GetSize()
    ##             parent.SetSize(wxSize(0,0))
    ##             parent.SetSize(size)
    ##             frame.SetToolBar(self)
    ##             size = frame.GetSize()
    ##             frame.SetSize(wxSize(0,0))
    ##             frame.SetSize(size)


        def Float(self, bool):
            "Floats or docks the toolbar programmatically."
            if bool:
                self.parentframe = self.GetParent()
                print self.title
                if self.title:
                    useStyle = wx.DEFAULT_FRAME_STYLE
                else:
                    useStyle = wx.THICK_FRAME
                self.floatframe = wx.MiniFrame(self.parentframe, -1, self.title,
                                              style = useStyle)

                self.Reparent(self.floatframe)
                self.parentframe.SetToolBar(None)
                self.floating = 1
                psize = self.parentframe.GetSize()
                self.parentframe.SetSize((0,0))
                self.parentframe.SetSize(psize)
                self.floatframe.SetToolBar(self)
                self.oldcolor = self.GetBackgroundColour()

                w = psize[0]
                h = self.GetSize()[1]
                if self.title:
                    h = h + self.titleheight
                self.floatframe.SetSize((w,h))
                self.floatframe.SetClientSize(self.GetSize())
                newpos = self.parentframe.GetPosition()
                newpos.y = newpos.y + _DOCKTHRESHOLD * 2
                self.floatframe.SetPosition(newpos)
                self.floatframe.Show(True)

                self.floatframe.Bind(wx.EVT_CLOSE, self.OnDock)
                #self.floatframe.Bind(wx.EVT_MOVE, self.OnMove)

            else:
                self.Reparent(self.parentframe)
                self.parentframe.SetToolBar(self)
                self.floating = 0
                self.floatframe.SetToolBar(None)
                self.floatframe.Destroy()
                size = self.parentframe.GetSize()
                self.parentframe.SetSize((0,0))
                self.parentframe.SetSize(size)
                self.SetBackgroundColour(self.oldcolor)


        def OnDock(self, e):
            self.Float(0)
            if hasattr(self, 'oldpos'):
                del self.oldpos


        def OnMove(self, e):
            homepos = self.parentframe.ClientToScreen((0,0))
            floatpos = self.floatframe.GetPosition()
            if (abs(homepos.x - floatpos.x) < _DOCKTHRESHOLD and
                abs(homepos.y - floatpos.y) < _DOCKTHRESHOLD):
                self.Float(0)
            #homepos = self.parentframe.GetPositionTuple()
            #homepos = homepos[0], homepos[1] + self.titleheight
            #floatpos = self.floatframe.GetPositionTuple()
            #if abs(homepos[0] - floatpos[0]) < 35 and abs(homepos[1] - floatpos[1]) < 35:
            #    self._SetFauxBarVisible(True)
            #else:
            #    self._SetFauxBarVisible(False)


        def OnMouse(self, e):
            if not self.IsFloatable():
                e.Skip()
                return

            if e.ButtonDClick(1) or e.ButtonDClick(2) or e.ButtonDClick(3) or e.ButtonDown() or e.ButtonUp():
                e.Skip()

            if e.ButtonDown():
                self.CaptureMouse()
                self.oldpos = (e.GetX(), e.GetY())

            if e.Entering():
                self.oldpos = (e.GetX(), e.GetY())

            if e.ButtonUp():
                self.ReleaseMouse()
                if self.IsFloating():
                    homepos = self.parentframe.ClientToScreen((0,0))
                    floatpos = self.floatframe.GetPosition()
                    if (abs(homepos.x - floatpos.x) < _DOCKTHRESHOLD and
                        abs(homepos.y - floatpos.y) < _DOCKTHRESHOLD):
                        self.Float(0)
                        return

            if e.Dragging():
                if not self.IsFloating():
                    self.Float(True)
                    self.oldpos = (e.GetX(), e.GetY())
                else:
                    if hasattr(self, 'oldpos'):
                        loc = self.floatframe.GetPosition()
                        pt = (loc.x - (self.oldpos[0]-e.GetX()), loc.y - (self.oldpos[1]-e.GetY()))
                        self.floatframe.Move(pt)



        def _SetFauxBarVisible(self, vis):
            return
            if vis:
                if self.parentframe.GetToolBar() == None:
                    if not hasattr(self, 'nullbar'):
                        self.nullbar = wx.ToolBar(self.parentframe, -1)
                    print "Adding fauxbar."
                    self.nullbar.Reparent(self.parentframe)
                    print "Reparented."
                    self.parentframe.SetToolBar(self.nullbar)
                    print "Set toolbar"
                    col = wx.NamedColour("GREY")
                    self.nullbar.SetBackgroundColour(col)
                    print "Set color"
                    size = self.parentframe.GetSize()
                    self.parentframe.SetSize((0,0))
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
                    self.parentframe.SetSize((0,0))
                    self.parentframe.SetSize(size)



