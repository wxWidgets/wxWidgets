
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import PyShell
_rename(globals(), PyShell.__dict__, modulename='py.PyShell')
del PyShell
del _rename

if __name__ == '__main__':
    main()
