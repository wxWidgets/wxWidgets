"""wx package

Provides a way to drop the wx prefix from wxPython objects."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx

import types

d_new = globals()
d_old = wx.__dict__

for old, obj in d_old.iteritems():
    if type(obj) is types.ModuleType or old.startswith('_'):
        # Skip modules and private names.
        continue
    new = old
    if old.startswith('EVT_'):
        # Leave name unmodified; add to the new wx namespace.
        d_new[new] = obj
    elif old.startswith('wxEVT_'):
        # Leave name unmodified; add to the new wx namespace.
        d_new[new] = obj
    else:
        if old.startswith('wx'):
            # Remove the 'wx' prefix.
            new = old[2:]
        # Add to the new wx package namespace.
        d_new[new] = obj

del d_new
del d_old
del new
del obj
del old
del types

del wx
