#----------------------------------------------------------------------------
# Name:         MarkerService.py
# Purpose:      Adding and removing line markers in text for easy searching
#
# Author:       Morgan Hua
#
# Created:      10/6/03
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import wx.stc
import wx.lib.docview
import wx.lib.pydocview
import STCTextEditor
_ = wx.GetTranslation


class MarkerService(wx.lib.pydocview.DocService):
    MARKERTOGGLE_ID = wx.NewId()
    MARKERDELALL_ID = wx.NewId()
    MARKERNEXT_ID = wx.NewId()
    MARKERPREV_ID = wx.NewId()


    def __init__(self):
        pass

    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        if document and document.GetDocumentTemplate().GetDocumentType() != STCTextEditor.TextDocument:
            return
        if not document and wx.GetApp().GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
            return

        editMenu = menuBar.GetMenu(menuBar.FindMenu(_("&Edit")))
        editMenu.AppendSeparator()
        editMenu.Append(MarkerService.MARKERTOGGLE_ID, _("Toggle &Marker\tCtrl+M"), _("Toggles a jump marker to text line"))
        wx.EVT_MENU(frame, MarkerService.MARKERTOGGLE_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, MarkerService.MARKERTOGGLE_ID, frame.ProcessUpdateUIEvent)
        editMenu.Append(MarkerService.MARKERDELALL_ID, _("Clear Markers"), _("Removes all jump markers from selected file"))
        wx.EVT_MENU(frame, MarkerService.MARKERDELALL_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, MarkerService.MARKERDELALL_ID, frame.ProcessUpdateUIEvent)
        editMenu.Append(MarkerService.MARKERNEXT_ID, _("Marker Next\tF4"), _("Moves to next marker in selected file"))
        wx.EVT_MENU(frame, MarkerService.MARKERNEXT_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, MarkerService.MARKERNEXT_ID, frame.ProcessUpdateUIEvent)
        editMenu.Append(MarkerService.MARKERPREV_ID, _("Marker Previous\tShift+F4"), _("Moves to previous marker in selected file"))
        wx.EVT_MENU(frame, MarkerService.MARKERPREV_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, MarkerService.MARKERPREV_ID, frame.ProcessUpdateUIEvent)


    def ProcessEvent(self, event):
        id = event.GetId()
        if id == MarkerService.MARKERTOGGLE_ID:
            wx.GetApp().GetDocumentManager().GetCurrentView().MarkerToggle()
            return True
        elif id == MarkerService.MARKERDELALL_ID:
            wx.GetApp().GetDocumentManager().GetCurrentView().MarkerDeleteAll()
            return True
        elif id == MarkerService.MARKERNEXT_ID:
            wx.GetApp().GetDocumentManager().GetCurrentView().MarkerNext()
            return True
        elif id == MarkerService.MARKERPREV_ID:
            wx.GetApp().GetDocumentManager().GetCurrentView().MarkerPrevious()
            return True
        else:
            return False


    def ProcessUpdateUIEvent(self, event):
        id = event.GetId()
        if id == MarkerService.MARKERTOGGLE_ID:
            view = wx.GetApp().GetDocumentManager().GetCurrentView()
            event.Enable(hasattr(view, "MarkerToggle"))
            return True
        elif id == MarkerService.MARKERDELALL_ID:
            view = wx.GetApp().GetDocumentManager().GetCurrentView()
            event.Enable(hasattr(view, "MarkerDeleteAll") and view.GetMarkerCount())
            return True
        elif id == MarkerService.MARKERNEXT_ID:
            view = wx.GetApp().GetDocumentManager().GetCurrentView()
            event.Enable(hasattr(view, "MarkerNext") and view.GetMarkerCount())
            return True
        elif id == MarkerService.MARKERPREV_ID:
            view = wx.GetApp().GetDocumentManager().GetCurrentView()
            event.Enable(hasattr(view, "MarkerPrevious") and view.GetMarkerCount())
            return True
        else:
            return False

