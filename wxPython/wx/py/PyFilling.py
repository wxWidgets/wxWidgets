
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import PyFilling
_rename(globals(), PyFilling.__dict__, modulename='py.PyFilling')
del PyFilling
del _rename

if __name__ == '__main__':
    main()
