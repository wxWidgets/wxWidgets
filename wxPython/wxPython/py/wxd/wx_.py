"""Decorator classes for documentation and shell scripting.

Information contained in this module, and all modules imported by this
module, is covered by the wxWindows Free Documentation Licence.  See
the LICENSE.txt file for details.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


_topics = {
    'Accelerators': None,
    'App': None,
    'Base': None,
    'ClipDragDrop': None,
    'Config': None,
    'Controls': None,
    'DataStructures': None,
    'DateTime': None,
    'Dialogs': None,
    'Drawing': None,
    'Errors': None,
    'EventFunctions': None,
    'Events': None,
    'FileSystem': None,
    'Frames': None,
    'Functions': None,
    'Help': None,
    'ImageHandlers': None,
    'Joystick': None,
    'LayoutConstraints': None,
    'Logging': None,
    'Menus': None,
    'MimeTypes': None,
    'Misc': None,
    'Panel': None,
    'Printing': None,
    'Process': None,
    'SashSplitter': None,
    'Sizers': None,
    'Streams': None,
    'Threading': None,
    'ToolBar': None,
    'Tree': None,
    'Validators': None,
    'Window': None,
    }

for topic in _topics.keys():
    _topics[topic] = __import__(topic, globals())
    exec 'from %s import *' % topic

del topic  # Cleanup the namespace.

try:
    del wx  # Cleanup any module that imports Parameters as wx.
except:
    pass
