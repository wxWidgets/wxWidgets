"""Buffer class."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from interpreter import Interpreter
import imp
import os
import sys

import document


class Buffer:
    """Buffer class."""

    id = 0

    def __init__(self, filename=None):
        """Create a Buffer instance."""
        Buffer.id += 1
        self.id = Buffer.id
        self.interp = Interpreter(locals={})
        self.name = ''
        self.editors = {}
        self.editor = None
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

    def addEditor(self, editor):
        """Add an editor."""
        self.editor = editor
        self.editors[editor.id] = editor

    def hasChanged(self):
        """Return True if text in editor has changed since last save."""
        if self.editor:
            return self.editor.hasChanged()
        else:
            return False

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
        # XXX This should really make sure filedir is first item in syspath.
        # XXX Or maybe this should be moved to the update namespace method.
        if self.doc.filedir and self.doc.filedir not in self.syspath:
            # To create the proper context for updateNamespace.
            self.syspath.insert(0, self.doc.filedir)
        if self.doc.filepath and os.path.exists(self.doc.filepath):
            self.confirmed = True
        if self.editor:
            text = self.doc.read()
            self.editor._setBuffer(buffer=self, text=text)

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
            self.doc.write(self.editor.getText())
            if self.editor:
                self.editor.setSavePoint()

    def saveAs(self, filename):
        """Save buffer."""
        self.doc = document.Document(filename)
        self.name = self.doc.filename
        self.modulename = self.doc.filebase
        self.save()

    def updateNamespace(self):
        """Update the namespace for autocompletion and calltips.

        Return True if updated, False if there was an error."""
        if not self.interp or not hasattr(self.editor, 'getText'):
            return False
        syspath = sys.path
        sys.path = self.syspath
        text = self.editor.getText()
        text = text.replace('\r\n', '\n')
        text = text.replace('\r', '\n')
        name = self.modulename or self.name
        module = imp.new_module(name)
        newspace = module.__dict__.copy()
        try:
            try:
                code = compile(text, name, 'exec')
            except:
                raise
#                return False
            try:
                exec code in newspace
            except:
                raise
#                return False
            else:
                # No problems, so update the namespace.
                self.interp.locals.clear()
                self.interp.locals.update(newspace)
                return True
        finally:
            sys.path = syspath
            for m in sys.modules.keys():
                if m not in self.modules:
                    del sys.modules[m]
