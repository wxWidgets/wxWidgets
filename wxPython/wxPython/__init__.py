# The "old" wxPython package

import warnings
warnings.warn(
    "The wxPython compatibility package is no longer automatically generated "
    "or actively maintained.  Please switch to the wx package as soon as possible.",
    DeprecationWarning, stacklevel=2)

# We need to be able to import from the wx package, but there is also
# a wxPython.wx module and that would normally be chosen first by
# import statements.  So instead we'll have a wxPython._wx module and
# then stuff it into sys.modules with a wxPython.wx alias so old
# programs will still work.

import _wx
import sys
sys.modules['wxPython.wx'] = _wx
wx = _wx
del sys

from wx import __version__
