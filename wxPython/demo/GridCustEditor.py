#!/usr/bin/env python

from wxPython.wx import *
from wxPython.grid import *

#---------------------------------------------------------------------------
class MyCellEditor(wxPyGridCellEditor):
    """
    This is a sample GridCellEditor that shows you how to make your own custom
    grid editors.  All the methods that can be overridden are show here.  The
    ones that must be overridden are marked with "*Must Override*" in the
    docstring.

    Notice that in order to call the base class version of these special
    methods we use the method name preceded by "base_".  This is because these
    methods are "virtual" in C++ so if we try to call wxGridCellEditor.Create
    for example, then when the wxPython extension module tries to call
    ptr->Create(...) then it actually calls the derived class version which
    looks up the method in this class and calls it, causing a recursion loop.
    If you don't understand any of this, don't worry, just call the "base_"
    version instead.
    """
    def __init__(self, log):
        self.log = log
        self.log.write("MyCellEditor ctor\n")
        wxPyGridCellEditor.__init__(self)


    def Create(self, parent, id, evtHandler):
        """
        Called to create the control, which must derive from wxControl.
        *Must Override*
        """
        self.log.write("MyCellEditor: Create\n")
        self._tc = wxTextCtrl(parent, id, "")
        self._tc.SetInsertionPoint(0)
        self.SetControl(self._tc)
        if evtHandler:
            self._tc.PushEventHandler(evtHandler)


    def SetSize(self, rect):
        """
        Called to position/size the edit control within the cell rectangle.
        If you don't fill the cell (the rect) then be sure to override
        PaintBackground and do something meaningful there.
        """
        self.log.write("MyCellEditor: SetSize %s\n" % rect)
        self._tc.SetDimensions(rect.x, rect.y, rect.width+2, rect.height+2)


    def Show(self, show, attr):
        """
        Show or hide the edit control.  You can use the attr (if not None)
        to set colours or fonts for the control.
        """
        self.log.write("MyCellEditor: Show(self, %s, %s)\n" % (show, attr))
        self.base_Show(show, attr)


    def PaintBackground(self, rect, attr):
        """
        Draws the part of the cell not occupied by the edit control.  The
        base  class version just fills it with background colour from the
        attribute.  In this class the edit control fills the whole cell so
        don't do anything at all in order to reduce flicker.
        """
        self.log.write("MyCellEditor: PaintBackground\n")


    def BeginEdit(self, row, col, grid):
        """
        Fetch the value from the table and prepare the edit control
        to begin editing.  Set the focus to the edit control.
        *Must Override*
        """
        self.log.write("MyCellEditor: BeginEdit (%d,%d)\n" % (row, col))
        self.startValue = grid.GetTable().GetValue(row, col)
        self._tc.SetValue(self.startValue)
        self._tc.SetInsertionPointEnd()
        self._tc.SetFocus()

        # For this example, select the text
        self._tc.SetSelection(0, self._tc.GetLastPosition())


    def EndEdit(self, row, col, grid):
        """
        Complete the editing of the current cell. Returns true if the value
        has changed.  If necessary, the control may be destroyed.
        *Must Override*
        """
        self.log.write("MyCellEditor: EndEdit (%d,%d)\n" % (row, col))
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
        self.log.write("MyCellEditor: Reset\n")
        self._tc.SetValue(self.startValue)
        self._tc.SetInsertionPointEnd()


    def IsAcceptedKey(self, evt):
        """
        Return TRUE to allow the given key to start editing: the base class
        version only checks that the event has no modifiers.  F2 is special
        and will always start the editor.
        """
        self.log.write("MyCellEditor: IsAcceptedKey: %d\n" % (evt.GetKeyCode()))

        ## Oops, there's a bug here, we'll have to do it ourself..
        ##return self.base_IsAcceptedKey(evt)

        return (not (evt.ControlDown() or evt.AltDown()) and
                evt.GetKeyCode() != WXK_SHIFT)


    def StartingKey(self, evt):
        """
        If the editor is enabled by pressing keys on the grid, this will be
        called to let the editor do something about that first key if desired.
        """
        self.log.write("MyCellEditor: StartingKey %d\n" % evt.GetKeyCode())
        key = evt.GetKeyCode()
        ch = None
        if key in [WXK_NUMPAD0, WXK_NUMPAD1, WXK_NUMPAD2, WXK_NUMPAD3, WXK_NUMPAD4,
                   WXK_NUMPAD5, WXK_NUMPAD6, WXK_NUMPAD7, WXK_NUMPAD8, WXK_NUMPAD9]:
            ch = ch = chr(ord('0') + key - WXK_NUMPAD0)

        elif key < 256 and key >= 0 and chr(key) in string.printable:
            ch = chr(key)
            if not evt.ShiftDown():
                ch = string.lower(ch)

        if ch is not None:
            # For this example, replace the text.  Normally we would append it.
            #self._tc.AppendText(ch)
            self._tc.SetValue(ch)
        else:
            evt.Skip()


    def StartingClick(self):
        """
        If the editor is enabled by clicking on the cell, this method will be
        called to allow the editor to simulate the click on the control if
        needed.
        """
        self.log.write("MyCellEditor: StartingClick\n")


    def Destroy(self):
        """final cleanup"""
        self.log.write("MyCellEditor: Destroy\n")
        self.base_Destroy()


    def Clone(self):
        """
        Create a new object which is the copy of this one
        *Must Override*
        """
        self.log.write("MyCellEditor: Clone\n")
        return MyCellEditor(self.log)


#---------------------------------------------------------------------------
class GridEditorTest(wxGrid):
    def __init__(self, parent, log):
        wxGrid.__init__(self, parent, -1)
        self.log = log

        self.CreateGrid(10, 3)

        # Somebody changed the grid so the type registry takes precedence
        # over the default attribute set for editors and renderers, so we
        # have to set null handlers for the type registry before the
        # default editor will get used otherwise...
        #self.RegisterDataType(wxGRID_VALUE_STRING, None, None)
        #self.SetDefaultEditor(MyCellEditor(self.log))

        # Or we could just do it like this:
        #self.RegisterDataType(wxGRID_VALUE_STRING,
        #                      wxGridCellStringRenderer(),
        #                      MyCellEditor(self.log))

        # but for this example, we'll just set the custom editor on one cell
        self.SetCellEditor(1, 0, MyCellEditor(self.log))
        self.SetCellValue(1, 0, "Try to edit this box")

        # and on a column
        attr = wxGridCellAttr()
        attr.SetEditor(MyCellEditor(self.log))
        self.SetColAttr(2, attr)
        self.SetCellValue(1, 2, "or any in this column")

        self.SetColSize(0, 150)
        self.SetColSize(1, 150)
        self.SetColSize(2, 150)


#---------------------------------------------------------------------------

class TestFrame(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, "Custom Grid Cell Editor Test",
                         size=(640,480))
        grid = GridEditorTest(self, log)

#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys
    app = wxPySimpleApp()
    frame = TestFrame(None, sys.stdout)
    frame.Show(true)
    app.MainLoop()


