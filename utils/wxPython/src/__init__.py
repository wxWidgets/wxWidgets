#----------------------------------------------------------------------------
# Name:         __init__.py
# Purpose:      The presence of this file turns this directory into a
#               Python package.
#
# Author:       Robin Dunn
#
# Created:      8/8/98
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------
#
# $Log$
# Revision 1.3  1998/12/15 20:41:12  RD
# Changed the import semantics from "from wxPython import *" to "from
# wxPython.wx import *"  This is for people who are worried about
# namespace pollution, they can use "from wxPython import wx" and then
# prefix all the wxPython identifiers with "wx."
#
# Added wxTaskbarIcon for wxMSW.
#
# Made the events work for wxGrid.
#
# Added wxConfig.
#
# Added wxMiniFrame for wxGTK, (untested.)
#
# Changed many of the args and return values that were pointers to gdi
# objects to references to reflect changes in the wxWindows API.
#
# Other assorted fixes and additions.
#
# Revision 1.2  1998/10/07 07:34:32  RD
# Version 0.4.1 for wxGTK
#
# Revision 1.1  1998/08/09 08:25:49  RD
# Initial version
#
#
