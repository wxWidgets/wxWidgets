#----------------------------------------------------------------------------
# Name:        wx.lib.mixins.inspection
# Purpose:     A mix-in class that can add PyCrust-based inspection of the
#              app's widgets and sizers.
#
# Author:      Robin Dunn
#
# Created:     21-Nov-2006
# RCS-ID:      $Id$
# Copyright:   (c) 2006 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------------

# NOTE: This class was originally based on ideas sent to the
# wxPython-users mail list by Dan Eloff.

import wx
from wx.lib.inspection import InspectionTool


#----------------------------------------------------------------------------

class InspectionMixin(object):
    """
    This class is intended to be used as a mix-in with the wx.App
    class.  When used it will add the ability to popup a
    InspectionFrame window where the widget under the mouse cursor
    will be selected in the tree and loaded into the shell's namespace
    as 'obj'.  The default key sequence to activate the inspector is
    Ctrl-Alt-I (or Cmd-Alt-I on Mac) but this can be changed via
    parameters to the `Init` method, or the application can call
    `ShowInspectionTool` from other event handlers if desired.

    To use this class simply derive a class from wx.App and
    InspectionMixin and then call the `Init` method from the app's
    OnInit.
    """
    def Init(self, pos=wx.DefaultPosition, size=wx.Size(850,700),
             config=None, locals=None,
             alt=True, cmd=True, shift=False, keyCode=ord('I')):
        """
        Make the event binding that will activate the InspectionFrame window.
        """
        self.Bind(wx.EVT_KEY_DOWN, self._OnKeyPress)
        self._alt = alt
        self._cmd = cmd
        self._shift = shift
        self._keyCode = keyCode
        InspectionTool().Init(pos, size, config, locals, self)

    def _OnKeyPress(self, evt):
        """
        Event handler, check for our hot-key.  Normally it is
        Ctrl-Alt-I but that can be changed by what is passed to the
        Init method.
        """
        if evt.AltDown() == self._alt  and \
               evt.CmdDown() == self._cmd and \
               evt.ShiftDown() == self._shift and \
               evt.GetKeyCode() == self._keyCode:
            self.ShowInspectionTool()
        else:
            evt.Skip()


    def ShowInspectionTool(self):
        """
        Show the Inspection tool, creating it if neccesary, setting it
        to display the widget under the cursor.
        """
        # get the current widget under the mouse
        wnd = wx.FindWindowAtPointer()
        InspectionTool().Show(wnd)


#---------------------------------------------------------------------------

class InspectableApp(wx.App, InspectionMixin):
    """
    A simple mix of wx.App and InspectionMixin that can be used stand-alone.
    """

    def OnInit(self):
        self.Init()
        return True

#---------------------------------------------------------------------------

