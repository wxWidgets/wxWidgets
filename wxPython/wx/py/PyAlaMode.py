
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import PyAlaMode
_rename(globals(), PyAlaMode.__dict__, modulename='py.PyAlaMode')
del PyAlaMode
del _rename

if __name__ == '__main__':
    main()
