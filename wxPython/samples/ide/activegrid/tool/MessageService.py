#----------------------------------------------------------------------------
# Name:         MessageService.py
# Purpose:      Message View Service for pydocview
#
# Author:       Morgan Hua
#
# Created:      9/2/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import Service
import STCTextEditor

class MessageView(Service.ServiceView):
    """ Reusable Message View for any document.
        When an item is selected, the document view is called back (with DoSelectCallback) to highlight and display the corresponding item in the document view.
    """

    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def _CreateControl(self, parent, id):
        txtCtrl = STCTextEditor.TextCtrl(parent, id)
        txtCtrl.SetMarginWidth(1, 0)  # hide line numbers
        txtCtrl.SetReadOnly(True)

        if wx.Platform == '__WXMSW__':
            font = "Courier New"
        else:
            font = "Courier"
        txtCtrl.SetFont(wx.Font(10, wx.DEFAULT, wx.NORMAL, wx.NORMAL, faceName = font))
        txtCtrl.SetFontColor(wx.BLACK)
        txtCtrl.StyleClearAll()
        txtCtrl.UpdateStyles()

        return txtCtrl


##    def ProcessEvent(self, event):
##        stcControl = self.GetControl()
##        if not isinstance(stcControl, wx.stc.StyledTextCtrl):
##            return wx.lib.docview.View.ProcessUpdateUIEvent(self, event)
##        id = event.GetId()
##        if id == wx.ID_CUT:
##            stcControl.Cut()
##            return True
##        elif id == wx.ID_COPY:
##            stcControl.Copy()
##            return True
##        elif id == wx.ID_PASTE:
##            stcControl.Paste()
##            return True
##        elif id == wx.ID_CLEAR:
##            stcControl.Clear()
##            return True
##        elif id == wx.ID_SELECTALL:
##            stcControl.SetSelection(0, -1)
##            return True
##
##
##    def ProcessUpdateUIEvent(self, event):
##        stcControl = self.GetControl()
##        if not isinstance(stcControl, wx.stc.StyledTextCtrl):
##            return wx.lib.docview.View.ProcessUpdateUIEvent(self, event)
##        id = event.GetId()
##        if id == wx.ID_CUT:
##            event.Enable(stcControl.CanCut())
##            return True
##        elif id == wx.ID_COPY:
##            event.Enable(stcControl.CanCopy())
##            return True
##        elif id == wx.ID_PASTE:
##            event.Enable(stcControl.CanPaste())
##            return True
##        elif id == wx.ID_CLEAR:
##            event.Enable(True)  # wxBug: should be stcControl.CanCut()) but disabling clear item means del key doesn't work in control as expected
##            return True
##        elif id == wx.ID_SELECTALL:
##            event.Enable(stcControl.GetTextLength() > 0)
##            return True

        
    #----------------------------------------------------------------------------
    # Service specific methods
    #----------------------------------------------------------------------------

    def ClearLines(self):
        self.GetControl().SetReadOnly(False)
        self.GetControl().ClearAll()
        self.GetControl().SetReadOnly(True)


    def AddLines(self, text):
        self.GetControl().SetReadOnly(False)
        self.GetControl().AddText(text)
        self.GetControl().SetReadOnly(True)


    def GetText(self):
        return self.GetControl().GetText()


    def GetCurrentPos(self):
        return self.GetControl().GetCurrentPos()


    def GetCurrLine(self):
        return self.GetControl().GetCurLine()


    #----------------------------------------------------------------------------
    # Callback Methods
    #----------------------------------------------------------------------------

    def SetCallback(self, callback):
        """ Sets in the event table for a doubleclick to invoke the given callback.
            Additional calls to this method overwrites the previous entry and only the last set callback will be invoked.
        """
        wx.stc.EVT_STC_DOUBLECLICK(self.GetControl(), self.GetControl().GetId(), callback)



class MessageService(Service.Service):


    #----------------------------------------------------------------------------
    # Constants
    #----------------------------------------------------------------------------
    SHOW_WINDOW = wx.NewId()  # keep this line for each subclass, need unique ID for each Service


    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def _CreateView(self):
        return MessageView(self)
        
        
