#----------------------------------------------------------------------------
# Name:         __init__.py
# Purpose:      The presence of this file turns this directory into a
#               Python package.
#
# Author:       Robin Dunn
#
# Created:      18-May-1999
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from sizer   import *
from box     import *
from border  import *
from shape   import *

#----------------------------------------------------------------------------
_msg = """\
Since the wxWindows library now includes its own sizers, the
classes in wxPython.lib.sizers have been depreciated.  Please
see the Reference Manual for details of the new classes.

To contiunue using wxPython.lib.sizers without this
message you can set the WXP_OLDSIZERS envronment
variable to any value.
"""


import os
from wxPython.wx import wxMessageDialog, wxOK, wxICON_EXCLAMATION, wxPlatform

if not os.environ.has_key('WXP_OLDSIZERS'):
    if wxPlatform == '__WXMSW__':
        dlg = wxMessageDialog(None, _msg,
                              "Depreciated Feature",
                              wxOK | wxICON_EXCLAMATION)
        dlg.ShowModal()
        dlg.Destroy()
    else:
	print '\a'
	print _msg

#----------------------------------------------------------------------------
