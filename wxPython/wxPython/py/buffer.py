"""Buffer class."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx

import imp
import os
import sys

import document

try:
    True
except NameError:
    True = 1==1
    False = 1==0


class Buffer:
    """Buffer class."""

    id = 0

    def __init__(self, editor, interp, filename=None):
        """Create a Buffer instance."""
        Buffer.id += 1
        self.id = Buffer.id
        self.name = ''
        self.editor = editor
        self.interp = interp
        self.modules = sys.modules.keys()
        self.syspath = sys.path[:]
        while True:
            try:
                self.syspath.remove('')
            except ValueError:
                break
        while True:
            try:
                self.syspath.remove('.')
            except ValueError:
                break
        self.open(filename)

    def getStatus(self):
        """Return (filepath, line, column) status tuple."""
        editor = self.editor
        pos = editor.GetCurrentPos()
        line = editor.LineFromPosition(pos) + 1
        col = editor.GetColumn(pos) + 1
        status = (self.doc.filepath or self.name, line, col)
        return status

    def hasChanged(self):
        """Return True if text in editor has changed since last save."""
        return self.editor.GetModify()

    def new(self, filepath):
        """New empty buffer."""
        if not filepath:
            return
        if os.path.exists(filepath):
            self.confirmed = self.overwriteConfirm(filepath)
        else:
            self.confirmed = True

    def open(self, filename):
        """Open file into buffer."""
        self.doc = document.Document(filename)
        self.name = self.doc.filename or ('Untitled:' + str(self.id))
        self.modulename = self.doc.filebase
        if self.doc.filepath and os.path.exists(self.doc.filepath):
            self.editor.ClearAll()
            self.editor.SetText(self.doc.read())
            self.editor.EmptyUndoBuffer()
            self.editor.SetSavePoint()
            self.confirmed = True
        if self.doc.filedir and self.doc.filedir not in self.syspath:
            self.syspath.insert(0, self.doc.filedir)

    def overwriteConfirm(filepath):
        """Confirm overwriting an existing file."""
        return False

    def save(self):
        """Save buffer."""
        filepath = self.doc.filepath
        if not filepath:
            return  # XXX Get filename
        if not os.path.exists(filepath):
            self.confirmed = True
        if not self.confirmed:
            self.confirmed = self.overwriteConfirm(filepath)
        if self.confirmed:
            self.doc.write(self.editor.GetText())
            self.editor.SetSavePoint()

    def saveAs(self, filename):
        """Save buffer."""
        self.doc = document.Document(filename)
        self.name = self.doc.filename
        self.modulename = self.doc.filebase
        filepath = self.doc.filepath
        if not filepath:
            return  # XXX Get filename
##         if not os.path.exists(filepath):
        self.confirmed = True
        if not self.confirmed:
            self.confirmed = self.overwriteConfirm(filepath)
        if self.confirmed:
            self.doc.write(self.editor.GetText())
            self.editor.SetSavePoint()

    def updateNamespace(self):
        """Update the namespace for autocompletion and calltips.

        Return True if updated, False if there was an error."""
        backup = self.interp.locals
        syspath = sys.path
        sys.path = self.syspath
        code = self.editor.GetText()
        module = imp.new_module(str(self.modulename))
        namespace = module.__dict__.copy()
        try:
            try:
                exec code in namespace
            except:
                self.interp.locals = backup
                return False
            else:
                self.interp.locals = namespace
                return True
        finally:
            sys.path = syspath
            for m in sys.modules.keys():
                if m not in self.modules:
                    del sys.modules[m]
