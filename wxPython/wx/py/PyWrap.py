
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import PyWrap
_rename(globals(), PyWrap.__dict__, modulename='py.PyWrap')
del PyWrap
del _rename

if __name__ == '__main__':
    main()
