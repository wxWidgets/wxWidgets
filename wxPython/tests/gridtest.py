# wxGrid workout

import sys

#----------------------------------------------------------------------------
from   wxPython.wx import *
from   wxPython.grid import *

class MyCellEditor(wxPyGridCellEditor):
    """
    A custom GridCellEditor that only accepts numbers.
    Also tries to work around wxGTK anomaly that key cannot start edit.
    """

    def Create(self, parent, id, evtHandler):
        """
        Called to create the control, which must derive from wxControl.
        *Must Override*
        """
        print "Create"
        theStyle = 0
        if wxPlatform == '__WXMSW__':
            theStyle = wxTE_PROCESS_TAB | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxTE_AUTO_SCROLL
            #theStyle = wxTE_PROCESS_TAB | wxTE_PROCESS_ENTER

        self._tc = wxTextCtrl(parent, id, "", style=theStyle)
        self._tc.SetInsertionPoint(0)
        self.SetControl(self._tc)
        if evtHandler:
            print evtHandler
            self._tc.PushEventHandler(evtHandler)


    def SetSize(self, rect):
        """
        Called to position/size the edit control within the cell rectangle.
        If you don't fill the cell (the rect) then be sure to override
        PaintBackground and do something meaningful there.
        """
        self._tc.SetDimensions(rect.x-1, rect.y-1, rect.width+4, rect.height+4)

    def BeginEdit(self, row, col, grid):
        """
        Fetch the value from the table and prepare the edit control
        to begin editing.  Set the focus to the edit control.
        *Must Override*
        """
        print "BeginEdit"
        self.startValue = grid.GetTable().GetValue(row, col)
        self._tc.SetValue(self.startValue)
        self._tc.SetFocus()

    def EndEdit(self, row, col, grid):
        """
        Complete the editing of the current cell. Returns true if the value
        has changed.  If necessary, the control may be destroyed.
        *Must Override*
        """
        changed = false
        val = self._tc.GetValue()
        if val != self.startValue:
            changed = true
            grid.GetTable().SetValue(row, col, val) # update the table

        self.startValue = ''
        self._tc.SetValue('')
        return changed


    def Reset(self):
        """
        Reset the value in the control back to its starting value.
        *Must Override*
        """
        self._tc.SetValue(self.startValue)
        self._tc.SetInsertionPointEnd()


    def IsAcceptedKey(self, evt):
        """
        Return TRUE to allow the given key to start editing: the base class
        version only checks that the event has no modifiers.
        """
        key = evt.GetKeyCode()
        print "KeyCode:", key
        # For linux the first range means number keys in main keyboard, and
        # the second range means numeric keypad keys, with num-lock on.
        if key in range(48,58) or key in range(326,336):
            return true
        else:
            return false


    def StartingKey(self, evt):
        """
        If the editor is enabled by pressing keys on the grid, this will be
        called to let the editor do something about that first key if desired.
        """
        key = evt.GetKeyCode()
        print "StartingKey", key
        ch = None
        if key in [WXK_NUMPAD0, WXK_NUMPAD1, WXK_NUMPAD2, WXK_NUMPAD3, WXK_NUMPAD4,
                   WXK_NUMPAD5, WXK_NUMPAD6, WXK_NUMPAD7, WXK_NUMPAD8, WXK_NUMPAD9]:
            ch = ch = chr(ord('0') + key - WXK_NUMPAD0)

        elif key < 256 and key >= 0 and chr(key) in string.printable:
            ch = chr(key)
            if not evt.ShiftDown():
                ch = string.lower(ch)

        if ch is not None:
            # Replace the text.  Other option would be to append it.
            # self._tc.AppendText(ch)
            self._tc.SetValue(ch)
            self._tc.SetInsertionPointEnd()
        else:
            evt.Skip()


    def Destroy(self):
        """final cleanup"""
        self.base_Destroy()


    def Clone(self):
        """
        Create a new object which is the copy of this one
        *Must Override*
        """
        print "clone"
        return MyCellEditor()


class MyGrid(wxGrid):
    def __init__(self, parent, cust):
        wxGrid.__init__(self, parent, -1)
        self.CreateGrid(10, 10)
        for column in xrange(10):
            self.SetColSize(column, 40)

        self.SetDefaultCellAlignment(wxCENTRE, wxCENTRE)

        import os
        if cust:
            for column in xrange(10):
                # Note, we create attr and cell editor for each column
                # otherwise segfault at exit, probably tries to free those
                # multiple times from each column.
                attr = wxGridCellAttr()
                attr.SetEditor(MyCellEditor())
                self.SetColAttr(column, attr)


#----------------------------------------------------------------------------

class CardNoteBook(wxNotebook):
    def __init__(self, parent, id):
        wxNotebook.__init__(self, parent, id)

        for title, cust in [("Default", 0), ("Custom Cell Editor", 1)]:
            win = MyGrid(self, cust)
            self.AddPage(win, title)

        EVT_NOTEBOOK_PAGE_CHANGED(self, self.GetId(), self.OnPageChanged)
        EVT_KEY_DOWN(self, self.OnKeyDown)
        EVT_NAVIGATION_KEY(self, self.OnNavKey)


    def OnKeyDown(self, evt):
        print 'CardNoteBook.OnKeyDown: ', evt.GetKeyCode()
        evt.Skip()


    def OnNavKey(self, evt):
        print 'CardNoteBook.OnNavKey:', evt
        evt.Skip()

    def OnPageChanged(self, event):
        event.Skip()


#----------------------------------------------------------------------------

class ScoreKeeper(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, -1, title, size = (500, 400),
                         style=wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE)
        EVT_CLOSE(self, self.OnCloseWindow)
        self.Centre(wxBOTH)

        self.nb = CardNoteBook(self, -1)
        #win=MyGrid(self, "GTK")
        self.Show(true)

    def OnCloseWindow(self, event):
        self.dying = true
        self.window = None
        self.mainmenu = None
        self.Destroy()

    def OnFileExit(self, event):
        self.Close()

#----------------------------------------------------------------------------

class MyApp(wxApp):
    def OnInit(self):
        frame = ScoreKeeper(None, -1, "ScoreKeeper: (A Demonstration)")
        frame.Show(true)
        self.SetTopWindow(frame)
        return true



#---------------------------------------------------------------------------

def main():
    app = MyApp(0)
    app.MainLoop()

main()
