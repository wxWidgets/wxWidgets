"""Renaming utility.

Provides a way to drop the wx prefix from wxPython objects."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import types

def rename(d_new, d_old):
    for old, obj in d_old.iteritems():
        if type(obj) is types.ModuleType or old.startswith('_'):
            # Skip modules and private names.
            continue
##         mod = d_old['__name__']
##         if hasattr(obj, '__module__') and not obj.__module__.startswith(mod):
##             # Skip objects imported from other modules, except those
##             # related to the current module, such as stc_.
##             continue
        new = old
        if old.startswith('EVT_') or old.startswith('wxEVT_'):
            # Leave these names unmodified.
            pass 
        elif old.startswith('wx'):
            new = old[2:]
        if new:
            d_new[new] = d_old[old]
