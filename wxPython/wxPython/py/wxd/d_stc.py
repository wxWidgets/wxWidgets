"""Decorator utility for documentation and shell scripting.

When you import stc from this module, all of the classes get decorated
with docstrings from our decoration class definitions.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import stc

import stc_

import decorator

decorator.decorate(real=stc, decoration=stc_)
