
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import PyCrust
_rename(globals(), PyCrust.__dict__, modulename='py.PyCrust')
del PyCrust
del _rename

if __name__ == '__main__':
    main()
