"""Document class."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import os


class Document:
    """Document class."""

    def __init__(self, filename=None):
        """Create a Document instance."""
        self.filename = filename
        self.filepath = None
        self.filedir = None
        self.filebase = None
        self.fileext = None
        if self.filename:
            self.filepath = os.path.realpath(self.filename)
            self.filedir, self.filename = os.path.split(self.filepath)
            self.filebase, self.fileext = os.path.splitext(self.filename)

    def read(self):
        """Return contents of file."""
        if self.filepath and os.path.exists(self.filepath):
            f = file(self.filepath, 'rb')
            try:
                return f.read()
            finally:
                f.close()
        else:
            return ''

    def write(self, text):
        """Write text to file."""
        try:
            f = file(self.filepath, 'wb')
            f.write(text)
        finally:
            if f:
                f.close()
