
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import PyAlaCarte
_rename(globals(), PyAlaCarte.__dict__, modulename='py.PyAlaCarte')
del PyAlaCarte
del _rename

if __name__ == '__main__':
    main()
