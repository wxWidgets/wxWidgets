"""
A throbber displays an animated image that can be
started, stopped, reversed, etc.  Useful for showing
an ongoing process (like most web browsers use) or
simply for adding eye-candy to an application.

Throbbers utilize a wxTimer so that normal processing
can continue unencumbered.
"""

#
# throbber.py - Cliff Wells <clifford.wells@comcast.net>
#
# Thanks to Harald Massa <harald.massa@suedvers.de> for
# suggestions and sample code.
#
# $Id$
#
# 12/12/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
#


import os
import wx

# ------------------------------------------------------------------------------

THROBBER_EVENT = wx.NewEventType()
EVT_UPDATE_THROBBER = wx.PyEventBinder(THROBBER_EVENT, 0)

class UpdateThrobberEvent(wx.PyEvent):
    def __init__(self):
        wx.PyEvent.__init__(self)
        self.SetEventType(THROBBER_EVENT)

# ------------------------------------------------------------------------------

class Throbber(wx.PyPanel):
    """
    The first argument is either the name of a file that will be split into frames
    (a composite image) or a list of  strings of image names that will be treated
    as individual frames.  If a single (composite) image is given, then additional
    information must be provided: the number of frames in the image and the width
    of each frame.  The first frame is treated as the "at rest" frame (it is not
    shown during animation, but only when Throbber.Rest() is called.
    A second, single image may be optionally specified to overlay on top of the
    animation. A label may also be specified to show on top of the animation.
    """
    def __init__(self, parent, id,
                 bitmap,          # single (composite) bitmap or list of bitmaps
                 pos = wx.DefaultPosition,
                 size = wx.DefaultSize,
                 frameDelay = 0.1,# time between frames
                 frames = 0,      # number of frames (only necessary for composite image)
                 frameWidth = 0,  # width of each frame (only necessary for composite image)
                 label = None,    # optional text to be displayed
                 overlay = None,  # optional image to overlay on animation
                 reverse = 0,     # reverse direction at end of animation
                 style = 0,       # window style
                 name = "throbber"):
        wx.PyPanel.__init__(self, parent, id, pos, size, style, name)
        self.name = name
        self.label = label
        self.running = (1 != 1)
        _seqTypes = (type([]), type(()))

        # set size, guessing if necessary
        width, height = size
        if width == -1:
            if type(bitmap) in _seqTypes:
                width = bitmap[0].GetWidth()
            else:
                if frameWidth:
                    width = frameWidth
        if height == -1:
            if type(bitmap) in _seqTypes:
                height = bitmap[0].GetHeight()
            else:
                height = bitmap.GetHeight()
        self.width, self.height = width, height

        # double check it
        assert width != -1 and height != -1, "Unable to guess size"

        if label:
            extentX, extentY = self.GetTextExtent(label)
            self.labelX = (width - extentX)/2
            self.labelY = (height - extentY)/2
        self.frameDelay = frameDelay
        self.current = 0
        self.direction = 1
        self.autoReverse = reverse
        self.overlay = overlay
        if overlay is not None:
            self.overlay = overlay
            self.overlayX = (width - self.overlay.GetWidth()) / 2
            self.overlayY = (height - self.overlay.GetHeight()) / 2
        self.showOverlay = overlay is not None
        self.showLabel = label is not None

        # do we have a sequence of images?
        if type(bitmap) in _seqTypes:
            self.submaps = bitmap
            self.frames = len(self.submaps)
        # or a composite image that needs to be split?
        else:
            self.frames = frames
            self.submaps = []
            for chunk in range(frames):
                rect = (chunk * frameWidth, 0, width, height)
                self.submaps.append(bitmap.GetSubBitmap(rect))

        # self.sequence can be changed, but it's not recommended doing it
        # while the throbber is running.  self.sequence[0] should always
        # refer to whatever frame is to be shown when 'resting' and be sure
        # that no item in self.sequence >= self.frames or < 0!!!
        self.sequence = range(self.frames)

        self.SetClientSize((width, height))

        timerID  = wx.NewId()
        self.timer = wx.Timer(self, timerID)

        self.Bind(EVT_UPDATE_THROBBER, self.Rotate)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_TIMER, self.OnTimer, self.timer)
        self.Bind(wx.EVT_WINDOW_DESTROY, self.OnDestroyWindow)


    def DoGetBestSize(self):
        return (self.width, self.height)
    

    def OnTimer(self, event):
        wx.PostEvent(self, UpdateThrobberEvent())


    def OnDestroyWindow(self, event):
        self.Stop()
        event.Skip()


    def Draw(self, dc):
        dc.DrawBitmap(self.submaps[self.sequence[self.current]], 0, 0, True)
        if self.overlay and self.showOverlay:
            dc.DrawBitmap(self.overlay, self.overlayX, self.overlayY, True)
        if self.label and self.showLabel:
            dc.DrawText(self.label, self.labelX, self.labelY)
            dc.SetTextForeground(wx.WHITE)
            dc.DrawText(self.label, self.labelX-1, self.labelY-1)


    def OnPaint(self, event):
        self.Draw(wx.PaintDC(self))
        event.Skip()


    def Rotate(self, event):
        self.current += self.direction
        if self.current >= len(self.sequence):
            if self.autoReverse:
                self.Reverse()
                self.current = len(self.sequence) - 1
            else:
                self.current = 1
        if self.current < 1:
            if self.autoReverse:
                self.Reverse()
                self.current = 1
            else:
                self.current = len(self.sequence) - 1
        self.Draw(wx.ClientDC(self))


    # --------- public methods ---------
    def SetFont(self, font):
        """Set the font for the label"""
        wx.Panel.SetFont(self, font)
        self.SetLabel(self.label)
        self.Draw(wx.ClientDC(self))


    def Rest(self):
        """Stop the animation and return to frame 0"""
        self.Stop()
        self.current = 0
        self.Draw(wx.ClientDC(self))


    def Reverse(self):
        """Change the direction of the animation"""
        self.direction = -self.direction


    def Running(self):
        """Returns True if the animation is running"""
        return self.running


    def Start(self):
        """Start the animation"""
        if not self.running:
            self.running = not self.running
            self.timer.Start(int(self.frameDelay * 1000))


    def Stop(self):
        """Stop the animation"""
        if self.running:
            self.timer.Stop()
            self.running = not self.running


    def SetFrameDelay(self, frameDelay = 0.05):
        """Delay between each frame"""
        self.frameDelay = frameDelay
        if self.running:
            self.Stop()
            self.Start()


    def ToggleOverlay(self, state = None):
        """Toggle the overlay image"""
        if state is None:
            self.showOverlay = not self.showOverlay
        else:
            self.showOverlay = state
        self.Draw(wx.ClientDC(self))


    def ToggleLabel(self, state = None):
        """Toggle the label"""
        if state is None:
            self.showLabel = not self.showLabel
        else:
            self.showLabel = state
        self.Draw(wx.ClientDC(self))


    def SetLabel(self, label):
        """Change the text of the label"""
        self.label = label
        if label:
            extentX, extentY = self.GetTextExtent(label)
            self.labelX = (self.width - extentX)/2
            self.labelY = (self.height - extentY)/2
        self.Draw(wx.ClientDC(self))



# ------------------------------------------------------------------------------

