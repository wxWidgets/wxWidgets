#----------------------------------------------------------------------
# Name:        wx.lib.splitter
# Purpose:     A class similar to wx.SplitterWindow but that allows more
#              than a single split
#
# Author:      Robin Dunn
#
# Created:     9-June-2005
# RCS-ID:      $Id$
# Copyright:   (c) 2005 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------
"""
This module provides the `MultiSplitterWindow` class, which is very
similar to the standard `wx.SplitterWindow` except it can be split
more than once.
"""

import wx

_RENDER_VER = (2,6,1,1)

#----------------------------------------------------------------------

class MultiSplitterWindow(wx.PyPanel):
    """
    This class is very similar to `wx.SplitterWindow` except that it
    allows for more than two windows and more than one sash.  Many of
    the same styles, constants, and methods behave the same as in
    wx.SplitterWindow.  The key differences are seen in the methods
    that deal with the child windows managed by the splitter, and also
    those that deal with the sash positions.  In most cases you will
    need to pass an index value to tell the class which window or sash
    you are refering to.

    The concept of the sash position is also different than in
    wx.SplitterWindow.  Since the wx.Splitterwindow has only one sash
    you can think of it's position as either relative to the whole
    splitter window, or as relative to the first window pane managed
    by the splitter.  Once there is more than one sash then the
    distinciton between the two concepts needs to be clairified.  I've
    chosen to use the second definition, and sash positions are the
    distance (either horizontally or vertically) from the origin of
    the window just before the sash in the splitter stack.

    NOTE: These things are not yet supported:

        * Using negative sash positions to indicate a position offset
          from the end.
          
        * User controlled unsplitting (with double clicks on the sash
          or dragging a sash until the pane size is zero.)
          
        * Sash gravity
       
    """
    def __init__(self, parent, id=-1,
                 pos = wx.DefaultPosition, size = wx.DefaultSize,
                 style = 0, name="multiSplitter"):
        
        # always turn on tab traversal
        style |= wx.TAB_TRAVERSAL

        # and turn off any border styles
        style &= ~wx.BORDER_MASK
        style |= wx.BORDER_NONE

        # initialize the base class
        wx.PyPanel.__init__(self, parent, id, pos, size, style, name)
        self.SetBackgroundStyle(wx.BG_STYLE_CUSTOM)

        # initialize data members
        self._windows = []
        self._sashes = []
        self._pending = {}
        self._permitUnsplitAlways = self.HasFlag(wx.SP_PERMIT_UNSPLIT)
        self._orient = wx.HORIZONTAL
        self._dragMode = wx.SPLIT_DRAG_NONE
        self._activeSash = -1
        self._oldX = 0
        self._oldY = 0
        self._checkRequestedSashPosition = False
        self._minimumPaneSize = 0
        self._sashCursorWE = wx.StockCursor(wx.CURSOR_SIZEWE)
        self._sashCursorNS = wx.StockCursor(wx.CURSOR_SIZENS)
        self._sashTrackerPen = wx.Pen(wx.BLACK, 2, wx.SOLID)
        self._needUpdating = False
        self._isHot = False

        # Bind event handlers
        self.Bind(wx.EVT_PAINT,        self._OnPaint)
        self.Bind(wx.EVT_IDLE,         self._OnIdle)
        self.Bind(wx.EVT_SIZE,         self._OnSize)
        self.Bind(wx.EVT_MOUSE_EVENTS, self._OnMouse)



    def SetOrientation(self, orient):
        """
        Set whether the windows managed by the splitter will be
        stacked vertically or horizontally.  The default is
        horizontal.
        """
        assert orient in [ wx.VERTICAL, wx.HORIZONTAL ]
        self._orient = orient

    def GetOrientation(self):
        """
        Returns the current orientation of the splitter, either
        wx.VERTICAL or wx.HORIZONTAL.
        """
        return self._orient


    def SetMinimumPaneSize(self, minSize):
        """
        Set the smallest size that any pane will be allowed to be
        resized to.
        """
        self._minimumPaneSize = minSize

    def GetMinimumPaneSize(self):
        """
        Returns the smallest allowed size for a window pane.
        """
        return self._minimumPaneSize

    

    def AppendWindow(self, window, sashPos=-1):
        """
        Add a new window to the splitter at the right side or bottom
        of the window stack.  If sashPos is given then it is used to
        size the new window.
        """
        self.InsertWindow(len(self._windows), window, sashPos)


    def InsertWindow(self, idx, window, sashPos=-1):
        """
        Insert a new window into the splitter at the position given in
        ``idx``.
        """
        assert window not in self._windows, "A window can only be in the splitter once!"
        self._windows.insert(idx, window)
        self._sashes.insert(idx, -1)
        if not window.IsShown():
            window.Show()
        if sashPos != -1:
            self._pending[window] = sashPos
        self._checkRequestedSashPosition = False
        self._SizeWindows()


    def DetachWindow(self, window):
        """
        Removes the window from the stack of windows managed by the
        splitter.  The window will still exist so you should `Hide` or
        `Destroy` it as needed.
        """
        assert window in self._windows, "Unknown window!"
        idx = self._windows.index(window)
        del self._windows[idx]
        del self._sashes[idx]
        self._SizeWindows()


    def ReplaceWindow(self, oldWindow, newWindow):
        """
        Replaces oldWindow (which is currently being managed by the
        splitter) with newWindow.  The oldWindow window will still
        exist so you should `Hide` or `Destroy` it as needed.
        """
        assert oldWindow in self._windows, "Unknown window!"
        idx = self._windows.index(oldWindow)
        self._windows[idx] = newWindow
        if not newWindow.IsShown():
            newWindow.Show()
        self._SizeWindows()


    def ExchangeWindows(self, window1, window2):
        """
        Trade the positions in the splitter of the two windows.
        """
        assert window1 in self._windows, "Unknown window!"
        assert window2 in self._windows, "Unknown window!"
        idx1 = self._windows.index(window1)
        idx2 = self._windows.index(window2)
        self._windows[idx1] = window2
        self._windows[idx2] = window1
        self._SizeWindows()


    def GetWindow(self, idx):
        """
        Returns the idx'th window being managed by the splitter.
        """
        assert idx < len(self._windows)
        return self._windows[idx]


    def GetSashPosition(self, idx):
        """
        Returns the position of the idx'th sash, measured from the
        left/top of the window preceding the sash.
        """
        assert idx < len(self._sashes)
        return self._sashes[idx]


    def SetSashPosition(self, idx, pos):
        """
        Set the psition of the idx'th sash, measured from the left/top
        of the window preceding the sash.
        """
        assert idx < len(self._sashes)
        self._sashes[idx] = pos
        self._SizeWindows()
        

    def SizeWindows(self):
        """
        Reposition and size the windows managed by the splitter.
        Useful when windows have been added/removed or when styles
        have been changed.
        """
        self._SizeWindows()
        

    def DoGetBestSize(self):
        """
        Overridden base class virtual.  Determines the best size of
        the control based on the best sizes of the child windows.
        """
        best = wx.Size(0,0)
        if not self._windows:
            best = wx.Size(10,10)

        sashsize = self._GetSashSize()
        if self._orient == wx.HORIZONTAL:
            for win in self._windows:
                winbest = win.GetEffectiveMinSize()
                best.width += max(self._minimumPaneSize, winbest.width)
                best.height = max(best.height, winbest.height)
            best.width += sashsize * (len(self._windows)-1)

        else:
            for win in self._windows:
                winbest = win.GetEffectiveMinSize()
                best.height += max(self._minimumPaneSize, winbest.height)
                best.width = max(best.width, winbest.width)
            best.height += sashsize * (len(self._windows)-1)
            
        border = 2 * self._GetBorderSize()
        best.width += border
        best.height += border
        return best

    # -------------------------------------
    # Event handlers
    
    def _OnPaint(self, evt):
        dc = wx.PaintDC(self)
        self._DrawSash(dc)


    def _OnSize(self, evt):
        parent = wx.GetTopLevelParent(self)
        if parent.IsIconized():
            evt.Skip()
            return
        self._SizeWindows()


    def _OnIdle(self, evt):
        evt.Skip()
        # if this is the first idle time after a sash position has
        # potentially been set, allow _SizeWindows to check for a
        # requested size.
        if not self._checkRequestedSashPosition:
            self._checkRequestedSashPosition = True
            self._SizeWindows()

        if self._needUpdating:
            self._SizeWindows()

            

    def _OnMouse(self, evt):
        if self.HasFlag(wx.SP_NOSASH):
            return

        x, y = evt.GetPosition()
        isLive = self.HasFlag(wx.SP_LIVE_UPDATE)
        adjustNeighbor = evt.ShiftDown()

        # LeftDown: set things up for dragging the sash
        if evt.LeftDown() and self._SashHitTest(x, y) != -1:
            self._activeSash = self._SashHitTest(x, y)
            self._dragMode = wx.SPLIT_DRAG_DRAGGING

            self.CaptureMouse()
            self._SetResizeCursor()

            if not isLive:
                self._pendingPos = (self._sashes[self._activeSash],
                                    self._sashes[self._activeSash+1])
                self._DrawSashTracker(x, y)

            self._oldX = x
            self._oldY = y
            return

        # LeftUp: Finsish the drag
        elif evt.LeftUp() and self._dragMode == wx.SPLIT_DRAG_DRAGGING:
            self._dragMode = wx.SPLIT_DRAG_NONE
            self.ReleaseMouse()
            self.SetCursor(wx.STANDARD_CURSOR)

            if not isLive:
                # erase the old tracker
                self._DrawSashTracker(self._oldX, self._oldY)

            diff = self._GetMotionDiff(x, y)

            # determine if we can change the position
            if isLive:
                oldPos1, oldPos2 = (self._sashes[self._activeSash],
                                    self._sashes[self._activeSash+1])
            else:
                oldPos1, oldPos2 = self._pendingPos
            newPos1, newPos2 = self._OnSashPositionChanging(self._activeSash,
                                                            oldPos1 + diff,
                                                            oldPos2 - diff,
                                                            adjustNeighbor)
            if newPos1 == -1:
                # the change was not allowed
                return

            # TODO: check for unsplit?

            self._SetSashPositionAndNotify(self._activeSash, newPos1, newPos2, adjustNeighbor)
            self._activeSash = -1
            self._pendingPos = (-1, -1)
            self._SizeWindows()

        # Entering or Leaving a sash: Change the cursor
        elif (evt.Moving() or evt.Leaving() or evt.Entering()) and self._dragMode == wx.SPLIT_DRAG_NONE:
            if evt.Leaving() or self._SashHitTest(x, y) == -1:
                self._OnLeaveSash()
            else:
                self._OnEnterSash()

        # Dragging the sash
        elif evt.Dragging() and self._dragMode == wx.SPLIT_DRAG_DRAGGING:
            diff = self._GetMotionDiff(x, y)
            if not diff:
                return  # mouse didn't move far enough

            # determine if we can change the position
            if isLive:
                oldPos1, oldPos2 = (self._sashes[self._activeSash],
                                    self._sashes[self._activeSash+1])
            else:
                oldPos1, oldPos2 = self._pendingPos
            newPos1, newPos2 = self._OnSashPositionChanging(self._activeSash,
                                                            oldPos1 + diff,
                                                            oldPos2 - diff,
                                                            adjustNeighbor)
            if newPos1 == -1:
                # the change was not allowed
                return

            if newPos1 == self._sashes[self._activeSash]:
                return  # nothing was changed

            if not isLive:
                # erase the old tracker
                self._DrawSashTracker(self._oldX, self._oldY)
           
            if self._orient == wx.HORIZONTAL:
                 x = self._SashToCoord(self._activeSash, newPos1)
            else:
                 y = self._SashToCoord(self._activeSash, newPos1)

            # Remember old positions
            self._oldX = x
            self._oldY = y

            if not isLive:
                # draw a new tracker
                self._pendingPos = (newPos1, newPos2)
                self._DrawSashTracker(self._oldX, self._oldY)
            else:
                self._DoSetSashPosition(self._activeSash, newPos1, newPos2, adjustNeighbor)
                self._needUpdating = True


    # -------------------------------------
    # Internal helpers
    
    def _RedrawIfHotSensitive(self, isHot):
        if not wx.VERSION >= _RENDER_VER:
            return
        if wx.RendererNative.Get().GetSplitterParams(self).isHotSensitive:
            self._isHot = isHot
            dc = wx.ClientDC(self)
            self._DrawSash(dc)


    def _OnEnterSash(self):
        self._SetResizeCursor()
        self._RedrawIfHotSensitive(True)


    def _OnLeaveSash(self):
        self.SetCursor(wx.STANDARD_CURSOR)
        self._RedrawIfHotSensitive(False)


    def _SetResizeCursor(self):
        if self._orient == wx.HORIZONTAL:
            self.SetCursor(self._sashCursorWE)
        else:
            self.SetCursor(self._sashCursorNS)


    def _OnSashPositionChanging(self, idx, newPos1, newPos2, adjustNeighbor):
        # TODO: check for possibility of unsplit (pane size becomes zero)

        # make sure that minsizes are honored
        newPos1, newPos2 = self._AdjustSashPosition(idx, newPos1, newPos2, adjustNeighbor)

        # sanity check
        if newPos1 <= 0:
            newPos2 += newPos1
            newPos1 = 0 

        # send the events
        evt = MultiSplitterEvent(
            wx.wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING, self)
        evt.SetSashIdx(idx)
        evt.SetSashPosition(newPos1)
        if not self._DoSendEvent(evt):
            # the event handler vetoed the change
            newPos1 = -1
        else:
            # or it might have changed the value
            newPos1 = evt.GetSashPosition()

        if adjustNeighbor and newPos1 != -1:
            evt.SetSashIdx(idx+1)
            evt.SetSashPosition(newPos2)
            if not self._DoSendEvent(evt):
                # the event handler vetoed the change
                newPos2 = -1
            else:
                # or it might have changed the value
                newPos2 = evt.GetSashPosition()
            if newPos2 == -1:
                newPos1 = -1

        return (newPos1, newPos2)


    def _AdjustSashPosition(self, idx, newPos1, newPos2=-1, adjustNeighbor=False):
        total = newPos1 + newPos2

        # these are the windows on either side of the sash
        win1 = self._windows[idx]
        win2 = self._windows[idx+1]

        # make adjustments for window min sizes
        minSize = self._GetWindowMin(win1)
        if minSize == -1 or self._minimumPaneSize > minSize:
            minSize = self._minimumPaneSize
        minSize += self._GetBorderSize()
        if newPos1 < minSize:
            newPos1 = minSize
            newPos2 = total - newPos1

        if adjustNeighbor:
            minSize = self._GetWindowMin(win2)
            if minSize == -1 or self._minimumPaneSize > minSize:
                minSize = self._minimumPaneSize
            minSize += self._GetBorderSize()
            if newPos2 < minSize:
                newPos2 = minSize
                newPos1 = total - newPos2
        
        return (newPos1, newPos2)


    def _DoSetSashPosition(self, idx, newPos1, newPos2=-1, adjustNeighbor=False):
        newPos1, newPos2 = self._AdjustSashPosition(idx, newPos1, newPos2, adjustNeighbor)
        if newPos1 == self._sashes[idx]:
            return False
        self._sashes[idx] = newPos1
        if adjustNeighbor:
            self._sashes[idx+1] = newPos2
        return True
        

    def _SetSashPositionAndNotify(self, idx, newPos1, newPos2=-1, adjustNeighbor=False):
        # TODO:  what is the thing about _requestedSashPosition for?

        self._DoSetSashPosition(idx, newPos1, newPos2, adjustNeighbor)

        evt = MultiSplitterEvent(
            wx.wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, self)
        evt.SetSashIdx(idx)
        evt.SetSashPosition(newPos1)
        self._DoSendEvent(evt)

        if adjustNeighbor:
            evt.SetSashIdx(idx+1)
            evt.SetSashPosition(newPos2)
            self._DoSendEvent(evt)


    def _GetMotionDiff(self, x, y):
        # find the diff from the old pos
        if self._orient == wx.HORIZONTAL:
            diff = x - self._oldX
        else:
            diff = y - self._oldY
        return diff


    def _SashToCoord(self, idx, sashPos):
        coord = 0
        for i in range(idx):
            coord += self._sashes[i]
            coord += self._GetSashSize()
        coord += sashPos
        return coord


    def _GetWindowMin(self, window):
        if self._orient == wx.HORIZONTAL:
            return window.GetMinWidth()
        else:
            return window.GetMinHeight()
        
            
    def _GetSashSize(self):
        if self.HasFlag(wx.SP_NOSASH):
            return 0
        if wx.VERSION >= _RENDER_VER:
            return wx.RendererNative.Get().GetSplitterParams(self).widthSash
        else:
            return 5


    def _GetBorderSize(self):
        if wx.VERSION >= _RENDER_VER:
            return wx.RendererNative.Get().GetSplitterParams(self).border
        else:
            return 0
        

    def _DrawSash(self, dc):
        if wx.VERSION >= _RENDER_VER:
            if self.HasFlag(wx.SP_3DBORDER):
                wx.RendererNative.Get().DrawSplitterBorder(
                    self, dc, self.GetClientRect())

        # if there are no splits then we're done.
        if len(self._windows) < 2:
            return

        # if we are not supposed to use a sash then we're done.
        if self.HasFlag(wx.SP_NOSASH):
            return

        # Reverse the sense of the orientation, in this case it refers
        # to the direction to draw the sash not the direction that
        # windows are stacked.
        orient = { wx.HORIZONTAL : wx.VERTICAL,
                   wx.VERTICAL : wx.HORIZONTAL }[self._orient]

        flag = 0
        if self._isHot:
            flag = wx.CONTROL_CURRENT

        pos = 0
        for sash in self._sashes[:-1]:
            pos += sash
            if wx.VERSION >= _RENDER_VER:
                wx.RendererNative.Get().DrawSplitterSash(self, dc,
                                                         self.GetClientSize(),
                                                         pos, orient, flag)
            else:
                dc.SetPen(wx.TRANSPARENT_PEN)
                dc.SetBrush(wx.Brush(self.GetBackgroundColour()))
                sashsize = self._GetSashSize()
                if orient == wx.VERTICAL:
                    x = pos
                    y = 0
                    w = sashsize
                    h = self.GetClientSize().height
                else:
                    x = 0
                    y = pos
                    w = self.GetClientSize().width
                    h = sashsize
                dc.DrawRectangle(x, y, w, h)

            pos += self._GetSashSize()


    def _DrawSashTracker(self, x, y):
        # Draw a line to represent the dragging sash, for when not
        # doing live updates
        w, h = self.GetClientSize()
        dc = wx.ScreenDC()

        if self._orient == wx.HORIZONTAL:
            x1 = x
            y1 = 2
            x2 = x
            y2 = h-2
            if x1 > w:
                x1 = w
                x2 = w
            elif x1 < 0:
                x1 = 0
                x2 = 0
        else:
            x1 = 2
            y1 = y
            x2 = w-2
            y2 = y
            if y1 > h:
                y1 = h
                y2 = h
            elif y1 < 0:
                y1 = 0
                y2 = 0

        x1, y1 = self.ClientToScreenXY(x1, y1)
        x2, y2 = self.ClientToScreenXY(x2, y2)
     
        dc.SetLogicalFunction(wx.INVERT)
        dc.SetPen(self._sashTrackerPen)
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.DrawLine(x1, y1, x2, y2)
        dc.SetLogicalFunction(wx.COPY)
        

    def _SashHitTest(self, x, y, tolerance=5):
        # if there are no splits then we're done.
        if len(self._windows) < 2:
            return -1

        if self._orient == wx.HORIZONTAL:
            z = x
        else:
            z = y

        pos = 0
        for idx, sash in enumerate(self._sashes[:-1]):
            pos += sash
            hitMin = pos - tolerance
            hitMax = pos + self._GetSashSize() + tolerance
            
            if z >= hitMin and z <= hitMax:
                return idx
            
            pos += self._GetSashSize() 

        return -1


    def _SizeWindows(self):
        # no windows yet?
        if not self._windows:
            return

        # are there any pending size settings?
        for window, spos in self._pending.items():
            idx = self._windows.index(window)
            # TODO: this may need adjusted to make sure they all fit
            # in the current client size
            self._sashes[idx] = spos
            del self._pending[window]

        # are there any that still have a -1?
        for idx, spos in enumerate(self._sashes[:-1]):
            if spos == -1:
                # TODO: this should also be adjusted
                self._sashes[idx] = 100
        
        cw, ch = self.GetClientSize()
        border = self._GetBorderSize()
        sash   = self._GetSashSize()
        
        if len(self._windows) == 1:
            # there's only one, it's an easy layout
            self._windows[0].SetDimensions(border, border,
                                           cw - 2*border, ch - 2*border)
        else:
            if 'wxMSW' in wx.PlatformInfo:
                self.Freeze()
            if self._orient == wx.HORIZONTAL:
                x = y = border
                h = ch - 2*border
                for idx, spos in enumerate(self._sashes[:-1]):
                    self._windows[idx].SetDimensions(x, y, spos, h)
                    x += spos + sash
                # last one takes the rest of the space. TODO make this configurable
                last = cw - 2*border - x
                self._windows[idx+1].SetDimensions(x, y, last, h)
                if last > 0:
                    self._sashes[idx+1] = last
            else:
                x = y = border
                w = cw - 2*border
                for idx, spos in enumerate(self._sashes[:-1]):
                    self._windows[idx].SetDimensions(x, y, w, spos)
                    y += spos + sash
                # last one takes the rest of the space. TODO make this configurable
                last = ch - 2*border - y
                self._windows[idx+1].SetDimensions(x, y, w, last)
                if last > 0:
                    self._sashes[idx+1] = last
            if 'wxMSW' in wx.PlatformInfo:
                self.Thaw()
                
        self._DrawSash(wx.ClientDC(self))
        self._needUpdating = False


    def _DoSendEvent(self, evt):
        return not self.GetEventHandler().ProcessEvent(evt) or evt.IsAllowed()
    
#----------------------------------------------------------------------

class MultiSplitterEvent(wx.PyCommandEvent):
    """
    This event class is almost the same as `wx.SplitterEvent` except
    it adds an accessor for the sash index that is being changed.  The
    same event type IDs and event binders are used as with
    `wx.SplitterEvent`.
    """
    def __init__(self, type=wx.wxEVT_NULL, splitter=None):
        wx.PyCommandEvent.__init__(self, type)
        if splitter:
            self.SetEventObject(splitter)
            self.SetId(splitter.GetId())
        self.sashIdx = -1
        self.sashPos = -1
        self.isAllowed = True

    def SetSashIdx(self, idx):
        self.sashIdx = idx

    def SetSashPosition(self, pos):
        self.sashPos = pos

    def GetSashIdx(self):
        return self.sashIdx

    def GetSashPosition(self):
        return self.sashPos

    # methods from wx.NotifyEvent
    def Veto(self):
        self.isAllowed = False
    def Allow(self):
        self.isAllowed = True
    def IsAllowed(self):
        return self.isAllowed
        


#----------------------------------------------------------------------



