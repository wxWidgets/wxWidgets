#----------------------------------------------------------------------------
# Name:        wx.lib.mixins.inspect
# Purpose:     A mix-in class that can add PyCrust-based inspection of the app
#
# Author:      Robin Dunn
#
# Created:     21-Nov-2006
# RCS-ID:      $Id$
# Copyright:   (c) 2006 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------------

# NOTE: This class is based on ideas sent to the wxPython-users
# mail-list by Dan Elof.

import wx.py

class InspectionMixin(object):
    """
    This class is intended to be used as a mix-in with the wx.App
    object.  When used it will add the ability to popup a PyCrust
    window where the widget under the mouse cursor will be loaded into
    the shell's namespace as 'win'.

    To use this class simply derive a class from wx.App and
    InspectionMixin and then call the Init() method from the app's
    OnInit.
    """    
    def Init(self):
        """
        Make the event binding that will activate the PyCrust window.
        """
        self.Bind(wx.EVT_KEY_DOWN, self.OnKeyPress)
        self._crust = None


    def OnKeyPress(self, evt):
        """
        Event handler
        """
        if evt.AltDown() and evt.CmdDown() and evt.KeyCode == ord('I'):
            self.ShowShell()
        else:
            evt.Skip()


    def ShowShell(self):
        """
        Show the PyCrust window.
        """
        if not self._crust:
            self._crust = wx.py.crust.CrustFrame(self.GetTopWindow())
            self._crust.shell.interp.locals['app'] = self
        win = wx.FindWindowAtPointer()
        self._crust.shell.interp.locals['win'] = win
        self._crust.Show()


