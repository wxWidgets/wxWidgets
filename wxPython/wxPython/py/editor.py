"""PyAlaCarte and PyAlaMode editors."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx

import base
import buffer
import crust
import dispatcher
import frame
import interpreter
import shell
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
        """Create an EditorFrame instance."""
        frame.Frame.__init__(self, parent, id, title, pos, size, style)
        self._buffers = {}
        self._buffer = None  # Current buffer.
        self.editor = None
        self._statusText = title + ' - the tastiest Python editor.'
        self.SetStatusText(self._statusText)
        wx.EVT_IDLE(self, self.OnIdle)
        self._setup()
        if filename:
            self.bufferCreate(filename)

    def _setup(self):
        """Setup prior to first buffer creation.

        Useful for subclasses."""
        pass

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
        for buffer in self._buffers.values():
            self._buffer = buffer
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
        if self._buffer:
            status = self._buffer.getStatus()
            text = 'File: %s  |  Line: %d  |  Column: %d' % status
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
        if self._buffer:
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
        interp = interpreter.Interpreter(locals={})
        self.editor = Editor(interp=interp, parent=self, filename=filename)
        self._buffer = self.editor.buffer
        self._buffers[self._buffer.id] = self._buffer
        self._buffer.editor.SetFocus()

    def bufferDestroy(self):
        """Destroy the current buffer."""
        if self._buffer:
            del self._buffers[self._buffer.id]
            self._buffer = None
        if self.editor:
            self.editor.Destroy()
            self.editor = None

    def bufferHasChanged(self):
        """Return True if buffer has changed since last save."""
        if self._buffer:
            return self._buffer.hasChanged()
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
        if self._buffer and self._buffer.doc.filedir:
            filedir = self._buffer.doc.filedir
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
        if self._buffer.doc.filepath:
            self._buffer.save()
            cancel = False
        else:
            cancel = self.bufferSaveAs()
        return cancel

    def bufferSaveAs(self):
        """Save buffer to a new filename."""
        if self.bufferHasChanged() and self._buffer.doc.filepath:
            cancel = self.bufferSuggestSave()
            if cancel:
                return cancel
        filedir = ''
        if self._buffer and self._buffer.doc.filedir:
            filedir = self._buffer.doc.filedir
        result = saveSingle(directory=filedir)
        if result.path:
            self._buffer.saveAs(result.path)
            cancel = False
        else:
            cancel = True
        return cancel

    def bufferSuggestSave(self):
        """Suggest saving changes.  Return True if user selected Cancel."""
        result = messageDialog(parent=None,
                               message='%s has changed.\n'
                                       'Would you like to save it first'
                                       '?' % self._buffer.name,
                               title='Save current file?')
        if result.positive:
            cancel = self.bufferSave()
        else:
            cancel = result.text == 'Cancel'
        return cancel

    def updateNamespace(self):
        """Update the buffer namespace for autocompletion and calltips."""
        if self._buffer.updateNamespace():
            self.SetStatusText('Namespace updated')
        else:
            self.SetStatusText('Error executing, unable to update namespace')


class EditorNotebookFrame(EditorFrame):
    """Frame containing one or more editors in a notebook."""

    def __init__(self, parent=None, id=-1, title='PyAlaMode',
                 pos=wx.wxDefaultPosition, size=(800, 600), 
                 style=wx.wxDEFAULT_FRAME_STYLE, filename=None):
        """Create an EditorNotebookFrame instance."""
        EditorFrame.__init__(self, parent, id, title, pos,
                             size, style, filename)

    def _setup(self):
        """Setup prior to first buffer creation.

        Useful for subclasses."""
        self._notebook = BufferNotebook(parent=self)
        dispatcher.connect(receiver=self._bufferChange,
                           signal='BufferChange', sender=self._notebook)
        intro = 'PyCrust %s' % version.VERSION
        import imp
        module = imp.new_module('__main__')
        import __builtin__
        module.__dict__['__builtins__'] = __builtin__
        namespace = module.__dict__.copy()
        self.crust = crust.Crust(parent=self._notebook, intro=intro, locals=namespace)
        self.shell = self.crust.shell
        # Override the filling so that status messages go to the status bar.
        self.crust.filling.tree.setStatusText = self.SetStatusText
        # Override the shell so that status messages go to the status bar.
        self.shell.setStatusText = self.SetStatusText
        # Fix a problem with the sash shrinking to nothing.
        self.crust.filling.SetSashPosition(200)
        self._notebook.AddPage(page=self.crust, text='PyCrust', select=True)
        self._buffer = self.crust.buffer
        self._buffers[self._buffer.id] = self._buffer
        self._buffer.editor.SetFocus()

    def _bufferChange(self, buffer):
        """Buffer change signal receiver."""
        self._buffer = buffer

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
        interp = interpreter.Interpreter(locals={})
        editor = Editor(interp=interp, parent=self._notebook,
                        filename=filename)
        self._buffer = editor.buffer
        self._buffers[self._buffer.id] = self._buffer
        self._notebook.AddPage(page=editor, text=self._buffer.name,
                               select=True)
        self._buffer.editor.SetFocus()

    def bufferDestroy(self):
        """Destroy the current buffer."""
        selection = self._notebook.GetSelection()
##         print "Destroy Selection:", selection
        if selection > 0:  # Don't destroy the PyCrust tab.
            if self._buffer:
                del self._buffers[self._buffer.id]
                self._buffer = None  # Do this before DeletePage().
            self._notebook.DeletePage(selection)

    def bufferNew(self):
        """Create new buffer."""
        self.bufferCreate()
        cancel = False
        return cancel

    def bufferOpen(self):
        """Open file in buffer."""
        filedir = ''
        if self._buffer and self._buffer.doc.filedir:
            filedir = self._buffer.doc.filedir
        result = openMultiple(directory=filedir)
        for path in result.paths:
            self.bufferCreate(path)
        cancel = False
        return cancel


class BufferNotebook(wx.wxNotebook):
    """A notebook containing a page for each buffer."""

    def __init__(self, parent):
        """Create a BufferNotebook instance."""
        wx.wxNotebook.__init__(self, parent, id=-1)
        wx.EVT_NOTEBOOK_PAGE_CHANGING(self, self.GetId(), self.OnPageChanging)
        wx.EVT_NOTEBOOK_PAGE_CHANGED(self, self.GetId(), self.OnPageChanged)

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
        page = self.GetPage(new)
        buffer = page.buffer
        buffer.editor.SetFocus()
        dispatcher.send(signal='BufferChange', sender=self, buffer=buffer)
        event.Skip()


class BufferEditorShellNotebookFrame(EditorFrame):
    """Frame containing one or more editor notebooks."""

    def __init__(self, parent=None, id=-1, title='PyAlaMode',
                 pos=wx.wxDefaultPosition, size=(600, 400), 
                 style=wx.wxDEFAULT_FRAME_STYLE,
                 filename=None, singlefile=False):
        """Create a BufferEditorShellNotebookFrame instance."""
        self._singlefile = singlefile
        EditorFrame.__init__(self, parent, id, title, pos,
                             size, style, filename)

    def _setup(self):
        """Setup prior to first buffer creation.

        Useful for subclasses."""
        if not self._singlefile:
            self._notebook = BufferNotebook(parent=self)
            dispatcher.connect(receiver=self._bufferChange,
                               signal='BufferChange', sender=self._notebook)

    def _bufferChange(self, buffer):
        """Buffer change signal receiver."""
        self._buffer = buffer

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
        if self._singlefile:
            self.bufferDestroy()
            notebook = self._notebook = EditorShellNotebook(parent=self,
                                                            filename=filename)
        else:
            notebook = EditorShellNotebook(parent=self._notebook,
                                           filename=filename)
        self._buffer = notebook.buffer
        if not self._singlefile:
            self._notebook.AddPage(page=notebook, text=self._buffer.name,
                                   select=True)
        self._buffers[self._buffer.id] = self._buffer
        self._buffer.editor.SetFocus()

    def bufferDestroy(self):
        """Destroy the current buffer."""
        if self._buffer:
            del self._buffers[self._buffer.id]
            self._buffer = None  # Do this before DeletePage().
        if self._singlefile:
            self._notebook.Destroy()
            self._notebook = None
        else:
            selection = self._notebook.GetSelection()
            print "Destroy Selection:", selection
            self._notebook.DeletePage(selection)

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
        if self._buffer and self._buffer.doc.filedir:
            filedir = self._buffer.doc.filedir
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


class BufferEditorShellNotebook(wx.wxNotebook):
    """A notebook containing a page for each buffer."""

    def __init__(self, parent):
        """Create a BufferEditorShellNotebook instance."""
        wx.wxNotebook.__init__(self, parent, id=-1)
        wx.EVT_NOTEBOOK_PAGE_CHANGED(self, self.GetId(), self.OnPageChanged)

    def OnPageChanged(self, event):
        """Page changed event handler."""
##         old = event.GetOldSelection()
##         print "Changed from old:", old
        new = event.GetSelection()
##         print "Changed to new:", new
        page = self.GetPage(new)
        buffer = page.buffer
        subselection = page.GetSelection()
        page.focus(subselection)
        dispatcher.send(signal='BufferChange', sender=self, buffer=buffer)
        event.Skip()


class EditorShellNotebook(wx.wxNotebook):
    """A notebook containing an editor page and a shell page."""

    def __init__(self, parent, filename=None):
        """Create an EditorShellNotebook instance."""
        wx.wxNotebook.__init__(self, parent, id=-1)
        usePanels = True
        if usePanels:
            shellparent = shellpanel = wx.wxPanel(self, -1)
            editorparent = editorpanel = wx.wxPanel(self, -1)
        else:
            shellparent = self
            editorparent = self
        self.shell = shell.Shell(parent=shellparent,
                                 style=wx.wxCLIP_CHILDREN | wx.wxSUNKEN_BORDER)
        self.editor = Editor(interp=self.shell.interp, parent=editorparent,
                             filename=filename)
        if usePanels:
            self.AddPage(page=editorpanel, text='File', select=True)
            self.AddPage(page=shellpanel, text='Shell')
            # Setup sizers
            shellsizer = wx.wxBoxSizer(wx.wxVERTICAL)
            shellsizer.Add(self.shell, 1, wx.wxEXPAND)
            shellpanel.SetSizer(shellsizer)
            shellpanel.SetAutoLayout(True)
            editorsizer = wx.wxBoxSizer(wx.wxVERTICAL)
            editorsizer.Add(self.editor, 1, wx.wxEXPAND)
            editorpanel.SetSizer(editorsizer)
            editorpanel.SetAutoLayout(True)
        else:
            self.AddPage(page=self.editor, text='File', select=True)
            self.AddPage(page=self.shell, text='Shell')
        self.buffer = self.editor.buffer
        self.editor.SetFocus()
        wx.EVT_NOTEBOOK_PAGE_CHANGED(self, self.GetId(), self.OnPageChanged)

    def OnPageChanged(self, event):
        """Page changed event handler."""
        selection = event.GetSelection()
        self.focus(selection)
        event.Skip()

    def focus(self, selection):
        if selection == 0:
            self.editor.SetFocus()
        else:
            self.shell.SetFocus()


class Editor(base.Editor):
    """Editor based on StyledTextCtrl."""

    def __init__(self, interp, parent, id=-1, pos=wx.wxDefaultPosition,
                 size=wx.wxDefaultSize,
                 style=wx.wxCLIP_CHILDREN | wx.wxSUNKEN_BORDER,
                 filename=None):
        """Create a Editor instance."""
        base.Editor.__init__(self, parent, id, pos, size, style)
        self.interp = interp
        # Find out for which keycodes the interpreter will autocomplete.
        self.autoCompleteKeys = self.interp.getAutoCompleteKeys()
        # Assign handlers for keyboard events.
        wx.EVT_CHAR(self, self.OnChar)
        wx.EVT_KEY_DOWN(self, self.OnKeyDown)
        self.buffer = buffer.Buffer(editor=self, interp=self.interp,
                                    filename=filename)

    def OnChar(self, event):
        """Keypress event handler.
        
        Only receives an event if OnKeyDown calls event.Skip() for the
        corresponding event."""

        key = event.KeyCode()
        if key in self.autoCompleteKeys:
            # Usually the dot (period) key activates auto completion.
            if self.AutoCompActive(): 
                self.AutoCompCancel()
            self.ReplaceSelection('')
            self.AddText(chr(key))
            text, pos = self.GetCurLine()
            text = text[:pos]
            if self.autoComplete: 
                self.autoCompleteShow(text)
        elif key == ord('('):
            # The left paren activates a call tip and cancels an
            # active auto completion.
            if self.AutoCompActive(): 
                self.AutoCompCancel()
            self.ReplaceSelection('')
            self.AddText('(')
            text, pos = self.GetCurLine()
            text = text[:pos]
            self.autoCallTipShow(text)
        else:
            # Allow the normal event handling to take place.
            event.Skip()

    def OnKeyDown(self, event):
        """Key down event handler."""

        key = event.KeyCode()
        # If the auto-complete window is up let it do its thing.
        if self.AutoCompActive():
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
        list = self.interp.getAutoCompleteList(command, 
                    includeMagic=self.autoCompleteIncludeMagic, 
                    includeSingle=self.autoCompleteIncludeSingle, 
                    includeDouble=self.autoCompleteIncludeDouble)
        if list and len(list) < 2000:
            options = ' '.join(list)
            offset = 0
            self.AutoCompShow(offset, options)

    def autoCallTipShow(self, command):
        """Display argument spec and docstring in a popup window."""
        if self.CallTipActive():
            self.CallTipCancel()
        (name, argspec, tip) = self.interp.getCallTip(command)
        if tip:
            dispatcher.send(signal='Shell.calltip', sender=self, calltip=tip)
        if not self.autoCallTip:
            return
        if argspec:
            startpos = self.GetCurrentPos()
            self.AddText(argspec + ')')
            endpos = self.GetCurrentPos()
            self.SetSelection(endpos, startpos)
        if tip:
            curpos = self.GetCurrentPos()
            size = len(name)
            tippos = curpos - (size + 1)
            fallback = curpos - self.GetColumn(curpos)
            # In case there isn't enough room, only go back to the
            # fallback.
            tippos = max(tippos, fallback)
            self.CallTipShow(tippos, tip)
            self.CallTipSetHighlight(0, size)


class DialogResults:
    """DialogResults class."""

    def __init__(self, returned):
        """Create a wrapper for the results returned by a dialog."""
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
