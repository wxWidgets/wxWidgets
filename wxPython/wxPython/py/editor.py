"""PyAlaCarte and PyAlaMode editors."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx

from buffer import Buffer
import crust
import dispatcher
import editwindow
import frame
from shell import Shell
import version

try:
    True
except NameError:
    True = 1==1
    False = 1==0


class EditorFrame(frame.Frame):
    """Frame containing one editor."""

    def __init__(self, parent=None, id=-1, title='PyAlaCarte',
                 pos=wx.wxDefaultPosition, size=(800, 600), 
                 style=wx.wxDEFAULT_FRAME_STYLE, filename=None):
        """Create EditorFrame instance."""
        frame.Frame.__init__(self, parent, id, title, pos, size, style)
        self.buffers = {}
        self.buffer = None  # Current buffer.
        self.editor = None
        self._defaultText = title + ' - the tastiest Python editor.'
        self._statusText = self._defaultText
        self.SetStatusText(self._statusText)
        wx.EVT_IDLE(self, self.OnIdle)
        self._setup()
        if filename:
            self.bufferCreate(filename)

    def _setup(self):
        """Setup prior to first buffer creation.

        Useful for subclasses."""
        pass

    def setEditor(self, editor):
        self.editor = editor
        self.buffer = self.editor.buffer
        self.buffers[self.buffer.id] = self.buffer

    def OnAbout(self, event):
        """Display an About window."""
        title = 'About PyAlaCarte'
        text = 'Another fine, flaky program.'
        dialog = wx.wxMessageDialog(self, text, title,
                                    wx.wxOK | wx.wxICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()

    def OnClose(self, event):
        """Event handler for closing."""
        for buffer in self.buffers.values():
            self.buffer = buffer
            if buffer.hasChanged():
                cancel = self.bufferSuggestSave()
                if cancel and event.CanVeto():
                    event.Veto()
                    return
        self.Destroy()

    def OnIdle(self, event):
        """Event handler for idle time."""
        self._updateStatus()
        self._updateTitle()
        event.Skip()

    def _updateStatus(self):
        """Show current status information."""
        if self.editor and hasattr(self.editor, 'getStatus'):
            status = self.editor.getStatus()
            text = 'File: %s  |  Line: %d  |  Column: %d' % status
        else:
            text = self._defaultText
        if text != self._statusText:
            self.SetStatusText(text)
            self._statusText = text

    def _updateTitle(self):
        """Show current title information."""
        title = self.GetTitle()
        if self.bufferHasChanged():
            if title.startswith('* '):
                pass
            else:
                self.SetTitle('* ' + title)
        else:
            if title.startswith('* '):
                self.SetTitle(title[2:])
        
    def hasBuffer(self):
        """Return True if there is a current buffer."""
        if self.buffer:
            return True
        else:
            return False

    def bufferClose(self):
        """Close buffer."""
        if self.bufferHasChanged():
            cancel = self.bufferSuggestSave()
            if cancel:
                return cancel
        self.bufferDestroy()
        cancel = False
        return cancel

    def bufferCreate(self, filename=None):
        """Create new buffer."""
        self.bufferDestroy()
        buffer = Buffer()
        editor = Editor(parent=self)
        buffer.addEditor(editor)
        buffer.open(filename)
        self.setEditor(editor)
        self.editor.setFocus()

    def bufferDestroy(self):
        """Destroy the current buffer."""
        if self.buffer:
            for editor in self.buffer.editors.values():
                editor.destroy()
            self.editor = None
            del self.buffers[self.buffer.id]
            self.buffer = None

    def bufferHasChanged(self):
        """Return True if buffer has changed since last save."""
        if self.buffer:
            return self.buffer.hasChanged()
        else:
            return False

    def bufferNew(self):
        """Create new buffer."""
        if self.bufferHasChanged():
            cancel = self.bufferSuggestSave()
            if cancel:
                return cancel
        self.bufferCreate()
        cancel = False
        return cancel

    def bufferOpen(self):
        """Open file in buffer."""
        if self.bufferHasChanged():
            cancel = self.bufferSuggestSave()
            if cancel:
                return cancel
        filedir = ''
        if self.buffer and self.buffer.doc.filedir:
            filedir = self.buffer.doc.filedir
        result = openSingle(directory=filedir)
        if result.path:
            self.bufferCreate(result.path)
        cancel = False
        return cancel

##     def bufferPrint(self):
##         """Print buffer."""
##         pass

##     def bufferRevert(self):
##         """Revert buffer to version of file on disk."""
##         pass

    def bufferSave(self):
        """Save buffer to its file."""
        if self.buffer.doc.filepath:
            self.buffer.save()
            cancel = False
        else:
            cancel = self.bufferSaveAs()
        return cancel

    def bufferSaveAs(self):
        """Save buffer to a new filename."""
        if self.bufferHasChanged() and self.buffer.doc.filepath:
            cancel = self.bufferSuggestSave()
            if cancel:
                return cancel
        filedir = ''
        if self.buffer and self.buffer.doc.filedir:
            filedir = self.buffer.doc.filedir
        result = saveSingle(directory=filedir)
        if result.path:
            self.buffer.saveAs(result.path)
            cancel = False
        else:
            cancel = True
        return cancel

    def bufferSuggestSave(self):
        """Suggest saving changes.  Return True if user selected Cancel."""
        result = messageDialog(parent=None,
                               message='%s has changed.\n'
                                       'Would you like to save it first'
                                       '?' % self.buffer.name,
                               title='Save current file?')
        if result.positive:
            cancel = self.bufferSave()
        else:
            cancel = result.text == 'Cancel'
        return cancel

    def updateNamespace(self):
        """Update the buffer namespace for autocompletion and calltips."""
        if self.buffer.updateNamespace():
            self.SetStatusText('Namespace updated')
        else:
            self.SetStatusText('Error executing, unable to update namespace')


class EditorNotebookFrame(EditorFrame):
    """Frame containing one or more editors in a notebook."""

    def __init__(self, parent=None, id=-1, title='PyAlaMode',
                 pos=wx.wxDefaultPosition, size=(800, 600), 
                 style=wx.wxDEFAULT_FRAME_STYLE, filename=None):
        """Create EditorNotebookFrame instance."""
        self.notebook = None
        EditorFrame.__init__(self, parent, id, title, pos,
                             size, style, filename)
        if self.notebook:
            dispatcher.connect(receiver=self._editorChange,
                               signal='EditorChange', sender=self.notebook)

    def _setup(self):
        """Setup prior to first buffer creation.

        Called automatically by base class during init."""
        self.notebook = EditorNotebook(parent=self)
        intro = 'PyCrust %s' % version.VERSION
        import imp
        module = imp.new_module('__main__')
        import __builtin__
        module.__dict__['__builtins__'] = __builtin__
        namespace = module.__dict__.copy()
        self.crust = crust.Crust(parent=self.notebook, intro=intro, locals=namespace)
        self.shell = self.crust.shell
        # Override the filling so that status messages go to the status bar.
        self.crust.filling.tree.setStatusText = self.SetStatusText
        # Override the shell so that status messages go to the status bar.
        self.shell.setStatusText = self.SetStatusText
        # Fix a problem with the sash shrinking to nothing.
        self.crust.filling.SetSashPosition(200)
        self.notebook.AddPage(page=self.crust, text='PyCrust', select=True)
        self.setEditor(self.crust.editor)
        self.crust.editor.SetFocus()

    def _editorChange(self, editor):
        """Editor change signal receiver."""
        self.setEditor(editor)

    def OnAbout(self, event):
        """Display an About window."""
        title = 'About PyAlaMode'
        text = 'Another fine, flaky program.'
        dialog = wx.wxMessageDialog(self, text, title,
                                    wx.wxOK | wx.wxICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()

    def _updateTitle(self):
        """Show current title information."""
        title = self.GetTitle()
        if self.bufferHasChanged():
            if title.startswith('* '):
                pass
            else:
                self.SetTitle('* ' + title)
        else:
            if title.startswith('* '):
                self.SetTitle(title[2:])
        
    def bufferCreate(self, filename=None):
        """Create new buffer."""
        buffer = Buffer()
        editor = Editor(parent=self.notebook)
        buffer.addEditor(editor)
        buffer.open(filename)
        self.setEditor(editor)
        self.notebook.AddPage(page=editor.window, text=self.buffer.name,
                              select=True)
        self.editor.setFocus()

    def bufferDestroy(self):
        """Destroy the current buffer."""
        selection = self.notebook.GetSelection()
##         print "Destroy Selection:", selection
        if selection > 0:  # Don't destroy the PyCrust tab.
            if self.buffer:
                del self.buffers[self.buffer.id]
                self.buffer = None  # Do this before DeletePage().
            self.notebook.DeletePage(selection)

    def bufferNew(self):
        """Create new buffer."""
        self.bufferCreate()
        cancel = False
        return cancel

    def bufferOpen(self):
        """Open file in buffer."""
        filedir = ''
        if self.buffer and self.buffer.doc.filedir:
            filedir = self.buffer.doc.filedir
        result = openMultiple(directory=filedir)
        for path in result.paths:
            self.bufferCreate(path)
        cancel = False
        return cancel


class EditorNotebook(wx.wxNotebook):
    """A notebook containing a page for each editor."""

    def __init__(self, parent):
        """Create EditorNotebook instance."""
        wx.wxNotebook.__init__(self, parent, id=-1)
        wx.EVT_NOTEBOOK_PAGE_CHANGING(self, self.GetId(),
                                      self.OnPageChanging)
        wx.EVT_NOTEBOOK_PAGE_CHANGED(self, self.GetId(),
                                     self.OnPageChanged)

    def OnPageChanging(self, event):
        """Page changing event handler."""
##         old = event.GetOldSelection()
##         print "Changing from old:", old
##         new = event.GetOldSelection()
##         print "Changing to new:", new
        event.Skip()

    def OnPageChanged(self, event):
        """Page changed event handler."""
##         old = event.GetOldSelection()
##         print "Changed from:", old
        new = event.GetSelection()
##         print "Changed to new:", new
        window = self.GetPage(new)
        dispatcher.send(signal='EditorChange', sender=self,
                        editor=window.editor)
        window.SetFocus()
        event.Skip()


class EditorShellNotebookFrame(EditorNotebookFrame):
    """Frame containing a notebook containing EditorShellNotebooks."""

    def __init__(self, parent=None, id=-1, title='PyAlaMode',
                 pos=wx.wxDefaultPosition, size=(600, 400), 
                 style=wx.wxDEFAULT_FRAME_STYLE,
                 filename=None, singlefile=False):
        """Create EditorShellNotebookFrame instance."""
        self._singlefile = singlefile
        EditorNotebookFrame.__init__(self, parent, id, title, pos,
                                     size, style, filename)

    def _setup(self):
        """Setup prior to first buffer creation.

        Called automatically by base class during init."""
        if not self._singlefile:
            self.notebook = EditorNotebook(parent=self)

    def OnAbout(self, event):
        """Display an About window."""
        title = 'About PyAlaModePlus'
        text = 'Another fine, flaky program.'
        dialog = wx.wxMessageDialog(self, text, title,
                                    wx.wxOK | wx.wxICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()

    def bufferCreate(self, filename=None):
        """Create new buffer."""
        if self._singlefile:
            self.bufferDestroy()
            notebook = EditorShellNotebook(parent=self,
                                           filename=filename)
            self.notebook = notebook
        else:
            notebook = EditorShellNotebook(parent=self.notebook,
                                           filename=filename)
        self.setEditor(notebook.editor)
        if not self._singlefile:
            self.notebook.AddPage(page=notebook, text=self.buffer.name,
                                  select=True)
        self.editor.setFocus()

    def bufferDestroy(self):
        """Destroy the current buffer."""
        if self.buffer:
            self.editor = None
            del self.buffers[self.buffer.id]
            self.buffer = None  # Do this before DeletePage().
        if self._singlefile:
            self.notebook.Destroy()
            self.notebook = None
        else:
            selection = self.notebook.GetSelection()
##             print "Destroy Selection:", selection
            self.notebook.DeletePage(selection)

    def bufferNew(self):
        """Create new buffer."""
        if self._singlefile and self.bufferHasChanged():
            cancel = self.bufferSuggestSave()
            if cancel:
                return cancel
        self.bufferCreate()
        cancel = False
        return cancel

    def bufferOpen(self):
        """Open file in buffer."""
        if self._singlefile and self.bufferHasChanged():
            cancel = self.bufferSuggestSave()
            if cancel:
                return cancel
        filedir = ''
        if self.buffer and self.buffer.doc.filedir:
            filedir = self.buffer.doc.filedir
        if self._singlefile:
            result = openSingle(directory=filedir)
            if result.path:
                self.bufferCreate(result.path)
        else:
            result = openMultiple(directory=filedir)
            for path in result.paths:
                self.bufferCreate(path)
        cancel = False
        return cancel


class EditorShellNotebook(wx.wxNotebook):
    """A notebook containing an editor page and a shell page."""

    def __init__(self, parent, filename=None):
        """Create EditorShellNotebook instance."""
        wx.wxNotebook.__init__(self, parent, id=-1)
        usePanels = True
        if usePanels:
            editorparent = editorpanel = wx.wxPanel(self, -1)
            shellparent = shellpanel = wx.wxPanel(self, -1)
        else:
            editorparent = self
            shellparent = self
        self.buffer = Buffer()
        self.editor = Editor(parent=editorparent)
        self.buffer.addEditor(self.editor)
        self.buffer.open(filename)
        self.shell = Shell(parent=shellparent, locals=self.buffer.interp.locals,
                           style=wx.wxCLIP_CHILDREN | wx.wxSUNKEN_BORDER)
        self.buffer.interp.locals.clear()
        if usePanels:
            self.AddPage(page=editorpanel, text='Editor', select=True)
            self.AddPage(page=shellpanel, text='Shell')
            # Setup sizers
            editorsizer = wx.wxBoxSizer(wx.wxVERTICAL)
            editorsizer.Add(self.editor.window, 1, wx.wxEXPAND)
            editorpanel.SetSizer(editorsizer)
            editorpanel.SetAutoLayout(True)
            shellsizer = wx.wxBoxSizer(wx.wxVERTICAL)
            shellsizer.Add(self.shell, 1, wx.wxEXPAND)
            shellpanel.SetSizer(shellsizer)
            shellpanel.SetAutoLayout(True)
        else:
            self.AddPage(page=self.editor.window, text='Editor', select=True)
            self.AddPage(page=self.shell, text='Shell')
        self.editor.setFocus()
        wx.EVT_NOTEBOOK_PAGE_CHANGED(self, self.GetId(), self.OnPageChanged)

    def OnPageChanged(self, event):
        """Page changed event handler."""
        selection = event.GetSelection()
        if selection == 0:
            self.editor.setFocus()
        else:
            self.shell.SetFocus()
        event.Skip()

    def SetFocus(self):
        wx.wxNotebook.SetFocus(self)
        selection = self.GetSelection()
        if selection == 0:
            self.editor.setFocus()
        else:
            self.shell.SetFocus()


class Editor:
    """Editor having an EditWindow."""

    def __init__(self, parent, id=-1, pos=wx.wxDefaultPosition,
                 size=wx.wxDefaultSize,
                 style=wx.wxCLIP_CHILDREN | wx.wxSUNKEN_BORDER):
        """Create Editor instance."""
        self.window = EditWindow(self, parent, id, pos, size, style)
        self.id = self.window.GetId()
        self.buffer = None
        # Assign handlers for keyboard events.
        wx.EVT_CHAR(self.window, self.OnChar)
        wx.EVT_KEY_DOWN(self.window, self.OnKeyDown)

    def _setBuffer(self, buffer, text):
        """Set the editor to a buffer.  Private callback called by buffer."""
        self.buffer = buffer
        self.autoCompleteKeys = buffer.interp.getAutoCompleteKeys()
        self.clearAll()
        self.setText(text)
        self.emptyUndoBuffer()
        self.setSavePoint()

    def destroy(self):
        """Destroy all editor objects."""
        self.window.Destroy()

    def clearAll(self):
        self.window.ClearAll()

    def emptyUndoBuffer(self):
        self.window.EmptyUndoBuffer()

    def getStatus(self):
        """Return (filepath, line, column) status tuple."""
        pos = self.window.GetCurrentPos()
        line = self.window.LineFromPosition(pos) + 1
        col = self.window.GetColumn(pos)
        if self.buffer:
            name = self.buffer.doc.filepath or self.buffer.name
        else:
            name = ''
        status = (name, line, col)
        return status

    def getText(self):
        """Return contents of editor."""
        return self.window.GetText()

    def hasChanged(self):
        """Return True if contents have changed."""
        return self.window.GetModify()

    def setFocus(self):
        """Set the input focus to the editor window."""
        self.window.SetFocus()

    def setSavePoint(self):
        self.window.SetSavePoint()

    def setText(self, text):
        """Set contents of editor."""
        self.window.SetText(text)

    def OnChar(self, event):
        """Keypress event handler.
        
        Only receives an event if OnKeyDown calls event.Skip() for the
        corresponding event."""

        key = event.KeyCode()
        if key in self.autoCompleteKeys:
            # Usually the dot (period) key activates auto completion.
            if self.window.AutoCompActive(): 
                self.window.AutoCompCancel()
            self.window.ReplaceSelection('')
            self.window.AddText(chr(key))
            text, pos = self.window.GetCurLine()
            text = text[:pos]
            if self.window.autoComplete: 
                self.autoCompleteShow(text)
        elif key == ord('('):
            # The left paren activates a call tip and cancels an
            # active auto completion.
            if self.window.AutoCompActive(): 
                self.window.AutoCompCancel()
            self.window.ReplaceSelection('')
            self.window.AddText('(')
            text, pos = self.window.GetCurLine()
            text = text[:pos]
            self.autoCallTipShow(text)
        else:
            # Allow the normal event handling to take place.
            event.Skip()

    def OnKeyDown(self, event):
        """Key down event handler."""

        key = event.KeyCode()
        # If the auto-complete window is up let it do its thing.
        if self.window.AutoCompActive():
            event.Skip()
            return
        controlDown = event.ControlDown()
        altDown = event.AltDown()
        shiftDown = event.ShiftDown()
        # Let Ctrl-Alt-* get handled normally.
        if controlDown and altDown:
            event.Skip()
        # Increase font size.
        elif controlDown and key in (ord(']'),):
            dispatcher.send(signal='FontIncrease')
        # Decrease font size.
        elif controlDown and key in (ord('['),):
            dispatcher.send(signal='FontDecrease')
        # Default font size.
        elif controlDown and key in (ord('='),):
            dispatcher.send(signal='FontDefault')
        else:
            event.Skip()

    def autoCompleteShow(self, command):
        """Display auto-completion popup list."""
        list = self.buffer.interp.getAutoCompleteList(command, 
                    includeMagic=self.window.autoCompleteIncludeMagic, 
                    includeSingle=self.window.autoCompleteIncludeSingle, 
                    includeDouble=self.window.autoCompleteIncludeDouble)
        if list and len(list) < 2000:
            options = ' '.join(list)
            offset = 0
            self.window.AutoCompShow(offset, options)

    def autoCallTipShow(self, command):
        """Display argument spec and docstring in a popup window."""
        if self.window.CallTipActive():
            self.window.CallTipCancel()
        (name, argspec, tip) = self.buffer.interp.getCallTip(command)
        if tip:
            dispatcher.send(signal='Shell.calltip', sender=self, calltip=tip)
        if not self.window.autoCallTip:
            return
        if argspec:
            startpos = self.window.GetCurrentPos()
            self.window.AddText(argspec + ')')
            endpos = self.window.GetCurrentPos()
            self.window.SetSelection(endpos, startpos)
        if tip:
            curpos = self.window.GetCurrentPos()
            size = len(name)
            tippos = curpos - (size + 1)
            fallback = curpos - self.window.GetColumn(curpos)
            # In case there isn't enough room, only go back to the
            # fallback.
            tippos = max(tippos, fallback)
            self.window.CallTipShow(tippos, tip)
            self.window.CallTipSetHighlight(0, size)


class EditWindow(editwindow.EditWindow):
    """EditWindow based on StyledTextCtrl."""

    def __init__(self, editor, parent, id=-1, pos=wx.wxDefaultPosition,
                 size=wx.wxDefaultSize,
                 style=wx.wxCLIP_CHILDREN | wx.wxSUNKEN_BORDER):
        """Create EditWindow instance."""
        editwindow.EditWindow.__init__(self, parent, id, pos, size, style)
        self.editor = editor


class DialogResults:
    """DialogResults class."""

    def __init__(self, returned):
        """Create wrapper for results returned by dialog."""
        self.returned = returned
        self.positive = returned in (wx.wxID_OK, wx.wxID_YES)
        self.text = self._asString()
        

    def __repr__(self):
        return str(self.__dict__)

    def _asString(self):
        returned = self.returned
        if returned == wx.wxID_OK:
            return "Ok"
        elif returned == wx.wxID_CANCEL:
            return "Cancel"
        elif returned == wx.wxID_YES:
            return "Yes"
        elif returned == wx.wxID_NO:
            return "No"


def fileDialog(parent=None, title='Open', directory='', filename='',
               wildcard='All Files (*.*)|*.*',
               style=wx.wxOPEN | wx.wxMULTIPLE):
    """File dialog wrapper function."""
    dialog = wx.wxFileDialog(parent, title, directory, filename,
                             wildcard, style)
    result = DialogResults(dialog.ShowModal())
    if result.positive:
        result.paths = dialog.GetPaths()
    else:
        result.paths = []
    dialog.Destroy()
    return result


def openSingle(parent=None, title='Open', directory='', filename='',
               wildcard='All Files (*.*)|*.*', style=wx.wxOPEN):
    """File dialog wrapper function."""
    dialog = wx.wxFileDialog(parent, title, directory, filename,
                             wildcard, style)
    result = DialogResults(dialog.ShowModal())
    if result.positive:
        result.path = dialog.GetPath()
    else:
        result.path = None
    dialog.Destroy()
    return result


def openMultiple(parent=None, title='Open', directory='', filename='',
                 wildcard='All Files (*.*)|*.*',
                 style=wx.wxOPEN | wx.wxMULTIPLE):
    """File dialog wrapper function."""
    return fileDialog(parent, title, directory, filename, wildcard, style)


def saveSingle(parent=None, title='Save', directory='', filename='',
               wildcard='All Files (*.*)|*.*',
               style=wx.wxSAVE | wx.wxHIDE_READONLY | wx.wxOVERWRITE_PROMPT):
    """File dialog wrapper function."""
    dialog = wx.wxFileDialog(parent, title, directory, filename,
                             wildcard, style)
    result = DialogResults(dialog.ShowModal())
    if result.positive:
        result.path = dialog.GetPath()
    else:
        result.path = None
    dialog.Destroy()
    return result


def directory(parent=None, message='Choose a directory', path='', style=0,
              pos=wx.wxDefaultPosition, size=wx.wxDefaultSize):
    """Dir dialog wrapper function."""
    dialog = wx.wxDirDialog(parent, message, path, style, pos, size)
    result = DialogResults(dialog.ShowModal())
    if result.positive:
        result.path = dialog.GetPath()
    else:
        result.path = None
    dialog.Destroy()
    return result


def messageDialog(parent=None, message='', title='Message box',
                  style=wx.wxYES_NO | wx.wxCANCEL | wx.wxCENTRE | wx.wxICON_QUESTION,
                  pos=wx.wxDefaultPosition):
    """Message dialog wrapper function."""
    dialog = wx.wxMessageDialog(parent, message, title, style, pos)
    result = DialogResults(dialog.ShowModal())
    dialog.Destroy()
    return result
