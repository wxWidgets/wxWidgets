"""wx package

Provides a way to drop the wx prefix from wxPython objects by
dynamically loading and renaming objects from the real wxPython
package.  This is the first phase of a transition to a new style of
using wxPython.  For example:

    import wx
    class MyFrame(wx.Frame):
        ...

instead of:

    from wxPython.wx import *
    class MyFrame(wxFrame):
        ...

or:

    from wxPython import wx
    class MyFrame(wx.wxFrame):
        ...


Internally, this package contains only one function, called _rename,
and one dictionary, called _newnames.  These are used by wx itself and
by its sub-modules whenever they are imported.  The _rename function
changes the names in the real wxPython module being imported according
to the rules that have been decided, e.g. most wx prefixes are
removed, and the new names are made visible in the wx package or
sub-packages.

The _newnames dictionary holds the set of new names (from wx and ALL
sub-modules), keyed by the original name. This serves two purposes,
duplicate old names in different modules are eliminated, the survivor
being the name in wx itself; and the dictionary is accessible to
external scripts whose purpose is to change occurrences of the
corresponding names in application programs that use wx.

"""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx

_newnames = {}

def _rename(d_new, d_old, modulename=None):
    " copy the names from d_old to d_new according to the rules"
    global _newnames
    import types
    prefix = 'wx.'
    if modulename:
        prefix += modulename + '.'
    for old, obj in d_old.items():
        if type(obj) is types.ModuleType or old.startswith('_'):
            # Skip modules and private names.
            continue
        if old.startswith('wx') and not old.startswith('wxEVT_'):
            # remove all wx prefixes except wxEVT_
            new = old[2:]
        else:
            # add everything else unchanged
            new = old
        if _newnames.has_key(old):
            d_new[new] = obj
            _newnames[old] = prefix + new      # add fully qualified new name to lookup using old name as key

# rename the wx namespace itself

_rename(globals(), wx.__dict__)
del wx

