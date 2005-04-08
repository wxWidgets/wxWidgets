#----------------------------------------------------------------------------
# Name:         TabbedView.py
# Purpose:
#
# Author:       Peter Yared
#
# Created:      8/17/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import wx.lib.docview

class TabbedView(dict, wx.lib.docview.View):

    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self):
        wx.lib.docview.View.__init__(self)
        self._views = {}
        self._currentView = None


    def OnCreate(self, doc, flags):
        frame = wx.GetApp().CreateDocumentFrame(self, doc, flags)
        sizer = wx.BoxSizer()
        self._notebook = wx.Notebook(frame, -1, style = wx.NB_BOTTOM)
        self.Activate()
        return True


    def AddView(self, viewName, view):
        self._notebook.AddPage(wx.Panel(self._notebook, -1), viewName)
        self._currentView = view
        self._views[viewName] = view


    def __getattr__(self, attrname):
        return getattr(self._currentView, attrname)


    def SetView(self, viewName):
        self._currentview = self._views[viewName]

